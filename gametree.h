//记录一颗博弈树的所有信息，包括搜索半径，最大深度，根节点指针、最佳节点指针，open表、closed表，并提供了博弈控制函数、
//节点扩展函数、α-β更新函数、α-β剪枝判断函数等。此外，GameTree类还提供了cmd命令行窗口的可视化功能。
#pragma once
#include <deque>
#include <vector>
#include "node.h"
#include <cstdint>
//uint8_t等一组固定宽度的整数类型

class GameTree
{
public:
    uint32_t maxDepth = 4;          //最大深度  1 2 3 4
    uint32_t radius = 2;            //搜索半径  从根节点开始往下搜索两层（min、max）
    Node *nodeRoot = new Node{};    //指向根节点 0
    Node *nodeBest = nullptr;       //指向最佳子节点 1
    Node *nodeSecond = nullptr;     //指向次子节点 1
    Node *nodeThird = nullptr;      //指向第三子节点 2
    Node *nodeFourth = nullptr;     //指向第四子节点 3
    Node *nodeFifth = nullptr;      //指向第五子节点 4
    std::deque<Node *> openTable;   //一个双向队列，存放待扩展节点的指针
    std::deque<Node *> closedTable; //一个双向队列，存放已扩展节点的指针
private:
    //返回当前棋局的待扩展点坐标集合，返回一个vector容器，容器中的元素都是一个点的坐标
    //std::pair主要的作用是将两个数据组合成一个数据，两个数据可以是同一类型或者不同类型
    std::vector<std::pair<uint8_t, uint8_t>> getSearchNodes(Node *node, char choice);
    //扩展node节点 生成node节点的所有子节点(创建棋局)
    uint8_t expandChildrenNodes(Node *node, char choice);
    //判断节点node是否能ab剪枝
    static bool isAlphaBetaCut(Node *node);
    //在某个叶节点完成估价以后，该函数负责更新其所有父节点的ab值
    static void updateValueFromNode(Node *node);
    //寻找下一步的最佳落棋点，即寻找根节点的最佳子节点
    void setNextPos(int five, int n); //得到最佳子节点

    //根据pair<uint8_t, uint8_t>,找到Node*指针，在setNextPos中使用
    Node *findNodeByPosition(uint8_t x, uint8_t y);

public:
    //构建函数 default表示默认
    GameTree() = default;
    GameTree(uint32_t maxDepth, uint8_t radius);
    GameTree(uint32_t maxDepth, uint8_t radius, uint8_t (&board)[15][15]);
    //控制博弈搜索过程
    uint8_t game(char choice, int five, int N);
    //得到最佳落棋点的位置值
    std::vector<std::pair<uint8_t, uint8_t>> getNextPos(int five, int N);
    //提供可视化窗口
    void showNextPos(int five, int N); //在cmd窗口中打印下一步落子坐标
    uint8_t showBoard(int five);       //打印当前棋局
};
