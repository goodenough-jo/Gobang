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
    bool isCenterSymmetric = true;
    bool isVerticalSymmetric = true;
    bool isHorizontalSymmetric = true;
    bool isDiagonalSymmetric45 = true;
    bool isDiagonalSymmetric135 = true;

    std::set<std::pair<int, int>> posSet(positions.begin(), positions.end());

    for (const auto& p : positions) {
        int x = p.first;
        int y = p.second;

        int cx = symmetricPoint.first;
        int cy = symmetricPoint.second;

        // 中心对称
        int x_center = 2 * cx - x;
        int y_center = 2 * cy - y;
        if (posSet.find({x_center, y_center}) == posSet.end()) isCenterSymmetric = false;

        // 垂直轴对称（x轴翻转）
        int x_vertical = 2 * cx - x;
        int y_vertical = y;
        if (posSet.find({x_vertical, y_vertical}) == posSet.end()) isVerticalSymmetric = false;

        // 水平轴对称（y轴翻转）
        int x_horizontal = x;
        int y_horizontal = 2 * cy - y;
        if (posSet.find({x_horizontal, y_horizontal}) == posSet.end()) isHorizontalSymmetric = false;

        // 主对角线 ↘ 判断: y - x = y' - x'，排除自身
        int value45 = y - x;
        bool found45 = false;
        for (const auto& q : posSet) {
            if (q == p) continue; // 排除自身
            if ((q.second - q.first) == value45) {
                found45 = true;
                break;
            }
        }
        if (!found45) isDiagonalSymmetric45 = false;

        // 副对角线 ↙ 判断: y + x = x' + y'，排除自身
        int value135 = y + x;
        bool found135 = false;
        for (const auto& q : posSet) {
            if (q == p) continue; // 排除自身
            if ((q.first + q.second) == value135) {
                found135 = true;
                break;
            }
        }
        if (!found135) isDiagonalSymmetric135 = false;
    }

    if (isCenterSymmetric || isVerticalSymmetric || isHorizontalSymmetric || isDiagonalSymmetric45
        || isDiagonalSymmetric135) {
        std::cout << "该局面具有如下对称性：\n";
        if (isCenterSymmetric) std::cout << "- 中心对称\n";
        if (isVerticalSymmetric) std::cout << "- 水平轴对称\n";
        if (isHorizontalSymmetric) std::cout << "- 垂直轴对称\n";
        if (isDiagonalSymmetric45) std::cout << "- 主对角线（↘）对称 (y - x = 常数)\n";
        if (isDiagonalSymmetric135) std::cout << "- 副对角线（↙）对称 (y + x = 常数)\n";
        return true;
    }

    std::cout << "不对称\n";
    return false;
    /*
    bool isCenterSymmetric = true;
    bool isVerticalSymmetric = true;
    bool isHorizontalSymmetric = true;
    bool isDiagonalSymmetric45 = true;
    bool isDiagonalSymmetric135 = true;

    // 逐个检查每个棋子是否有对应的对称棋子
    std::set<std::pair<int, int>> posSet;
    for (const auto& p : positions) {
        posSet.insert(p);
    }

    for (const auto& p : positions) {
        // 计算中心对称
        int symX_center = 2 * symmetricPoint.first - p.first;   // 对称位置
        int symY_center = 2 * symmetricPoint.second - p.second; // 对称位置

        if (posSet.find({symX_center, symY_center}) == posSet.end()) { isCenterSymmetric = false; }

        //计算垂直轴对称，由于是node的xy与棋盘是相反的，所以垂直即是棋盘的水平
        int symX_vertical = 2 * symmetricPoint.first - p.first;
        int symY_vertical = p.second;
        if (posSet.find({symX_vertical, symY_vertical}) == posSet.end()) { isVerticalSymmetric = false; }

        //计算水平轴对称，同理
        int symX_horizontal = p.first;
        int symY_horizontal = 2 * symmetricPoint.second - p.second;
        if (posSet.find({symX_horizontal, symY_horizontal}) == posSet.end()) { isHorizontalSymmetric = false; }


        
    }
    if (isCenterSymmetric || isVerticalSymmetric || isHorizontalSymmetric) {
        std::cout << "该局面具有如下对称性：\n";
        if (isCenterSymmetric) std::cout << "- 中心对称\n";
        if (isVerticalSymmetric) std::cout << "- 水平轴对称\n";
        if (isHorizontalSymmetric) std::cout << "- 垂直轴对称\n";
        return true;
    }

    std::cout << "不对称\n";
    return false;
    */
}

void fiveNAction::getSymmetricPoint(const std::vector<std::pair<int, int>>& positions)
{
    // 我们根据棋子的分布计算一个对称中心
    float centerX = 0, centerY = 0;
    int count = positions.size();

    // 计算所有棋子的平均位置（重心）
    for (const auto& p : positions) {
        centerX += p.first;
        centerY += p.second;
    }

    // 计算重心的坐标，作为对称中心
    centerX /= count;
    centerY /= count;
    symmetricPoint = {centerX, centerY};
    // std::cout << "symmetricPoint:" << symmetricPoint.first << ", " << symmetricPoint.second << "\n";
}
