//记录一个节点（棋局）的所有信息，包括深度、估值得分、落子位置、棋局信息、父节点信息、子节点信息，并提供了判断当前节点是否为MAX节点的函数、当前棋局的估值函数、判断胜负的函数
#pragma once
#include <string>
#include <set>
#include <cstdint>

class Node
{
public:
    Node();
    Node(Node *node, uint8_t x, uint8_t y, char choice);
    int32_t value; //当前节点是叶节点，记录估值得分，当前是MAX节点，记录a值，当前是min节点，记录b值
    //记录当前节点的深度，根节点深度为0
    uint32_t depth;
    //记录当前棋局最后一步落子点的x坐标
    uint8_t fX;
    //记录当前棋局最后一步落子点的y坐标
    uint8_t fY;
    //记录当前棋局，‘B’（66）表示黑子，‘W’(87)表示白子，0（48）表示空位
    uint8_t board[15][15]{};
    Node *father;
    std::set<Node *> children; //set为集合容器 自动排序 且不重复

    bool isMaxNode();           //判断当前节点是否为MAX节点
    void evaluate(char choice); //评估函数
    uint8_t boardIdentify();    //判断当前棋局是否已经分出胜负，若黑方获胜返回66,白方获胜返回87，未分出胜负返回0

    //计算先手五元组s中黑棋得分
    static int32_t evaluateBlackP(std::string &s);
    //计算先手五元组s中白棋得分
    static int32_t evaluateWhiteP(std::string &s);
    //计算后手五元组s中黑棋得分
    static int32_t evaluateBlackF(std::string &s);
    //计算后手五元组s中白棋得分
    static int32_t evaluateWhiteF(std::string &s);

    static std::string convert(uint8_t pos); //将位置字符转换为字符串
};
