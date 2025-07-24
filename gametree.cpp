#include "gametree.h"
#include "node.h"
#include "surChessPieces.h"
#include <iostream>
using std::cout;
using std::endl;
#include <cmath>
#include "fivenaction.h"
// #include <memory>

uint8_t max(uint8_t a, uint8_t b)
{
    uint8_t m = a;
    if (b > a) m = b;
    return m;
}
uint8_t min(uint8_t a, uint8_t b)
{
    uint8_t m = a;
    if (b < a) m = b;
    return m;
}

GameTree::GameTree(uint32_t maxDepth, uint8_t radius)
    : maxDepth{maxDepth} // 使用成员初始化列表初始化 maxDepth
    , radius{radius}     // 使用成员初始化列表初始化 radius
{}

GameTree::GameTree(uint32_t maxDepth, uint8_t radius, uint8_t (&board)[15][15]) : maxDepth{maxDepth}, radius{radius}
{
    memcpy(nodeRoot->board, board, sizeof(board));
}


//搜索该节点周围，半径3圈可用的棋子集合
std::vector<std::pair<uint8_t, uint8_t>> GameTree::getSearchNodes(Node *node, char choice)
{
    std::vector<std::pair<uint8_t, uint8_t>> mask;
    bool hasChess = false;
    bool newBoard[15][15];

    // 使用memset初始化newBoard为false(0)
    memset(newBoard, 0, sizeof(newBoard));

    for (uint8_t i = 0; i < 15; ++i) {
        for (uint8_t j = 0; j < 15; ++j) {
            if (node->board[i][j] != 0) { // 如果当前位置有棋子
                hasChess = true;
                // 计算边界，搜索半径为3--------------------------------------
                uint8_t xStart = i > 2 ? i - 2 : 0;
                uint8_t xEnd = i < 12 ? i + 2 : 14;
                uint8_t yStart = j > 2 ? j - 2 : 0;
                uint8_t yEnd = j < 12 ? j + 2 : 14;

                for (uint8_t x = xStart; x <= xEnd; ++x) {
                    for (uint8_t y = yStart; y <= yEnd; ++y) {
                        if (node->board[x][y] == 0) { // 如果没有棋子,作为可用棋子集合：不会产生黑棋禁手/执白棋的一方
                            if (choice != '0' || ForbiddenCheck(node->board, x, y) == NO_FORBIDDEN) {
                                newBoard[x][y] = true;
                            }
                        }
                    }
                }
            }
        }
    }

    if (!hasChess) {
        mask.emplace_back(7, 7); // 如果没有棋子，只添加中心点
    } else {
        uint8_t value = 0;
        for (uint8_t i = 0; i < 15; ++i) {
            for (uint8_t j = 0; j < 15; ++j) {
                if (newBoard[i][j]) {
                    value++;
                    //cout << "(x,y): " << (int)i <<","<< (int)j << "("<< (int)value << ")\n";
                    mask.emplace_back(i, j);
                }
            }
        }
    }

    return mask;
}

//将集合放入该节点的children和openTable中
uint8_t GameTree::expandChildrenNodes(Node *node, char choice)
{
    //调用getSearchNodes(node)获取待扩展点集合，存入变量temp
    vector<pair<uint8_t, uint8_t>> temp = getSearchNodes(node, choice);
    // 使用基于范围的 for 循环遍历 temp,pos会依次成为 temp 容器中的每一个元素
    for (auto pos : temp) {
        //新建当前棋局node的子节点nChild, nChild的最后落子点为node的待扩展点的坐标
        Node *nChild = new Node(node,pos.first,pos.second,choice);
        //pos.first = pos.x; pos.second = pos.y 若node为根节点，则(pos.first, pos.second) = 'B'
        //if(pos.first == 7 && pos.second == 7) printf("the first node's depth is %d\n", nChild->depth);
        //在节点node的子节点中加入节点n
        node->children.insert(nChild);
        //将节点n放在openTable的前端
        openTable.push_front(nChild);
    }
    return temp.size();
}


//MCTS初始化
GameTree::GameTree(uint32_t maxDepth, uint8_t radius, int iterations, double exploration, uint8_t (&board)[15][15])
    : maxDepth{maxDepth}
    , radius{radius}
    , maxIterations{iterations}
    , exploration{exploration}
{
    memcpy(nodeRoot->board, board, sizeof(board));
}

//MCTS----select
Node *GameTree::select(Node *node)
{
    while (!node->children.empty()) {
        // 如果有未充分探索的子节点，优先选择
        for (Node* child : node->children) {
            if (child->visitCount == 0) {
                return child;
            }
        }
        // 否则选择UCT值最高的节点
        Node *bestChild = nullptr;
        double bestScore = -std::numeric_limits<double>::max();

        for (Node *child : node->children) {
            double exploit = child->totalReward / child->visitCount;
            //double exploit = (node->player == child->player) ? child->totalReward / child->visitCount : 1.0 - (child->totalReward / child->visitCount);
            double explore = exploration * std::sqrt(std::log(node->visitCount) / child->visitCount);
            double score = exploit + explore;
            //MCTS公式计算

            if (score > bestScore) {
                bestScore = score;
                bestChild = child;
            }
        }
        node = bestChild;
    }
    return node;
}

//MCTS----expand
Node *GameTree::expand(Node *node, char choice)
{
    if (node->boardIdentify() != 0) { // 游戏结束不扩展
        return node;
    }

    // 特别处理根节点：一次性扩展所有子节点
    if(node==nodeRoot){
        vector<pair<uint8_t, uint8_t>> moves = getSearchNodes(node, choice);
        for (auto &move : moves) {
            // 跳过已存在子节点
            bool exists = false;
            for (Node* child : node->children) {
                if (child->fX == move.first && child->fY == move.second) {
                    exists = true;
                    break;
                }
            }
            if (!exists) {
                Node* child = new Node(node, move.first, move.second, choice);
                node->children.insert(child);
            }
        }
        return node; // 返回根节点本身
    }

    vector<pair<uint8_t, uint8_t>> moves = getSearchNodes(node, choice);
    vector<pair<uint8_t, uint8_t>> unexploredMoves;

    // 找出未探索的动作
    for (auto &move : moves) {
        bool found = false;
        for (Node *child : node->children) {
            cout << "123432 child X,Y: " << (int)child->fX << (int)child->fY << endl;
            cout << "134321 move X,Y: " << (int)move.first << (int)move.second << endl;
            if (child->fX == move.first && child->fY == move.second) {
                found = true;
                break;
            }
        }
        if (!found) {
            unexploredMoves.push_back(move);
        }
    }

    //如果存在未探索的节点，随机选一个动作创建新节点
    if (!unexploredMoves.empty()) {
        // 随机选择一个未探索的动作
        int randomIndex = rand() % unexploredMoves.size();
        auto move = unexploredMoves[randomIndex];
        Node* child = new Node(node, move.first, move.second, choice);
        node->children.insert(child);
        return child;
    }
    return node; // 没有未探索动作
}

//MCTS----simulate
double GameTree::simulate(Node* node, char choice) {
    // 设置根节点玩家
    uint8_t rootPlayer = (choice == 0) ? 'B' : 'W';
    // 1. 检查是否终局
    uint8_t result = node->boardIdentify();
    if (result == 'B') {
        return (rootPlayer == 'B') ? 1.0 : 0.0; // 黑胜
    } else if (result == 'W') {
        return (rootPlayer == 'W') ? 1.0 : 0.0; // 白胜
    }

    // 2. 使用评估函数获取局面价值
    node->evaluate(choice);

    // 3. 将评估值转换为胜率概率 [0, 1]
    const double k = 0.001; // 缩放因子，控制sigmoid曲线形状
    double rawValue = static_cast<double>(node->value);
    double reward = 1.0 / (1.0 + std::exp(-k * rawValue));

    // // 4. 调整奖励值到合理范围?不是很必要吧
    // reward = std::max(0.0, std::min(1.0, reward)); // 限制在[0,1]区间

    // 5. 考虑当前玩家视角
    if ((node->depth % 2 == 0 && rootPlayer == 'W') || (node->depth % 2 == 1 && rootPlayer == 'B')) {
        reward = 1.0 - reward; // 对手视角转换
    }

    return reward;
}

//MCTS----backPropagate
void GameTree::backPropagate(Node* node, double reward) {
    while (node != nullptr) {
        // 1. 更新节点统计
        node->visitCount++;
        node->totalReward += reward;
        // 2. 反向传播奖励
        reward = 1.0 - reward; // 对手视角转换
        // 3. 移动到父节点
        node = node->father;
    }
}


/*具有保留价值，优化的思路
void GameTree::setNextPos(int five, int N)
{
    std::cout << "Root children count: " << nodeRoot->children.size() << "\n";
    std::cout << "Available moves: " << getSearchNodes(nodeRoot, '0').size() << "\n";
    // 根据访问次数排序子节点
    vector<Node*> sortedChildren(nodeRoot->children.begin(), nodeRoot->children.end());
    sort(sortedChildren.begin(), sortedChildren.end(), [](Node* a, Node* b) { return a->visitCount > b->visitCount; });

    // 重置指针
    nodeBest = nodeSecond = nodeThird = nodeFourth = nodeFifth = nullptr;

    // 选择前N个节点
    if (!sortedChildren.empty()) nodeBest = sortedChildren[0];
    if (N > 1 && sortedChildren.size() > 1) nodeSecond = sortedChildren[1];
    if (N > 2 && sortedChildren.size() > 2) nodeThird = sortedChildren[2];
    if (N > 3 && sortedChildren.size() > 3) nodeFourth = sortedChildren[3];
    if (N > 4 && sortedChildren.size() > 4) nodeFifth = sortedChildren[4];
}
*/

/*原本算法
bool GameTree::isAlphaBetaCut(Node *node)
{
    //node是空节点或根节点
    if (node == nullptr || node->father == nullptr) return false;
    //node是MAX节点
    if (node->isMaxNode() && node->value > node->father->value) return true;
    //node是MIN节点
    if (!node->isMaxNode() && node->value < node->father->value) return true;
    //node为叶子节点
    return isAlphaBetaCut(node->father); //判断其父节点能否剪枝
}

void GameTree::updateValueFromNode(Node *node)
{
    //node为空节点
    if (node == nullptr) return;
    // node为叶子节点
    if (node->children.empty()) {
        updateValueFromNode(node->father);
        return;
    }
    //node是MAX节点 maxValue是node所有子节点的最大估值
    if (node->isMaxNode()) {
        int32_t cntValue = INT32_MIN;
        for (Node *n : node->children)
            if (n->value != INT32_MAX) cntValue = max(cntValue, n->value);
        if (cntValue > node->value) {
            node->value = cntValue;
            updateValueFromNode(node->father);
        }
    } else { //node是MIN节点 minValue是node所有子节点的最小估值
        int32_t cntValue = INT32_MAX;
        for (Node *n : node->children)
            if (n->value != INT32_MIN) cntValue = min(cntValue, n->value);
        if (cntValue < node->value) {
            node->value = cntValue;
            updateValueFromNode(node->father);
        }
    }
}
*/

Node *GameTree::findNodeByPosition(uint8_t x, uint8_t y)
{
    for (Node *n : nodeRoot->children) {
        if (n->fX == x && n->fY == y) { return n; }
    }
    return nullptr;
}

//寻找下一步的最佳落棋点，即寻找根节点的最佳子节点
void GameTree::setNextPos(int five, int N) //five = 0表示5手交换 非0的整数表示不五手N打 n表示五手n打
{
    //    初始化nodeBest为nodeRoot的第一个子节点;
    nodeBest = *nodeRoot->children.begin();
    //遍历子节点 寻找最大值
    for (Node *n : nodeRoot->children) {
        if (n->value > nodeBest->value) { nodeBest = n; }
    }

    //若处于五手打N中
    if (five == 0) {
        fiveNAction fna;
        std::vector<std::pair<int, int>> currentBoard = this->nodeRoot->getCurrentBoard(); //获取当前棋盘所有点的位置
        fna.getBlackPostions(this->nodeRoot->board);
        // //-------调试 判断当前棋盘的点是否合理
        // std::cout << "打印未打点前棋盘的坐标:\n";
        // for (std::pair<int, int> i : currentBoard) {
        //     cout << "(" << (uint8_t) (i.second + 'A') << "," << 15 - i.first << ")" << endl;
        //     std::cout << i.second << ", " << i.first << "\n";
        // }
        // //调试:打印当前棋盘黑子的坐标
        // std::cout << "打印未打点前黑子的坐标:\n";
        // for (std::pair<int, int> i : fna.blackPositions) {
        //     cout << "(" << (uint8_t) (i.second + 'A') << "," << 15 - i.first << ")" << endl;
        //     std::cout << i.second << ", " << i.first << "\n";
        // }
        //------------------

        // 调用FiveNAction的isSymmetric函数判断当前棋盘是否对称
        fna.getSymmetricPoint(currentBoard);

        // //------------调试 计算重心,以棋盘的x,y为准
        // std::cout << "重心:\n";
        // std::cout << fna.symmetricPoint.second << ", " << fna.symmetricPoint.first << "\n";
        //-------------
        //先判断整个棋局是否对称,再判断重心与黑子对称点
        if (fna.isSymmetric(currentBoard)) {
            //将nodeBest加入currentBoard中
            currentBoard.push_back({nodeBest->fX, nodeBest->fY});
            //计算黑子的对称点是否与整个棋盘的重心相同
            fna.getBlackSymmetricPoint();
            // std::cout << "黑子对称点:" << fna.blackSymmetricPoint.second << ", " << fna.blackSymmetricPoint.first
            //           << "\n";
            //如果重心与黑子的对称点相同,则直接使用棋盘的对称
            if (fna.symmetricPoint == fna.blackSymmetricPoint) {
                // std::cout << "黑子与整个棋盘的对称点相同.\n\n";
                switch (N) {
                case 2: {
                    // //-------调试信息:检查此时currentBoard存放的值
                    // std::cout << "case 2:nodeBest与currentBoard的坐标:\n";
                    // for (std::pair<int, int> i : currentBoard) {
                    //     cout << "(" << (uint8_t) (i.second + 'A') << "," << 15 - i.first << ")" << endl;
                    //     std::cout << i.second << ", " << i.first << "\n";
                    // }
                    // //------------
                    //遍历子节点 寻找第二大值
                    nodeSecond = *nodeRoot->children.begin();
                    for (Node *n : nodeRoot->children) {
                        //---------调试信息,检验是否能正确判断对称

                        // cout << "n->value: " << n->value << "\n";
                        // cout << "(" << (uint8_t) (n->fY + 'A') << "," << 15 - n->fX << ")" << endl;
                        // cout << (int) n->fY << ", " << (int) n->fX << "\n\n";
                        //----------------
                        if (n->value == nodeBest->value) continue;
                        // 构造一个临时棋盘，加入这个候选打点
                        std::vector<std::pair<int, int>> testBoard = currentBoard;
                        testBoard.push_back({n->fX, n->fY});

                        if ((n->value < nodeBest->value) && (n->value > nodeSecond->value)
                            && (!fna.isSymmetric(testBoard)))
                            nodeSecond = n;
                    }

                    break;
                }
                case 3: {
                    //遍历子节点 寻找第二大值
                    nodeSecond = *nodeRoot->children.begin();
                    for (Node *n : nodeRoot->children)
                        if ((n->value < nodeBest->value) && (n->value > nodeSecond->value)) nodeSecond = n;
                    //当N==3时,currentBoard在加入nodeBest的前提下加入nodeSecond;nodeBest已经在if之前swich之后加入
                    currentBoard.push_back({nodeSecond->fX, nodeSecond->fY});

                    //遍历子节点 寻找第三大值
                    nodeThird = *nodeRoot->children.begin();
                    for (Node *n : nodeRoot->children) {
                        if (n->value == nodeBest->value || n->value == nodeSecond->value) continue;

                        // 构造一个临时棋盘，加入这个候选打点
                        std::vector<std::pair<int, int>> testBoard = currentBoard;
                        testBoard.push_back({n->fX, n->fY});
                        if ((n->value < nodeBest->value) && (n->value < nodeSecond->value)
                            && (n->value > nodeThird->value) && (!fna.isSymmetric(testBoard)))
                            nodeThird = n;
                    }
                    break;
                }
                case 4: {
                    //遍历子节点 寻找第二大值
                    nodeSecond = *nodeRoot->children.begin();
                    for (Node *n : nodeRoot->children)
                        if ((n->value < nodeBest->value) && (n->value > nodeSecond->value)) nodeSecond = n;
                    currentBoard.push_back({nodeSecond->fX, nodeSecond->fY});
                    //遍历子节点 寻找第三大值
                    nodeThird = *nodeRoot->children.begin();
                    for (Node *n : nodeRoot->children)
                        if ((n->value < nodeBest->value) && (n->value < nodeSecond->value)
                            && (n->value > nodeThird->value))
                            nodeThird = n;
                    currentBoard.push_back({nodeThird->fX, nodeThird->fY});
                    //遍历子节点 寻找第四大值
                    nodeFourth = *nodeRoot->children.begin();
                    for (Node *n : nodeRoot->children) {
                        if (n->value == nodeBest->value || n->value == nodeSecond->value || n->value == nodeThird->value)
                            continue;

                        // 构造一个临时棋盘，加入这个候选打点
                        std::vector<std::pair<int, int>> testBoard = currentBoard;
                        if ((n->value < nodeBest->value) && (n->value < nodeSecond->value)
                            && (n->value < nodeThird->value) && (n->value > nodeFourth->value)
                            && (!fna.isSymmetric(testBoard)))
                            nodeFourth = n;
                    }
                    break;
                }
                case 5: {
                    //遍历子节点 寻找第二大值
                    nodeSecond = *nodeRoot->children.begin();
                    for (Node *n : nodeRoot->children)
                        if ((n->value < nodeBest->value) && (n->value > nodeSecond->value)) nodeSecond = n;
                    currentBoard.push_back({nodeSecond->fX, nodeSecond->fY});

                    //遍历子节点 寻找第三大值
                    nodeThird = *nodeRoot->children.begin();
                    for (Node *n : nodeRoot->children)
                        if ((n->value < nodeBest->value) && (n->value < nodeSecond->value)
                            && (n->value > nodeThird->value))
                            nodeThird = n;
                    currentBoard.push_back({nodeThird->fX, nodeThird->fY});
                    //遍历子节点 寻找第四大值
                    nodeFourth = *nodeRoot->children.begin();
                    for (Node *n : nodeRoot->children)
                        if ((n->value < nodeBest->value) && (n->value < nodeSecond->value)
                            && (n->value < nodeThird->value) && (n->value > nodeFourth->value))
                            nodeFourth = n;
                    currentBoard.push_back({nodeFourth->fX, nodeFourth->fY});
                    //遍历子节点 寻找第五大值
                    nodeFifth = *nodeRoot->children.begin();
                    for (Node *n : nodeRoot->children) {
                        if (n->value == nodeBest->value || n->value == nodeSecond->value || n->value == nodeThird->value
                            || n->value == nodeFourth->value)
                            continue;

                        // 构造一个临时棋盘，加入这个候选打点
                        std::vector<std::pair<int, int>> testBoard = currentBoard;
                        if ((n->value < nodeBest->value) && (n->value < nodeSecond->value)
                            && (n->value < nodeThird->value) && (n->value < nodeFourth->value)
                            && (n->value > nodeFifth->value) && (!fna.isSymmetric(testBoard)))
                            nodeFifth = n;
                    }
                    break;
                }
                }
            } else {
                // std::cout << "黑子与整个棋盘的对称点不相同.\n\n";
                //如果不同,则需要利用黑子的对称与棋盘的对称
                //将nodeBest加入currentBoard中
                fna.blackPositions.push_back({nodeBest->fX, nodeBest->fY});
                switch (N) {
                case 2: {
                    //遍历子节点 寻找第二大值
                    nodeSecond = *nodeRoot->children.begin();
                    for (Node *n : nodeRoot->children) {
                        //---------调试信息,检验是否能正确判断对称

                        cout << "n->value: " << n->value << "\n";
                        cout << "(" << (uint8_t) (n->fY + 'A') << "," << 15 - n->fX << ")" << endl;
                        cout << (int) n->fY << ", " << (int) n->fX << "\n\n";
                        //----------------
                        if (n->value == nodeBest->value) continue;
                        //------防止对称 及防止黑棋对称与棋盘对称的情况同时出现
                        // 构造一个临时棋盘，加入这个候选打点
                        std::vector<std::pair<int, int>> testBoard = currentBoard;
                        std::vector<std::pair<int, int>> testBlackBoard = fna.blackPositions;
                        testBoard.push_back({n->fX, n->fY});
                        testBlackBoard.push_back({n->fX, n->fY});
                        //-------------------
                        if ((n->value < nodeBest->value) && (n->value > nodeSecond->value)
                            && (!(fna.isSymmetric(testBoard) && fna.isBlackSymmetric(testBlackBoard))))
                            nodeSecond = n;
                    }

                    break;
                }
                case 3: {
                    //遍历子节点 寻找第二大值
                    nodeSecond = *nodeRoot->children.begin();
                    for (Node *n : nodeRoot->children)
                        if ((n->value < nodeBest->value) && (n->value > nodeSecond->value)) nodeSecond = n;
                    currentBoard.push_back({nodeSecond->fX, nodeSecond->fY});
                    fna.blackPositions.push_back({nodeSecond->fX, nodeSecond->fY});

                    //遍历子节点 寻找第三大值
                    nodeThird = *nodeRoot->children.begin();
                    for (Node *n : nodeRoot->children) {
                        if (n->value == nodeBest->value || n->value == nodeSecond->value) continue;

                        // 构造一个临时棋盘，加入这个候选打点
                        std::vector<std::pair<int, int>> testBoard = currentBoard;
                        std::vector<std::pair<int, int>> testBlackBoard = fna.blackPositions;
                        testBoard.push_back({n->fX, n->fY});
                        testBlackBoard.push_back({n->fX, n->fY});
                        if ((n->value < nodeBest->value) && (n->value < nodeSecond->value)
                            && (n->value > nodeThird->value)
                            && (!(fna.isSymmetric(testBoard) && fna.isBlackSymmetric(testBlackBoard))))
                            nodeThird = n;
                    }
                    break;
                }
                case 4: {
                    //遍历子节点 寻找第二大值
                    nodeSecond = *nodeRoot->children.begin();
                    for (Node *n : nodeRoot->children)
                        if ((n->value < nodeBest->value) && (n->value > nodeSecond->value)) nodeSecond = n;
                    currentBoard.push_back({nodeSecond->fX, nodeSecond->fY});
                    fna.blackPositions.push_back({nodeSecond->fX, nodeSecond->fY});
                    //遍历子节点 寻找第三大值
                    nodeThird = *nodeRoot->children.begin();
                    for (Node *n : nodeRoot->children)
                        if ((n->value < nodeBest->value) && (n->value < nodeSecond->value)
                            && (n->value > nodeThird->value))
                            nodeThird = n;
                    currentBoard.push_back({nodeThird->fX, nodeThird->fY});
                    fna.blackPositions.push_back({nodeThird->fX, nodeThird->fY});
                    //遍历子节点 寻找第四大值
                    nodeFourth = *nodeRoot->children.begin();
                    for (Node *n : nodeRoot->children) {
                        if (n->value == nodeBest->value || n->value == nodeSecond->value || n->value == nodeThird->value)
                            continue;

                        // 构造一个临时棋盘，加入这个候选打点
                        std::vector<std::pair<int, int>> testBoard = currentBoard;
                        std::vector<std::pair<int, int>> testBlackBoard = fna.blackPositions;
                        testBlackBoard.push_back({n->fX, n->fY});
                        if ((n->value < nodeBest->value) && (n->value < nodeSecond->value)
                            && (n->value < nodeThird->value) && (n->value > nodeFourth->value)
                            && (!(fna.isSymmetric(testBoard) && fna.isBlackSymmetric(testBlackBoard))))
                            nodeFourth = n;
                    }
                    break;
                }
                case 5: {
                    //遍历子节点 寻找第二大值
                    nodeSecond = *nodeRoot->children.begin();
                    for (Node *n : nodeRoot->children)
                        if ((n->value < nodeBest->value) && (n->value > nodeSecond->value)) nodeSecond = n;
                    currentBoard.push_back({nodeSecond->fX, nodeSecond->fY});
                    fna.blackPositions.push_back({nodeSecond->fX, nodeSecond->fY});
                    //遍历子节点 寻找第三大值
                    nodeThird = *nodeRoot->children.begin();
                    for (Node *n : nodeRoot->children)
                        if ((n->value < nodeBest->value) && (n->value < nodeSecond->value)
                            && (n->value > nodeThird->value))
                            nodeThird = n;
                    currentBoard.push_back({nodeThird->fX, nodeThird->fY});
                    fna.blackPositions.push_back({nodeThird->fX, nodeThird->fY});
                    //遍历子节点 寻找第四大值
                    nodeFourth = *nodeRoot->children.begin();
                    for (Node *n : nodeRoot->children)
                        if ((n->value < nodeBest->value) && (n->value < nodeSecond->value)
                            && (n->value < nodeThird->value) && (n->value > nodeFourth->value))
                            nodeFourth = n;
                    currentBoard.push_back({nodeFourth->fX, nodeFourth->fY});
                    fna.blackPositions.push_back({nodeFourth->fX, nodeFourth->fY});
                    //遍历子节点 寻找第五大值
                    nodeFifth = *nodeRoot->children.begin();
                    for (Node *n : nodeRoot->children) {
                        if (n->value == nodeBest->value || n->value == nodeSecond->value || n->value == nodeThird->value
                            || n->value == nodeFourth->value)
                            continue;

                        // 构造一个临时棋盘，加入这个候选打点
                        std::vector<std::pair<int, int>> testBoard = currentBoard;
                        std::vector<std::pair<int, int>> testBlackBoard = fna.blackPositions;
                        testBlackBoard.push_back({n->fX, n->fY});
                        if ((n->value < nodeBest->value) && (n->value < nodeSecond->value)
                            && (n->value < nodeThird->value) && (n->value < nodeFourth->value)
                            && (n->value > nodeFifth->value)
                            && (!(fna.isSymmetric(testBoard) && fna.isBlackSymmetric(testBlackBoard))))
                            nodeFifth = n;
                    }
                    break;
                }
                }
            }

        } else {
            switch (N) {
            case 2: {
                //遍历子节点 寻找第二大值
                nodeSecond = *nodeRoot->children.begin();
                for (Node *n : nodeRoot->children)
                    if ((n->value < nodeBest->value) && (n->value > nodeSecond->value)) nodeSecond = n;
                break;
            }
            case 3: {
                //遍历子节点 寻找第二大值
                nodeSecond = *nodeRoot->children.begin();
                for (Node *n : nodeRoot->children)
                    if ((n->value < nodeBest->value) && (n->value > nodeSecond->value)) nodeSecond = n;
                //遍历子节点 寻找第三大值
                nodeThird = *nodeRoot->children.begin();
                for (Node *n : nodeRoot->children)
                    if ((n->value < nodeBest->value) && (n->value < nodeSecond->value) && (n->value > nodeThird->value))
                        nodeThird = n;
                break;
            }
            case 4: {
                //遍历子节点 寻找第二大值
                nodeSecond = *nodeRoot->children.begin();
                for (Node *n : nodeRoot->children)
                    if ((n->value < nodeBest->value) && (n->value > nodeSecond->value)) nodeSecond = n;
                //遍历子节点 寻找第三大值
                nodeThird = *nodeRoot->children.begin();
                for (Node *n : nodeRoot->children)
                    if ((n->value < nodeBest->value) && (n->value < nodeSecond->value) && (n->value > nodeThird->value))
                        nodeThird = n;
                //遍历子节点 寻找第四大值
                nodeFourth = *nodeRoot->children.begin();
                for (Node *n : nodeRoot->children)
                    if ((n->value < nodeBest->value) && (n->value < nodeSecond->value) && (n->value < nodeThird->value)
                        && (n->value > nodeFourth->value))
                        nodeFourth = n;
                break;
            }
            case 5: {
                //遍历子节点 寻找第二大值
                nodeSecond = *nodeRoot->children.begin();
                for (Node *n : nodeRoot->children)
                    if ((n->value < nodeBest->value) && (n->value > nodeSecond->value)) nodeSecond = n;
                //遍历子节点 寻找第三大值
                nodeThird = *nodeRoot->children.begin();
                for (Node *n : nodeRoot->children)
                    if ((n->value < nodeBest->value) && (n->value < nodeSecond->value) && (n->value > nodeThird->value))
                        nodeThird = n;
                //遍历子节点 寻找第四大值
                nodeFourth = *nodeRoot->children.begin();
                for (Node *n : nodeRoot->children)
                    if ((n->value < nodeBest->value) && (n->value < nodeSecond->value) && (n->value < nodeThird->value)
                        && (n->value > nodeFourth->value))
                        nodeFourth = n;
                //遍历子节点 寻找第五大值
                nodeFifth = *nodeRoot->children.begin();
                for (Node *n : nodeRoot->children)
                    if ((n->value < nodeBest->value) && (n->value < nodeSecond->value) && (n->value < nodeThird->value)
                        && (n->value < nodeFourth->value) && (n->value > nodeFifth->value))
                        nodeFifth = n;
                break;
            }
            }
        }
    }
}

uint8_t GameTree::game(char choice, int five, int N)
{
    // 如果已经分出胜负，直接返回结果
    uint8_t result = nodeRoot->boardIdentify();
    if (result == 'B' || result == 'W') return result;

    // 先完全扩展根节点
    expand(nodeRoot, choice);

    for (int i = 0; i < maxIterations; ++i) {
        Node *selected = select(nodeRoot);
        // 根节点已被完全扩展，直接从子节点开始
        if (selected == nodeRoot && !nodeRoot->children.empty()) {
            selected = *nodeRoot->children.begin();
        }
        Node *expanded = expand(selected, choice);
        double simulationResult = simulate(expanded, choice);
        backPropagate(expanded, simulationResult);
    }

    getNextPos(five, N);

    /*原本的版本
    // 初始化openTable和closedTable
    openTable.push_back(nodeRoot);

    while (!openTable.empty()) {
        // 取出未扩展节点队列的首元素
        Node *node = openTable.front();
        openTable.pop_front();
        closedTable.push_back(node);

        // 如果需要剪枝，则跳过当前节点
        if (isAlphaBetaCut(node->father)) continue;

        // 如果节点未达到最大深度，则扩展子节点
        if (node->depth < maxDepth) {
            uint8_t numChildren = expandChildrenNodes(node, choice);
            if (numChildren == 0) {
                // 没有子节点，直接进行评估
                node->evaluate(choice);
                updateValueFromNode(node);
            } else {
                // 有子节点，继续循环
                continue;
            }
        } else {
            // 到达最大深度，进行评估
            node->evaluate(choice);
            updateValueFromNode(node);
        }
    }
    // 寻找最佳子节点并设置下一步位置
    setNextPos(five, N);
    */

    return 0;
}

//得到最佳落棋点的位置值
std::vector<std::pair<uint8_t, uint8_t>> GameTree::getNextPos(int five, int n)
{
    vector<pair<uint8_t, uint8_t>> mask;
    setNextPos(five, n);

    // 安全添加节点坐标
    auto safeAdd = [&](Node* node) {
        mask.push_back(node? make_pair(node->fX, node->fY) :make_pair(uint8_t(255), uint8_t(255)));
    };

    if (five) { // 非五手N打
        safeAdd(nodeBest);
    } else {    // 五手N打
        safeAdd(nodeBest);
        if (n > 1) safeAdd(nodeSecond);
        if (n > 2) safeAdd(nodeThird);
        if (n > 3) safeAdd(nodeFourth);
        if (n > 4) safeAdd(nodeFifth);
    }

    return mask;
}
//打印最后落子点
void GameTree::showNextPos(int five, int N)
{
    //打印最佳落子点之前判定胜负
    if (five) {
        if (nodeBest == nullptr)
            cout << "(255, 255)" << endl;
        else {
            cout << "机器的落子点位置为(" << (uint8_t) (nodeBest->fY + 'A') << "," << 15 - nodeBest->fX << ")" << endl;
        }
    } else if (five == 0) {
        //五手n打时机器打点
        switch (N) {
        case 2: {
            cout << "机器的落子点位置为(" << (uint8_t) (nodeBest->fY + 'A') << "," << 15 - nodeBest->fX << ")" << endl;
            cout << "机器的落子点位置为(" << (uint8_t) (nodeSecond->fY + 'A') << "," << 15 - nodeSecond->fX << ")"
                 << endl;
            break;
        }
        case 3: {
            cout << "机器的落子点位置为(" << (uint8_t) (nodeBest->fY + 'A') << "," << 15 - nodeBest->fX << ")" << endl;
            cout << "机器的落子点位置为(" << (uint8_t) (nodeSecond->fY + 'A') << "," << 15 - nodeSecond->fX << ")"
                 << endl;
            cout << "机器的落子点位置为(" << (uint8_t) (nodeThird->fY + 'A') << "," << 15 - nodeThird->fX << ")"
                 << endl;
            break;
        }
        case 4: {
            cout << "机器的落子点位置为(" << (uint8_t) (nodeBest->fY + 'A') << "," << 15 - nodeBest->fX << ")" << endl;
            cout << "机器的落子点位置为(" << (uint8_t) (nodeSecond->fY + 'A') << "," << 15 - nodeSecond->fX << ")"
                 << endl;
            cout << "机器的落子点位置为(" << (uint8_t) (nodeThird->fY + 'A') << "," << 15 - nodeThird->fX << ")"
                 << endl;
            cout << "机器的落子点位置为(" << (uint8_t) (nodeFourth->fY + 'A') << "," << 15 - nodeFourth->fX << ")"
                 << endl;
            break;
        }
        case 5: {
            cout << "机器的落子点位置为(" << (uint8_t) (nodeBest->fY + 'A') << "," << 15 - nodeBest->fX << ")" << endl;
            cout << "机器的落子点位置为(" << (uint8_t) (nodeSecond->fY + 'A') << "," << 15 - nodeSecond->fX << ")"
                 << endl;
            cout << "机器的落子点位置为(" << (uint8_t) (nodeThird->fY + 'A') << "," << 15 - nodeThird->fX << ")"
                 << endl;
            cout << "机器的落子点位置为(" << (uint8_t) (nodeFourth->fY + 'A') << "," << 15 - nodeFourth->fX << ")"
                 << endl;
            cout << "机器的落子点位置为(" << (uint8_t) (nodeFifth->fY + 'A') << "," << 15 - nodeFifth->fX << ")"
                 << endl;
            break;
        }
        }
    }
}

//显示棋盘
uint8_t GameTree::showBoard(int five)
{
    if (nodeBest == nullptr) {
        nodeBest = nodeRoot; //nodeRoot深度为0
    } //nodeBest指向最佳子节点

    uint8_t row = 0;
    for (uint8_t i = 0; i < 15; i++) {
        if (row >= 6) cout << " ";
        cout << uint32_t(16 - (++row)) << " ";
        for (uint8_t j = 0; j < 15; j++) {
            if (j != 0) cout << "─";
            if (nodeBest->board[i][j] == 'B') {
                // cout << "○"; //黑棋
                cout << "●"; //黑棋
                continue;
            }
            if (nodeBest->board[i][j] == 'W') {
                // cout << "●";
                cout << "○"; //白旗
                continue;
            }
            if (five == 0) //第五步棋子
            {
                if (nodeSecond != NULL) {
                    if (nodeSecond->board[i][j] == 'B') {
                        // cout << "○"; //黑棋
                        cout << "●"; //黑棋
                        continue;
                    }
                    if (nodeSecond->board[i][j] == 'W') {
                        // cout << "●";
                        cout << "○"; //白旗
                        continue;
                    }
                }
                if (nodeThird != NULL) {
                    if (nodeThird->board[i][j] == 'B') {
                        // cout << "○"; //黑棋
                        cout << "●"; //黑棋
                        continue;
                    }
                    if (nodeThird->board[i][j] == 'W') {
                        // cout << "●";
                        cout << "○";
                        continue;
                    }
                }
                if (nodeFourth != NULL) {
                    if (nodeFourth->board[i][j] == 'B') {
                        // cout << "○"; //黑棋
                        cout << "●";
                        continue;
                    }
                    if (nodeFourth->board[i][j] == 'W') {
                        // cout << "●";
                        cout << "○";
                        continue;
                    }
                }
                if (nodeFifth != NULL) {
                    if (nodeFifth->board[i][j] == 'B') {
                        // cout << "○"; //黑棋
                        cout << "●";
                        continue;
                    }
                    if (nodeFifth->board[i][j] == 'W') {
                        // cout << "●";
                        cout << "○";
                        continue;
                    }
                }
            }
            if (i == 0 && j == 0) {
                cout << "┌";
                continue;
            }
            if (i == 0 && j == 14) {
                cout << "┐";
                continue;
            }
            if (i == 14 && j == 0) {
                cout << "└";
                continue;
            }
            if (i == 14 && j == 14) {
                cout << "┘";
                continue;
            }
            if (i == 0) {
                cout << "┬";
                continue;
            }
            if (i == 14) {
                cout << "┴";
                continue;
            }
            if (j == 0) {
                cout << "├";
                continue;
            }
            if (j == 14) {
                cout << "┤";
                continue;
            }
            cout << "┼";
        }
        cout << endl;
    }
    cout << "   A B C D E F G H I J K L M N O" << endl;
    cout << endl;
    uint8_t result = nodeBest->boardIdentify(); //分出胜负，'B'(黑)/'W'(白)/0(判断不出)
    if (result == 'B') return 'B';
    if (result == 'W') return 'W';
    return 0;
}
