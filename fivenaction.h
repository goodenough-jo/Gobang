//用于5手N打时的禁手规则,2<=N<=5
//在5手N打时，落子可以禁手，不可对称，黑方最终选择的棋局不可出现三三禁手（四四？
//大致功能是2：避免对称+敌我棋局检查，留下的黑子不能有禁手
//对称？图形对称？

/*
 * 五手N打前，先判断棋盘是否处于对称，若处于对称则需要考虑打点对称的问题。
 */
#pragma once
#include <cstdint>
#include <vector>

class fiveNAction
{
public:
    fiveNAction() = default;
    std::pair<float, float> symmetricPoint; //存放五手打N前的对称点，即第四手的对称点

    bool checkSymmetry(const std::vector<std::pair<uint8_t, uint8_t>> &mask); //检查对称
    //bool checkForbid();
    bool isSymmetric(const std::vector<std::pair<int, int>> &positions); //判断棋盘是否处于对称

    void getSymmetricPoint(const std::vector<std::pair<int, int>> &positions);
};
