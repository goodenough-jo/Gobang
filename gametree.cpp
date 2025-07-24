#include "gametree.h"
#include "node.h"
#include "surChessPieces.h"
// #include "fivenaction.h"
// #include <memory>
#include <iostream>
using std::cout;
using std::endl;
#include <algorithm>
#include <cmath>

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
        Node *nChild
            = new Node(node,pos.first,pos.second,choice);
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



    // 处理节点不足的情况
    if (sortedChildren.size() < N) {
        // 添加虚拟节点确保数量足够
        for (int i = sortedChildren.size(); i < N; ++i) {
            Node* dummy = new Node();
            dummy->fX = dummy->fY = 255; // 无效位置
            sortedChildren.push_back(dummy);
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
                cout << "○"; //黑棋
                continue;
            }
            if (nodeBest->board[i][j] == 'W') {
                cout << "●";
                continue;
            }
            if (five == 0) //第五步棋子
            {
                if (nodeSecond != NULL) {
                    if (nodeSecond->board[i][j] == 'B') {
                        cout << "○"; //黑棋
                        continue;
                    }
                    if (nodeSecond->board[i][j] == 'W') {
                        cout << "●";
                        continue;
                    }
                }
                if (nodeThird != NULL) {
                    if (nodeThird->board[i][j] == 'B') {
                        cout << "○"; //黑棋
                        continue;
                    }
                    if (nodeThird->board[i][j] == 'W') {
                        cout << "●";
                        continue;
                    }
                }
                if (nodeFourth != NULL) {
                    if (nodeFourth->board[i][j] == 'B') {
                        cout << "○"; //黑棋
                        continue;
                    }
                    if (nodeFourth->board[i][j] == 'W') {
                        cout << "●";
                        continue;
                    }
                }
                if (nodeFifth != NULL) {
                    if (nodeFifth->board[i][j] == 'B') {
                        cout << "○"; //黑棋
                        continue;
                    }
                    if (nodeFifth->board[i][j] == 'W') {
                        cout << "●";
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
