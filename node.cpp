#include "node.h"

#include <err.h>
#include <string.h>
#include <cstring>
//memset
#include <iostream>
using std::cerr;
using std::cout;

Node::Node() //创建根节点
{
    father = nullptr;
    children.clear();
    value = INT32_MIN;
    //初始深度为0；
    depth = 0; //偶数为Max节点
    fX = fY = 0;
    //为新申请的内存做初始化 初始化棋盘(空)
    memset(board, 0, sizeof(board));
}

//根据当前节点生成新的子节点 (x,y)表示当前棋局的最后落子点
Node::Node(Node *node, uint8_t x, uint8_t y, char choice)
{
    visitCount = 0;
    totalReward = 0.0;
    //node为父节点
    depth = node->depth + 1; //更新深度
    value = isMaxNode() ? INT32_MIN : INT32_MAX;
    father = node;
    children.clear();
    fX = x;
    fY = y;
    //从源内存地址拷贝若干个字节到目标内存地址中
    memcpy(board, node->board, sizeof(board));
    //深度为0为根节点 深度为奇数为黑子，深度为偶数为白子
    if (choice == 0) //先手
    {
        //当前的depth=0是白子的棋局 depth & 1u 表示深度的二进制数的 最低位（即101 中的最后一个1）若为1，则depth = 1为
        board[fX][fY] = (depth & 1u) ? 'B' : 'W'; //先手
    } else {
        board[fX][fY] = (depth & 1u) ? 'W' : 'B'; //后手
    }
}

//深度为偶数为Max节点
bool Node::isMaxNode()
{
    return (depth & 1u) ^ 1u; //^相同为0 不相同为1；深度为偶数为Max节点
}

//计算先手该五元组s中的黑方得分
int32_t Node::evaluateBlackP(std::string &s)
{
    std::string patterns[31] = {
        "B0000", "0B000", "00B00", "000B0", "0000B", "BB000", "0BB00", "00BB0", "000BB", "B0B00", "0B0B0",
        "00B0B", "B00B0", "0B00B", "B000B", "BBB00", "0BBB0", "00BBB", "BB0B0", "0BB0B", "B0BB0", "0B0BB",
        "BB00B", "B00BB", "B0B0B", "BBBB0", "BBB0B", "BB0BB", "B0BBB", "0BBBB", "BBBBB",
    };
    int32_t scores[31] = {
        1,   1,   1,   1,   1,   10,  10,  10,  10,  10,    10,    10,    10,    10,    10,      100,
        100, 100, 100, 100, 100, 100, 100, 100, 100, 10000, 10000, 10000, 10000, 10000, 1000000,
    };
    for (uint8_t i = 0; i < 31; i++)
        if (s == patterns[i]) return scores[i];
    return 0; //未找到对应的patterns就返回0 退出
}
//计算先手该五元组s中的白方得分
int32_t Node::evaluateWhiteP(std::string &s)
{
    std::string patterns[31] = {
        "W0000", "0W000", "00W00", "000W0", "0000W", "WW000", "0WW00", "00WW0", "000WW", "W0W00", "0W0W0",
        "00W0W", "W00W0", "0W00W", "W000W", "WWW00", "0WWW0", "00WWW", "WW0W0", "0WW0W", "W0WW0", "0W0WW",
        "WW00W", "W00WW", "W0W0W", "WWWW0", "WWW0W", "WW0WW", "W0WWW", "0WWWW", "WWWWW",
    };
    int32_t scores[31] = {
        1,    1,    1,    1,    1,    10,   10,   10,   10,   10,     10,     10,     10,     10,     10,       1000,
        2000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 100000, 100000, 100000, 100000, 100000, 10000000,
    };
    for (uint8_t i = 0; i < 31; i++)
        if (s == patterns[i]) return scores[i];
    return 0;
}
//计算后手该五元组s中的黑方得分
int32_t Node::evaluateBlackF(std::string &s)
{
    std::string patterns[31] = {
        "B0000", "0B000", "00B00", "000B0", "0000B", "BB000", "0BB00", "00BB0", "000BB", "B0B00", "0B0B0",
        "00B0B", "B00B0", "0B00B", "B000B", "BBB00", "0BBB0", "00BBB", "BB0B0", "0BB0B", "B0BB0", "0B0BB",
        "BB00B", "B00BB", "B0B0B", "BBBB0", "BBB0B", "BB0BB", "B0BBB", "0BBBB", "BBBBB",
    };
    int32_t scores[31] = {
        1,    1,    1,    1,    1,    10,   10,   10,   10,   10,     10,     10,     10,     10,     10,       1000,
        2000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 100000, 100000, 100000, 100000, 100000, 10000000,
    };
    for (uint8_t i = 0; i < 31; i++)
        if (s == patterns[i]) return scores[i];
    return 0;
}
//计算后手该五元组s中的白方得分
int32_t Node::evaluateWhiteF(std::string &s)
{
    std::string patterns[31] = {
        "W0000", "0W000", "00W00", "000W0", "0000W", "WW000", "0WW00", "00WW0", "000WW", "W0W00", "0W0W0",
        "00W0W", "W00W0", "0W00W", "W000W", "WWW00", "0WWW0", "00WWW", "WW0W0", "0WW0W", "W0WW0", "0W0WW",
        "WW00W", "W00WW", "W0W0W", "WWWW0", "WWW0W", "WW0WW", "W0WWW", "0WWWW", "WWWWW",
    };
    int32_t scores[31] = {
        1,   1,   1,   1,   1,   10,  10,  10,  10,  10,    10,    10,    10,    10,    10,      100,
        100, 100, 100, 100, 100, 100, 100, 100, 100, 10000, 10000, 10000, 10000, 10000, 1000000,
    };
    for (uint8_t i = 0; i < 31; i++)
        if (s == patterns[i]) return scores[i];
    return 0;
}

//将位置字符转换为字符串
std::string Node::convert(uint8_t pos)
{
    if (pos == 0)
        return "0";
    else if (pos == 'B')
        return "B";
    else
        return "W";
}

//判断当前棋局是否已经分出胜负，黑方获胜返回‘B’,白方获胜返回‘W’,未评判出获胜方返回0
uint8_t Node::boardIdentify()
{
    for (uint8_t i = 0; i < 15; i++) {
        for (uint8_t j = 0; j < 15; j++) {
            //纵向五元组
            if (j + 4 < 15) {
                std::string s;
                for (uint8_t k = 0; k < 5; k++)
                    s += convert(board[i][j + k]);
                if (s == "BBBBB") return 'B';
                if (s == "WWWWW") return 'W';
            }
            //横向五元组
            if (i + 4 < 15) {
                std::string s;
                for (uint8_t k = 0; k < 5; k++)
                    s += convert(board[i + k][j]);
                if (s == "BBBBB") return 'B';
                if (s == "WWWWW") return 'W';
            }
            //副对角线五元组
            if (i + 4 < 15 && j + 4 < 15) {
                std::string s;
                for (uint8_t k = 0; k < 5; k++)
                    s += convert(board[i + k][j + k]);
                if (s == "BBBBB") return 'B';
                if (s == "WWWWW") return 'W';
            }
            //主对角线五元组
            if (i + 4 < 15 && j - 4 >= 0) {
                std::string s;
                for (uint8_t k = 0; k < 5; k++)
                    s += convert(board[i + k][j - k]);
                if (s == "BBBBB") return 'B';
                if (s == "WWWWW") return 'W';
            }
        }
    }
    return 0;
}

//估值函数
void Node::evaluate(char choice) {
    // 使用局部变量避免频繁访问成员
    int32_t score = 0;

    // 方向数组：水平、垂直、主对角线、副对角线
    const int dx[4] = {0, 1, 1, 1};
    const int dy[4] = {1, 0, 1, -1};

    for (uint8_t i = 0; i < 15; ++i) {
        for (uint8_t j = 0; j < 15; ++j) {
            // 跳过空点
            if (board[i][j] == 0) continue;

            for (int d = 0; d < 4; ++d) {
                // 检查是否有效的五元组起点
                int ni = i + dx[d] * 4;
                int nj = j + dy[d] * 4;
                if (ni < 0 || ni >= 15 || nj < 0 || nj >= 15) continue;

                // 构建五元组字符串
                std::string pattern;
                for (int k = 0; k < 5; ++k) {
                    int x = i + dx[d] * k;
                    int y = j + dy[d] * k;
                    pattern += convert(board[x][y]);
                }

                // 评估五元组
                if (choice == 0) {
                    score += evaluateBlackP(pattern) - evaluateWhiteP(pattern);
                } else {
                    score += evaluateWhiteF(pattern) - evaluateBlackF(pattern);
                }
            }
        }
    }
    value = score;
}
