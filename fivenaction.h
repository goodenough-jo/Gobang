//用于5手N打时的禁手规则,2<=N<=5
//在5手N打时，落子可以禁手，不可对称，黑方最终选择的棋局不可出现三三禁手（四四？
//大致功能是2：避免对称+敌我棋局检查，留下的黑子不能有禁手
//对称？图形对称？

#pragma once
#include <cstdint>
#include <vector>

class fiveNAction
{
public:
    fiveNAction() = default;
    bool checkSymmetry(const std::vector<std::pair<uint8_t, uint8_t>> &mask); //检查对称
    //bool checkForbid();                                                       //检查留下的黑子是否有禁手，对于白方而言
};
