#include "gametree.h"
#include "node.h"
#include "surChessPieces.h"
#include "fivenaction.h"
#include <iostream>
using std::cout;
using std::endl;
#include <memory>

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
                // 计算边界，搜索半径为3
                uint8_t xStart = i > 2 ? i - 2 : 0;
                uint8_t xEnd = i < 12 ? i + 2 : 14;
                uint8_t yStart = j > 2 ? j - 2 : 0;
                uint8_t yEnd = j < 12 ? j + 2 : 14;

                for (uint8_t x = xStart; x <= xEnd; ++x) {
                    for (uint8_t y = yStart; y <= yEnd; ++y) {
                        if (node->board[x][y] == 0) { // 如果没有棋子
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
        for (uint8_t i = 0; i < 15; ++i) {
            for (uint8_t j = 0; j < 15; ++j) {
                if (newBoard[i][j]) { mask.emplace_back(i, j); }
            }
        }
    }

    return mask;
}

uint8_t GameTree::expandChildrenNodes(Node *node, char choice)
{
    //调用getSearchNodes(node)获取待扩展点集合，存入变量temp
    vector<pair<uint8_t, uint8_t>> temp = getSearchNodes(node, choice);
    // 使用基于范围的 for 循环遍历 temp,pos会依次成为 temp 容器中的每一个元素
    for (auto pos : temp) {
        //新建当前棋局node的子节点nChild, nChild的最后落子点为node的待扩展点的坐标
        Node *nChild
            = new Node(node,
                       pos.first,
                       pos.second,
                       choice); //pos.first = pos.x; pos.second = pos.y 若node为根节点，则(pos.first, pos.second) = 'B'
        //if(pos.first == 7 && pos.second == 7) printf("the first node's depth is %d\n", nChild->depth);
        //在节点node的子节点中加入节点n
        node->children.insert(nChild);
        //将节点n放在openTable的前端
        openTable.push_front(nChild);
    }
    return temp.size();
}

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

Node *GameTree::findNodeByPosition(uint8_t x, uint8_t y)
{
    for (Node *n : nodeRoot->children) {
        if (n->fX == x && n->fY == y) { return n; }
    }
    return nullptr;
}

//寻找下一步的最佳落棋点，即寻找根节点的最佳子节点
//改对称的必要，需要改节点--------------------------------------------------------------------------------
//注意前方石山代码-----------------------------------------
void GameTree::setNextPos(int five, int N) //five = 0表示5手交换 非0的整数表示不五手N打 n表示五手n打
{
    vector<pair<uint8_t, uint8_t>> candidatePoints;
    candidatePoints.clear();

    vector<Node *> MaxValueSortNodes; //存储估值依次最高的点，估值点的值是不变的话
    for (auto p : nodeRoot->children)
        MaxValueSortNodes.push_back(p);

    sort(MaxValueSortNodes.begin(), MaxValueSortNodes.end(), [](Node *a, Node *b) { return a->value > b->value; });

    set<vector<pair<uint8_t, uint8_t>>> combined;
    //const int maxRetries = 20; //安全上限？

    auto vp = std::make_unique<fiveNAction>(); //访问指针
    if (N == 2) {
        int firstCount = 0, secondCount = 1;
        int firstIndex = min(firstCount, (int) MaxValueSortNodes.size());
        int secondIndex = min(secondCount, (int) MaxValueSortNodes.size());
        do {
            candidatePoints.emplace_back(MaxValueSortNodes[firstIndex]->fX, MaxValueSortNodes[firstIndex]->fY);
            candidatePoints.emplace_back(MaxValueSortNodes[secondIndex]->fX, MaxValueSortNodes[secondIndex]->fY);

            if (combined.find(candidatePoints) != combined.end()) {
                candidatePoints.clear();
                secondIndex++;
                continue;
            }
            combined.insert(candidatePoints);

            //n==2时，第二个打点开始依次遍历（贪心算法），直到第二个打点遍历到估值集合的末尾任然是对称的，那么第一个打点的位置加一，第二个打点跟在后面再遍历
            if (secondIndex == MaxValueSortNodes.size()) {
                firstIndex++;
                secondIndex = firstIndex + 1;
            }
            //如果这两个打点到了最后一种组合还是对称，那么就直接返回最大可能的那一种组合
            if (firstIndex == MaxValueSortNodes.size() - 1) {
                candidatePoints.emplace_back(MaxValueSortNodes[0]->fX, MaxValueSortNodes[0]->fY);
                candidatePoints.emplace_back(MaxValueSortNodes[1]->fX, MaxValueSortNodes[1]->fY);
                break;
            }
        } while (vp->checkSymmetry(candidatePoints));
    } else if (N == 3) {
        int firstCount = 0, secondCount = 1, thirdCount = 2;
        int firstIndex = min(firstCount, (int) MaxValueSortNodes.size());
        int secondIndex = min(secondCount, (int) MaxValueSortNodes.size());
        int thirdIndex = min(thirdCount, (int) MaxValueSortNodes.size());
        do {
            candidatePoints.emplace_back(MaxValueSortNodes[firstIndex]->fX, MaxValueSortNodes[firstIndex]->fY);
            candidatePoints.emplace_back(MaxValueSortNodes[secondIndex]->fX, MaxValueSortNodes[secondIndex]->fY);
            candidatePoints.emplace_back(MaxValueSortNodes[thirdIndex]->fX, MaxValueSortNodes[thirdIndex]->fY);

            if (combined.find(candidatePoints) != combined.end()) {
                candidatePoints.clear();
                thirdIndex++;
                continue;
            }
            combined.insert(candidatePoints);

            if (thirdIndex == MaxValueSortNodes.size()) {
                secondIndex++;
                thirdIndex = secondIndex + 1;
            }
            if (secondIndex == MaxValueSortNodes.size() - 1) {
                firstIndex++;
                secondIndex = firstIndex + 1;
                thirdIndex = secondCount + 1;
            }
            if (firstIndex == MaxValueSortNodes.size() - 2) {
                candidatePoints.emplace_back(MaxValueSortNodes[0]->fX, MaxValueSortNodes[0]->fY);
                candidatePoints.emplace_back(MaxValueSortNodes[1]->fX, MaxValueSortNodes[1]->fY);
                candidatePoints.emplace_back(MaxValueSortNodes[2]->fX, MaxValueSortNodes[2]->fY);
                break;
            }
        } while (vp->checkSymmetry(candidatePoints));
    } else if (N == 4) {
        int firstCount = 0, secondCount = 1, thirdCount = 2, fourthCount = 3;
        int firstIndex = min(firstCount, (int) MaxValueSortNodes.size());
        int secondIndex = min(secondCount, (int) MaxValueSortNodes.size());
        int thirdIndex = min(thirdCount, (int) MaxValueSortNodes.size());
        int fourthIndex = min(fourthCount, (int) MaxValueSortNodes.size());
        do {
            candidatePoints.emplace_back(MaxValueSortNodes[firstIndex]->fX, MaxValueSortNodes[firstIndex]->fY);
            candidatePoints.emplace_back(MaxValueSortNodes[secondIndex]->fX, MaxValueSortNodes[secondIndex]->fY);
            candidatePoints.emplace_back(MaxValueSortNodes[thirdIndex]->fX, MaxValueSortNodes[thirdIndex]->fY);
            candidatePoints.emplace_back(MaxValueSortNodes[fourthIndex]->fX, MaxValueSortNodes[fourthIndex]->fY);

            if (combined.find(candidatePoints) != combined.end()) {
                candidatePoints.clear();
                fourthIndex++;
                continue;
            }
            combined.insert(candidatePoints);

            if (fourthIndex == MaxValueSortNodes.size()) {
                thirdIndex++;
                fourthIndex = thirdIndex + 1;
            }
            if (thirdIndex == MaxValueSortNodes.size() - 1) {
                secondIndex++;
                thirdIndex = secondIndex + 1;
                fourthIndex = thirdIndex + 1;
            }
            if (secondIndex == MaxValueSortNodes.size() - 2) {
                firstIndex++;
                secondIndex = firstIndex + 1;
                thirdIndex = secondCount + 1;
                fourthIndex = thirdIndex + 1;
            }
            if (firstIndex == MaxValueSortNodes.size() - 3) {
                candidatePoints.emplace_back(MaxValueSortNodes[0]->fX, MaxValueSortNodes[0]->fY);
                candidatePoints.emplace_back(MaxValueSortNodes[1]->fX, MaxValueSortNodes[1]->fY);
                candidatePoints.emplace_back(MaxValueSortNodes[2]->fX, MaxValueSortNodes[2]->fY);
                candidatePoints.emplace_back(MaxValueSortNodes[3]->fX, MaxValueSortNodes[3]->fY);
                break;
            }

        } while (vp->checkSymmetry(candidatePoints));
    } else if (N == 5) {
        int firstCount = 0, secondCount = 1, thirdCount = 2, fourthCount = 3, fifthCount = 4;
        int firstIndex = min(firstCount, (int) MaxValueSortNodes.size());
        int secondIndex = min(secondCount, (int) MaxValueSortNodes.size());
        int thirdIndex = min(thirdCount, (int) MaxValueSortNodes.size());
        int fourthIndex = min(fourthCount, (int) MaxValueSortNodes.size());
        int fifthIndex = min(fifthCount, (int) MaxValueSortNodes.size());

        do {
            candidatePoints.emplace_back(MaxValueSortNodes[firstIndex]->fX, MaxValueSortNodes[firstIndex]->fY);
            candidatePoints.emplace_back(MaxValueSortNodes[secondIndex]->fX, MaxValueSortNodes[secondIndex]->fY);
            candidatePoints.emplace_back(MaxValueSortNodes[thirdIndex]->fX, MaxValueSortNodes[thirdIndex]->fY);
            candidatePoints.emplace_back(MaxValueSortNodes[fourthIndex]->fX, MaxValueSortNodes[fourthIndex]->fY);
            candidatePoints.emplace_back(MaxValueSortNodes[fifthIndex]->fX, MaxValueSortNodes[fifthIndex]->fY);

            if (combined.find(candidatePoints) != combined.end()) {
                candidatePoints.clear();
                fifthIndex++;
                continue;
            }
            combined.insert(candidatePoints);

            if (fifthIndex == MaxValueSortNodes.size()) {
                fourthIndex++;
                fifthIndex = fourthIndex + 1;
            }
            if (fourthIndex == MaxValueSortNodes.size() - 1) {
                thirdIndex++;
                fourthIndex = thirdIndex + 1;
                fifthIndex = fourthIndex + 1;
            }
            if (thirdIndex == MaxValueSortNodes.size() - 2) {
                secondIndex++;
                thirdIndex = secondIndex + 1;
                fourthIndex = thirdIndex + 1;
                fifthIndex = fourthIndex + 1;
            }
            if (secondIndex == MaxValueSortNodes.size() - 3) {
                firstIndex++;
                secondIndex = firstIndex + 1;
                thirdIndex = secondCount + 1;
                fourthIndex = thirdIndex + 1;
                fifthIndex = fourthIndex + 1;
            }
            if (firstIndex == MaxValueSortNodes.size() - 4) {
                candidatePoints.emplace_back(MaxValueSortNodes[0]->fX, MaxValueSortNodes[0]->fY);
                candidatePoints.emplace_back(MaxValueSortNodes[1]->fX, MaxValueSortNodes[1]->fY);
                candidatePoints.emplace_back(MaxValueSortNodes[2]->fX, MaxValueSortNodes[2]->fY);
                candidatePoints.emplace_back(MaxValueSortNodes[3]->fX, MaxValueSortNodes[3]->fY);
                candidatePoints.emplace_back(MaxValueSortNodes[4]->fX, MaxValueSortNodes[4]->fY);
                break;
            }

        } while (vp->checkSymmetry(candidatePoints));
    }

    nodeBest = MaxValueSortNodes[0]; //不是五手N打的情况

    if (five == 0) {
        switch (N) {
        case 2: {
            nodeBest = findNodeByPosition(candidatePoints[0].first, candidatePoints[0].second);
            nodeSecond = findNodeByPosition(candidatePoints[1].first, candidatePoints[2].second);
            break;
        }
        case 3: {
            nodeBest = findNodeByPosition(candidatePoints[0].first, candidatePoints[0].second);
            nodeSecond = findNodeByPosition(candidatePoints[1].first, candidatePoints[1].second);
            nodeThird = findNodeByPosition(candidatePoints[2].first, candidatePoints[2].second);
            break;
        }
        case 4: {
            nodeBest = findNodeByPosition(candidatePoints[0].first, candidatePoints[0].second);
            nodeSecond = findNodeByPosition(candidatePoints[1].first, candidatePoints[1].second);
            nodeThird = findNodeByPosition(candidatePoints[2].first, candidatePoints[2].second);
            nodeFourth = findNodeByPosition(candidatePoints[3].first, candidatePoints[3].second);
            break;
        }
        case 5: {
            nodeBest = findNodeByPosition(candidatePoints[0].first, candidatePoints[0].second);
            nodeSecond = findNodeByPosition(candidatePoints[1].first, candidatePoints[1].second);
            nodeThird = findNodeByPosition(candidatePoints[2].first, candidatePoints[2].second);
            nodeFourth = findNodeByPosition(candidatePoints[3].first, candidatePoints[3].second);
            nodeFifth = findNodeByPosition(candidatePoints[4].first, candidatePoints[4].second);
            break;
        }
        }
    }
}

uint8_t GameTree::game(char choice, int five, int N)
{
    // 如果已经分出胜负，直接返回结果
    uint8_t result = nodeRoot->boardIdentify();
    if (result == 'B' || result == 'W') return result;

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
    return 0;
}

//得到最佳落棋点的位置值
std::vector<std::pair<uint8_t, uint8_t>> GameTree::getNextPos(int five, int n)
{
    vector<pair<uint8_t, uint8_t>> mask;
    if (five) {
        if (nodeBest == nullptr)
            mask.emplace_back(pair<uint8_t, uint8_t>(255, 255));
        else
            mask.emplace_back(pair<uint8_t, uint8_t>(nodeBest->fX, nodeBest->fY));
    } else {
        //黑子第五步实现5手n打，five=0
        switch (n) {
        case 2: {
            setNextPos(five, n);
            mask.emplace_back(pair<uint8_t, uint8_t>(nodeBest->fX, nodeBest->fY));
            mask.emplace_back(pair<uint8_t, uint8_t>(nodeSecond->fX, nodeSecond->fY));
            break;
        }
        case 3: {
            setNextPos(five, n);
            mask.emplace_back(pair<uint8_t, uint8_t>(nodeBest->fX, nodeBest->fY));
            mask.emplace_back(pair<uint8_t, uint8_t>(nodeSecond->fX, nodeSecond->fY));
            mask.emplace_back(pair<uint8_t, uint8_t>(nodeThird->fX, nodeThird->fY));
            break;
        }
        case 4: {
            setNextPos(five, n);
            mask.emplace_back(pair<uint8_t, uint8_t>(nodeBest->fX, nodeBest->fY));
            mask.emplace_back(pair<uint8_t, uint8_t>(nodeSecond->fX, nodeSecond->fY));
            mask.emplace_back(pair<uint8_t, uint8_t>(nodeThird->fX, nodeThird->fY));
            mask.emplace_back(pair<uint8_t, uint8_t>(nodeFourth->fX, nodeFourth->fY));
            break;
        }
        case 5: {
            setNextPos(five, n);
            mask.emplace_back(pair<uint8_t, uint8_t>(nodeBest->fX, nodeBest->fY));
            mask.emplace_back(pair<uint8_t, uint8_t>(nodeSecond->fX, nodeSecond->fY));
            mask.emplace_back(pair<uint8_t, uint8_t>(nodeThird->fX, nodeThird->fY));
            mask.emplace_back(pair<uint8_t, uint8_t>(nodeFourth->fX, nodeFourth->fY));
            mask.emplace_back(pair<uint8_t, uint8_t>(nodeFifth->fX, nodeFifth->fY));
            break;
        }
        }
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
