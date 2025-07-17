#include "fivenaction.h"
#include <set>
#include <iostream>

//图形相似对称？
bool fiveNAction::checkSymmetry(const std::vector<std::pair<uint8_t, uint8_t>> &mask)
{
    int flag = 0;
    //‘B’（66）表示黑子，‘W’(87)表示白子，0（48）表示空位
    //对于开局起始位置，实际上覆盖范围应该没有这么大，可以考虑缩小范围，时间复杂度O(N*N)
    const int size = mask.size();

    //两两打点位相比，对称就flag++并继续下一对
    for (int i = 0; i < size - 1; ++i) {
        for (int j = 1; j < size; ++j) {
            //关于天元的中心对称
            if (mask[i].first == mask[j].second && mask[i].second == mask[j].first) flag++;
            //水平对称
            else if (mask[i].first == 14 - mask[j].first && mask[i].second == mask[j].second)
                flag++;
            //垂直对称
            else if (mask[i].first == mask[j].first && mask[i].second == 14 - mask[j].second)
                flag++;
        }
    }
    return flag == size * (size - 1) / 2; //如果相等，那么对称，返回true
}

//检查棋盘是否中心对称
bool fiveNAction::isSymmetric(const std::vector<std::pair<int, int>>& positions)
{
    std::set<std::pair<int, int>> posSet;
    for (const auto& p : positions) {
        posSet.insert(p);
    }

    for (const auto& p : positions) {
        int symX = 14 - p.first;
        int symY = 14 - p.second;
        if (posSet.find({symX, symY}) == posSet.end()) {
            std::cout << "不对称\n";
            return false;
        }
    }
    std::cout << "对称\n";
    return true;
}
