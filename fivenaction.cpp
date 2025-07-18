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
    // 创建一个集合来存储所有已下棋子的坐标
    std::set<std::pair<int, int>> posSet;
    for (const auto& p : positions) {
        std::cout << "\n" << p.first << " " << p.second << "\n";
        posSet.insert(p);
    }

    // 假设我们根据棋子的分布计算一个对称中心
    /*这里有一个疑问：如果计算的点小数点不是0.5怎么办？
     * 
     */
    float centerX = 0, centerY = 0;
    int count = positions.size();

    // 计算所有棋子的平均位置（重心）
    for (const auto& p : positions) {
        centerX += p.first;
        centerY += p.second;
    }

    std::cout << "计算前centerX与centerY：" << centerX << " " << centerY << "\n";

    std::cout << "count:" << count << "\n";

    // 计算重心的坐标，作为对称中心
    centerX /= count;
    centerY /= count;

    std::cout << "对称点位置：" << centerX << " " << centerY << "\n";

    // 逐个检查每个棋子是否有对应的对称棋子
    for (const auto& p : positions) {
        // 计算与当前棋子对称的位置
        int symX = 2 * centerX - p.first;  // 对称位置
        int symY = 2 * centerY - p.second; // 对称位置

        // 如果对称点在集合中，说明这两个点形成了对称对
        if (posSet.find({symX, symY}) == posSet.end()) {
            // 如果没有找到对应的对称点，返回 false
            std::cout << "不对称\n";
            return false;
        }
    }
    std::cout << "对称\n";
    return true; // 如果所有点都有对应的对称点，返回 true
}
