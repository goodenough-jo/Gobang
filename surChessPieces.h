//五子棋禁手判定算法
/*
 * 说明：
 * 八方向中心棋：
 * 0----向上
 * 1----左下到右上
 * 2----右
 * 3----左上到右下
 * 4----向下
 * 5----右上到左下
 * 6----左
 * 7----右下到左上
 *
 * 四方向：
 * 0---竖直 1----左下到右上 2---水平方向 3---左上到右下方向
 * 棋子的符号表示法
 * 0---黑子 +表示空位 ？白子或边界
 */
#include <string.h>
#include <cstdint>
#define NO_FORBIDDEN 0          //无禁手
#define THREE_THREE_FORBIDDEN 1 //三三禁手
#define FOUR_FOUR_FORBIDDEN 2   //四四禁手
#define LONG_FORBIDDEN 3        //长连禁手

//#define NONE 0 //无子 0
//#define BLACK 1 //黑子 'B'
//#define WHITE 2 //白子 'W'
using namespace std;

//黑棋棋子数
struct SurChessPieces
{
    int adjsame[8];      //记录与(x, y)相邻连续黑色棋子数 8个方向
    int adjempty[8];     //记录adjsame后相邻连续空子数
    int jumpsame[8];     //记录adjempty后的连续黑子数
    int jumpempty[8];    //记录jumpsame后的空位数
    int jumpjumpsame[8]; //记录jumpempty后的连续黑子数

    SurChessPieces()
    {
        memset(this, 0, sizeof(SurChessPieces)); //初始化
    }
};
/*
 * 统计(x, y)周围相邻棋子情况
 */
SurChessPieces calSurChessPieces(uint8_t chessboard[15][15], int x, int y)
{
    SurChessPieces surChessPieces;

    //进行棋盘盘面的检查 搜索(x, y)周边棋盘
    int _x, _y;
    //向上搜索
    for (_y = y - 1; _y >= 0 && chessboard[x][_y] == 'B'; _y--) {
        surChessPieces.adjsame[0]++; //黑子数
    }
    for (; _y >= 0 && chessboard[x][_y] == 0; _y--) {
        surChessPieces.adjempty[0]++; //空位数
    }
    for (; _y >= 0 && chessboard[x][_y] == 'B'; _y--) {
        surChessPieces.jumpsame[0]++;
    }
    for (; _y >= 0 && chessboard[x][_y] == 0; _y--) {
        surChessPieces.jumpempty[0]++;
    }
    for (; _y >= 0 && chessboard[x][_y] == 'B'; _y--) {
        surChessPieces.jumpjumpsame[0]++;
    }
    //右上搜索
    for (_x = x + 1, _y = y - 1; _x < 15 && _y >= 0 && chessboard[_x][_y] == 'B'; _x++, _y--) {
        surChessPieces.adjsame[1]++;
    }
    for (; _x <= 15 && _y >= 0 && chessboard[_x][_y] == 0; _x++, _y--) {
        surChessPieces.adjempty[1]++;
    }
    for (; _x <= 15 && _y >= 0 && chessboard[_x][_y] == 'B'; _x++, _y--) {
        surChessPieces.jumpsame[1]++;
    }
    for (; _x <= 15 && _y >= 0 && chessboard[_x][_y] == 0; _x++, _y--) {
        surChessPieces.jumpempty[1]++;
    }
    for (; _x <= 15 && _y >= 0 && chessboard[_x][_y] == 'B'; _x++, _y--) {
        surChessPieces.jumpjumpsame[1]++;
    }
    //向右搜索
    for (_x = x + 1; _x < 15 && chessboard[_x][y] == 'B'; _x++) {
        surChessPieces.adjsame[2]++;
    }
    for (; _x < 15 && chessboard[_x][y] == 0; _x++) {
        surChessPieces.adjempty[2]++;
    }
    for (; _x < 15 && chessboard[_x][y] == 'B'; _x++) {
        surChessPieces.jumpsame[2]++;
    }
    for (; _x < 15 && chessboard[_x][y] == 0; _x++) {
        surChessPieces.jumpempty[2]++;
    }
    for (; _x < 15 && chessboard[_x][y] == 'B'; _x++) {
        surChessPieces.jumpjumpsame[2]++;
    }
    //向右下搜索
    for (_x = x + 1, _y = y + 1; _x < 15 && _y < 15 && chessboard[_x][_y] == 'B'; _x++, _y++) {
        surChessPieces.adjsame[3]++;
    }
    for (; _y < 15 && _x < 15 && chessboard[_x][_y] == 0; _x++, _y++) {
        surChessPieces.adjempty[3]++;
    }
    for (; _y < 15 && _x < 15 && chessboard[_x][_y] == 'B'; _x++, _y++) {
        surChessPieces.jumpsame[3]++;
    }
    for (; _y < 15 && _x < 15 && chessboard[_x][_y] == 0; _x++, _y++) {
        surChessPieces.jumpempty[3]++;
    }
    for (; _y < 15 && _x < 15 && chessboard[_x][_y] == 'B'; _x++, _y++) {
        surChessPieces.jumpjumpsame[3]++;
    }
    //向下搜索
    for (_y = y + 1; _y < 15 && chessboard[x][_y] == 'B'; _y++) {
        surChessPieces.adjsame[4]++;
    }
    for (; _y < 15 && chessboard[x][_y] == 0; _y++) {
        surChessPieces.adjempty[4]++;
    }
    for (; _y < 15 && chessboard[x][_y] == 'B'; _y++) {
        surChessPieces.jumpsame[4]++;
    }
    for (; _y < 15 && chessboard[x][_y] == 0; _y++) {
        surChessPieces.jumpempty[4]++;
    }
    for (; _y < 15 && chessboard[x][_y] == 'B'; _y++) {
        surChessPieces.jumpjumpsame[4]++;
    }
    //向左下搜索
    for (_x = x - 1, _y = y + 1; _x >= 0 && _y < 15 && chessboard[_x][_y] == 'B'; _x--, _y++) {
        surChessPieces.adjsame[5]++;
    }
    for (; _x >= 0 && _y < 15 && chessboard[_x][_y] == 0; _x--, _y++) {
        surChessPieces.adjempty[5]++;
    }
    for (; _x >= 0 && _y < 15 && chessboard[_x][_y] == 'B'; _x--, _y++) {
        surChessPieces.jumpsame[5]++;
    }
    for (; _x >= 0 && _y < 15 && chessboard[_x][_y] == 0; _x--, _y++) {
        surChessPieces.jumpempty[5]++;
    }
    for (; _x >= 0 && _y < 15 && chessboard[_x][_y] == 'B'; _x--, _y++) {
        surChessPieces.jumpjumpsame[5]++;
    }
    //向左搜索
    for (_x = x - 1; _x >= 0 && chessboard[_x][y] == 'B'; _x--) {
        surChessPieces.adjsame[6]++;
    }
    for (; _x >= 0 && chessboard[_x][y] == 0; _x--) {
        surChessPieces.adjempty[6]++;
    }
    for (; _x >= 0 && chessboard[_x][y] == 'B'; _x--) {
        surChessPieces.jumpsame[6]++;
    }
    for (; _x >= 0 && chessboard[_x][y] == 0; _x--) {
        surChessPieces.jumpempty[6]++;
    }
    for (; _x >= 0 && chessboard[_x][y] == 'B'; _x--) {
        surChessPieces.jumpjumpsame[6]++;
    }
    //向左上搜索
    for (_x = x - 1, _y = y - 1; _x >= 0 && _y >= 0 && chessboard[_x][_y] == 'B'; _x--, _y--) {
        surChessPieces.adjsame[7]++;
    }
    for (; _x >= 0 && _y >= 0 && chessboard[_x][_y] == 0; _x--, _y--) {
        surChessPieces.adjempty[7]++;
    }
    for (; _x >= 0 && _y >= 0 && chessboard[_x][_y] == 'B'; _x--, _y--) {
        surChessPieces.jumpsame[7]++;
    }
    for (; _x >= 0 && _y >= 0 && chessboard[_x][_y] == 0; _x--, _y--) {
        surChessPieces.jumpempty[7]++;
    }
    for (; _x >= 0 && _y >= 0 && chessboard[_x][_y] == 'B'; _x--, _y--) {
        surChessPieces.jumpjumpsame[7]++;
    }
    return surChessPieces;
}

/*
 * 判断(x , y)周边8个方向是否出现了活三
 * 返回值为true表明出现了 返回值为false表明没出现
 */
bool hasJumpThree(uint8_t chessboard[15][15], int x, int y)
{
    //1.搜索
    SurChessPieces surChessPieces = calSurChessPieces(chessboard, x, y);
    //2.检查
    int threecount = 0;
    for (int i = 0; i < 4; i++) {
        if (surChessPieces.adjsame[i] + surChessPieces.adjsame[i + 4] == 2) //?000?
        {
            if (surChessPieces.adjempty[i] > 0 && surChessPieces.adjempty[i + 4] > 0) //构成活三
            {
                if (surChessPieces.jumpsame[i] > 0 || surChessPieces.jumpsame[i + 4] > 0) { return false; }
                threecount++;
                if (threecount > 1) return true;
            }
        } else if (surChessPieces.adjsame[i] + surChessPieces.adjsame[i + 4] == 1) {
            //?+0+00+?
            if (surChessPieces.adjempty[i] == 1 && surChessPieces.jumpsame[i] == 1
                && (surChessPieces.jumpempty[i] > 1
                    || surChessPieces.jumpempty[i] == 1 && surChessPieces.jumpjumpsame[i] == 0)
                && (surChessPieces.adjempty[i + 4] > 1
                    || surChessPieces.adjempty[i + 4] == 1 && surChessPieces.jumpsame[i + 4] == 0)) {
                threecount++;
                if (threecount > 1) return true;
            }
            //?+00+0+?
            if (surChessPieces.adjempty[i + 4] == 1 && surChessPieces.jumpsame[i + 4] == 1
                && (surChessPieces.jumpempty[i + 4] > 1
                    || surChessPieces.jumpempty[i + 4] == 1 && surChessPieces.jumpjumpsame[i + 4] == 0)
                && (surChessPieces.adjempty[i] > 1
                    || surChessPieces.adjempty[i] == 1 && surChessPieces.jumpsame[i] == 0)) {
                threecount++;
                if (threecount > 1) return true;
            }
            /*
            //?+0+B0+?
            // if (surChessPieces.adjempty[i] == 1 && surChessPieces.jumpsame[i] == 1
            //     && surChessPieces.jumpempty[i] > 0 && surChessPieces.adjempty[i + 4] > 1) {
            //     threecount++;
            //     if (threecount > 1)
            //         return true;
            // }
            //?+0B+0+?
            // if (surChessPieces.adjempty[i + 4] == 1 && surChessPieces.jumpsame[i + 4] == 1
            //     && surChessPieces.jumpempty[i + 4] > 0 && surChessPieces.adjempty[i] > 1) {
            //     threecount++;
            //     if (threecount > 1)
            //         return true;
            // }
            //?+B0+0+?
            // if (surChessPieces.adjempty[i] > 0 && surChessPieces.adjempty[i + 4] == 1
            //     && surChessPieces.jumpsame[i + 4] == 1 && surChessPieces.jumpempty[i + 4] > 0) {
            //     threecount++;
            //     if (threecount > 1)
            //         return true;
            // }
            //?+0+0B+?
            // if (surChessPieces.adjempty[i + 4] > 0 && surChessPieces.adjempty[i] == 1
            //     && surChessPieces.jumpsame[i] == 1 && surChessPieces.jumpempty[i] > 0) {
            //     threecount++;
            //     if (threecount > 1)
            //         return true;
            // }
            */
        } else if (surChessPieces.adjsame[i] + surChessPieces.adjsame[i + 4] == 0) {
            //+B+00+
            if (surChessPieces.adjempty[i + 4] == 1 && surChessPieces.jumpsame[i + 4] == 2
                && (surChessPieces.jumpempty[i + 4] > 1
                    || surChessPieces.jumpempty[i + 4] == 1 && surChessPieces.jumpjumpsame[i + 4] == 0)
                && (surChessPieces.adjempty[i] > 1
                    || surChessPieces.adjempty[i] == 1 && surChessPieces.jumpsame[i] == 0)) {
                threecount++;
                if (threecount > 1) return true;
            }
            /*
            // if (surChessPieces.adjempty[i] > 0 && surChessPieces.adjempty[i + 4] == 1
            //     && surChessPieces.jumpsame[i + 4] == 2 && surChessPieces.jumpempty[i + 4] > 0) {
            //     threecount++;
            //     if (threecount > 1)
            //         return true;
            // }
            */
            //+00+B+
            if (surChessPieces.adjempty[i] == 1 && surChessPieces.jumpsame[i] == 2
                && (surChessPieces.jumpempty[i] > 1
                    || surChessPieces.jumpempty[i] == 1 && surChessPieces.jumpjumpsame[i] == 0)
                && (surChessPieces.adjempty[i + 4] > 1
                    || surChessPieces.adjempty[i + 4] == 1 && surChessPieces.jumpsame[i + 4] == 0)) {
                threecount++;
                if (threecount > 1) return true;
            }
            /*
            // if (surChessPieces.adjempty[i + 4] > 0 && surChessPieces.adjempty[i] == 1
            //     && surChessPieces.jumpsame[i] == 2 && surChessPieces.jumpempty[i] > 0) {
            //     threecount++;
            //     if (threecount > 1)
            //         return true;
            // }
*/
        }
    }
    if (threecount > 1) return true;
    return false;
}
/*
 * 判断(x , y)周边8个方向是否出现了活四
 */
bool hasJumpFour(uint8_t chessboard[15][15], int x, int y)
{
    //1.搜索
    SurChessPieces surChessPieces = calSurChessPieces(chessboard, x, y);
    //2.检查
    int fourcount = 0;
    for (int i = 0; i < 4; i++) {
        if (surChessPieces.adjsame[i] + surChessPieces.adjsame[i + 4] == 3) //?0000?
        {
            if (surChessPieces.adjempty[i] > 0 && surChessPieces.adjempty[i + 4] > 0) //构成活四
            {
                fourcount++; //形成四子的个数加一 循环遍历各个方向 可能构成双活四
                if (fourcount > 1) return true;
            }
            //构成冲四 |0000+
            else if ((surChessPieces.adjempty[i] == 0 && surChessPieces.jumpempty[i] == 0
                      && surChessPieces.jumpsame[i] == 0 && surChessPieces.jumpjumpsame[i] == 0)
                     && surChessPieces.adjempty[i + 4] > 0) {
                fourcount++;
                if (fourcount > 1) return true;
            }
            //构成冲四 +0000|
            else if ((surChessPieces.adjempty[i + 4] == 0 && surChessPieces.jumpsame[i + 4] == 0
                      && surChessPieces.jumpempty[i + 4] == 0 && surChessPieces.jumpjumpsame[i + 4] == 0)
                     && surChessPieces.adjempty[i] > 0) {
                fourcount++;
                if (fourcount > 1) return true;
            }
        } else if (surChessPieces.adjsame[i] + surChessPieces.adjsame[i + 4] == 2) {
            //1.?+00B+0+?
            if (surChessPieces.adjempty[i] > 0 && surChessPieces.adjempty[i + 4] == 1
                && surChessPieces.jumpsame[i + 4] == 1 && surChessPieces.jumpempty[i + 4] > 0) {
                fourcount++;
                if (fourcount > 1) return true;
            }
            //?+0+B00+?
            else if (surChessPieces.adjempty[i + 4] > 0 && surChessPieces.adjempty[i] == 1
                     && surChessPieces.jumpsame[i] == 1 && surChessPieces.jumpempty[i] > 0) {
                fourcount++;
                if (fourcount > 1) return true;
            }
            //2.+0B0+0+
            else if (surChessPieces.adjempty[i] > 0 && surChessPieces.adjsame[i] == 1
                     && surChessPieces.adjempty[i + 4] == 1 && surChessPieces.jumpsame[i + 4] == 1
                     && surChessPieces.jumpempty[i + 4] > 0) {
                fourcount++;
                if (fourcount > 1) return true;
            }
            //+0+0B0+
            else if (surChessPieces.adjempty[i + 4] > 0 && surChessPieces.adjsame[i + 4] == 1
                     && surChessPieces.adjempty[i] == 1 && surChessPieces.jumpsame[i] == 1
                     && surChessPieces.jumpempty[i] > 0) {
                fourcount++;
                if (fourcount > 1) return true;
            }
            //3.+B00+0+
            else if (surChessPieces.adjempty[i] > 0 && surChessPieces.adjempty[i + 4] == 1
                     && surChessPieces.jumpsame[i + 4] == 1 && surChessPieces.jumpempty[i + 4] > 0) {
                fourcount++;
                if (fourcount > 1) return true;
            }
            //+0+00B+
            else if (surChessPieces.adjempty[i + 4] > 0 && surChessPieces.adjempty[i] == 1
                     && surChessPieces.jumpsame[i] == 1 && surChessPieces.jumpempty[i] > 0) {
                fourcount++;
                if (fourcount > 1) return true;
            }
            //构成冲四
            //?|00B+0+?
            else if ((surChessPieces.adjempty[i] == 0 && surChessPieces.jumpsame[i] == 0
                      && surChessPieces.jumpempty[i] == 0 && surChessPieces.jumpjumpsame[i] == 0)
                     && surChessPieces.adjempty[i + 4] == 1 && surChessPieces.jumpsame[i + 4] == 1
                     && surChessPieces.jumpempty[i + 4] > 0) {
                fourcount++;
                if (fourcount > 1) return true;
            }
            //?+00B+0|?
            else if (surChessPieces.adjempty[i] > 0 && surChessPieces.adjempty[i + 4] == 1
                     && surChessPieces.jumpsame[i + 4] == 1
                     && (surChessPieces.jumpempty[i + 4] == 0 && surChessPieces.jumpjumpsame[i + 4] == 0)) {
                fourcount++;
                if (fourcount > 1) return true;
            }
            //?+0+B00|?
            else if ((surChessPieces.adjempty[i + 4] == 0 && surChessPieces.jumpsame[i + 4] == 0
                      && surChessPieces.jumpempty[i + 4] == 0 && surChessPieces.jumpjumpsame[i + 4] == 0)
                     && surChessPieces.adjempty[i] == 1 && surChessPieces.jumpsame[i] == 1
                     && surChessPieces.jumpempty[i] > 0) {
                fourcount++;
                if (fourcount > 1) return true;
            }
            //?|0+B00+?
            else if (surChessPieces.adjempty[i + 4] > 0
                     && (surChessPieces.adjempty[i] == 1 && surChessPieces.jumpsame[i] == 1
                         && surChessPieces.jumpempty[i] == 0 && surChessPieces.jumpjumpsame[i] == 0)) {
                fourcount++;
                if (fourcount > 1) return true;
            }
            //+0B0+0|
            else if (surChessPieces.adjempty[i] > 0 && surChessPieces.adjsame[i] == 1
                     && surChessPieces.adjempty[i + 4] == 1 && surChessPieces.jumpsame[i + 4] == 1
                     && (surChessPieces.jumpempty[i + 4] == 0 && surChessPieces.jumpjumpsame[i + 4] == 0)) {
                fourcount++;
                if (fourcount > 1) return true;
            }
            //|0B0+0+
            else if ((surChessPieces.adjempty[i] == 0 && surChessPieces.jumpsame[i] == 0
                      && surChessPieces.jumpempty[i] == 0 && surChessPieces.jumpjumpsame[i] == 0)
                     && surChessPieces.adjsame[i] == 1 && surChessPieces.adjempty[i + 4] == 1
                     && surChessPieces.jumpsame[i + 4] == 1 && surChessPieces.jumpempty[i + 4] > 0) {
                fourcount++;
                if (fourcount > 1) return true;
            }

            //+0+0B0|
            else if ((surChessPieces.adjempty[i + 4] == 0 && surChessPieces.jumpsame[i + 4] == 0
                      && surChessPieces.jumpempty[i + 4] == 0 && surChessPieces.jumpjumpsame[i + 4] == 0)
                     && surChessPieces.adjsame[i + 4] == 1 && surChessPieces.adjempty[i] == 1
                     && surChessPieces.jumpsame[i] == 1 && surChessPieces.jumpempty[i] > 0) {
                fourcount++;
                if (fourcount > 1) return true;
            }
            //+|0+0B0+
            else if (surChessPieces.adjempty[i + 4] > 0 && surChessPieces.adjsame[i + 4] == 1
                     && surChessPieces.adjempty[i] == 1 && surChessPieces.jumpsame[i] == 1
                     && (surChessPieces.jumpempty[i] == 0 && surChessPieces.jumpjumpsame[i] == 0)) {
                fourcount++;
                if (fourcount > 1) return true;
            }
            //|B00+0+
            else if ((surChessPieces.adjempty[i] == 0 && surChessPieces.jumpsame[i] == 0
                      && surChessPieces.jumpempty[i] == 0 && surChessPieces.jumpjumpsame[i] == 0)
                     && surChessPieces.adjempty[i + 4] == 1 && surChessPieces.jumpsame[i + 4] == 1
                     && surChessPieces.jumpempty[i + 4] > 0) {
                fourcount++;
                if (fourcount > 1) return true;
            }
            //+B00+0|
            else if (surChessPieces.adjempty[i] > 0 && surChessPieces.adjempty[i + 4] == 1
                     && surChessPieces.jumpsame[i + 4] == 1
                     && (surChessPieces.jumpempty[i + 4] == 0 && surChessPieces.jumpjumpsame[i + 4] == 0)) {
                fourcount++;
                if (fourcount > 1) return true;
            }
            //|0+00B+  p
            else if (surChessPieces.adjempty[i + 4] > 0 && surChessPieces.adjempty[i] == 1
                     && surChessPieces.jumpsame[i] == 1
                     && (surChessPieces.jumpempty[i] == 0 && surChessPieces.jumpjumpsame[i] == 0)) {
                fourcount++;
                if (fourcount > 1) return true;
            }
            //+0+00B|
            else if ((surChessPieces.adjempty[i + 4] == 0 && surChessPieces.jumpsame[i + 4] == 0
                      && surChessPieces.jumpempty[i + 4] == 0 && surChessPieces.jumpjumpsame[i + 4] == 0)
                     && surChessPieces.adjempty[i] == 1 && surChessPieces.jumpsame[i] == 1
                     && surChessPieces.jumpempty[i] > 0) {
                fourcount++;
                if (fourcount > 1) return true;
            }
        } else if (surChessPieces.adjsame[i] + surChessPieces.adjsame[i + 4] == 1) {
            //1.0B+00+
            if (surChessPieces.adjsame[i] == 1 && surChessPieces.adjempty[i] > 0 && surChessPieces.adjempty[i + 4] == 1
                && surChessPieces.jumpsame[i + 4] == 2 && surChessPieces.jumpempty[i + 4] > 0) {
                fourcount++;
                if (fourcount > 1) return true;
            }
            //+00+B0
            else if (surChessPieces.adjsame[i + 4] == 1 && surChessPieces.adjempty[i + 4] > 0
                     && surChessPieces.adjempty[i] == 1 && surChessPieces.jumpsame[i] == 2
                     && surChessPieces.jumpempty[i] > 0) {
                fourcount++;
                if (fourcount > 1) return true;
            }
            //2.+B0+00+
            if (surChessPieces.adjempty[i] > 0 && surChessPieces.adjempty[i + 4] == 1
                && surChessPieces.jumpsame[i + 4] == 2 && surChessPieces.jumpempty[i + 4] > 0) {
                fourcount++;
                if (fourcount > 1) return true;
            }
            //+00+0B+
            if (surChessPieces.adjempty[i + 4] > 0 && surChessPieces.adjempty[i] == 1 && surChessPieces.jumpsame[i] == 2
                && surChessPieces.jumpempty[i] > 0) {
                fourcount++;
                if (fourcount > 1) return true;
            }
            //冲四
            //|0B+00+
            else if (surChessPieces.adjsame[i] == 1
                     && (surChessPieces.adjempty[i] == 0 && surChessPieces.jumpsame[i] == 0
                         && surChessPieces.jumpempty[i] == 0 && surChessPieces.jumpjumpsame[i] == 0)
                     && surChessPieces.adjempty[i + 4] == 1 && surChessPieces.jumpsame[i + 4] == 2
                     && surChessPieces.jumpempty[i + 4] > 0) {
                fourcount++;
                if (fourcount > 1) return true;
            }
            //0B+00|
            else if (surChessPieces.adjsame[i] == 1 && surChessPieces.adjempty[i] > 0
                     && surChessPieces.adjempty[i + 4] == 1 && surChessPieces.jumpsame[i + 4] == 2
                     && (surChessPieces.jumpempty[i + 4] == 0 && surChessPieces.jumpjumpsame[i + 4] == 0)) {
                fourcount++;
                if (fourcount > 1) return true;
            }
            //+00+B0|
            else if (surChessPieces.adjsame[i + 4] == 1
                     && (surChessPieces.adjempty[i + 4] == 0 && surChessPieces.jumpsame[i + 4] == 0
                         && surChessPieces.jumpempty[i + 4] == 0 && surChessPieces.jumpjumpsame[i + 4] == 0)
                     && surChessPieces.adjempty[i] == 1 && surChessPieces.jumpsame[i] == 2
                     && surChessPieces.jumpempty[i] > 0) {
                fourcount++;
                if (fourcount > 1) return true;
            }
            //|00+B0+
            else if (surChessPieces.adjsame[i + 4] == 1 && surChessPieces.adjempty[i + 4] > 0
                     && surChessPieces.adjempty[i] == 1 && surChessPieces.jumpsame[i] == 2
                     && (surChessPieces.jumpempty[i] == 0 && surChessPieces.jumpjumpsame[i] == 0)) {
                fourcount++;
                if (fourcount > 1) return true;
            }
            //|B0+00+
            if ((surChessPieces.adjempty[i] == 0 && surChessPieces.jumpsame[i] == 0 && surChessPieces.jumpempty[i] == 0
                 && surChessPieces.jumpjumpsame[i] == 0)
                && surChessPieces.adjempty[i + 4] == 1 && surChessPieces.jumpsame[i + 4] == 2
                && surChessPieces.jumpempty[i + 4] > 0) {
                fourcount++;
                if (fourcount > 1) return true;
            }
            //+B0+00|
            if (surChessPieces.adjempty[i] > 0 && surChessPieces.adjempty[i + 4] == 1
                && surChessPieces.jumpsame[i + 4] == 2
                && (surChessPieces.jumpempty[i + 4] == 0 && surChessPieces.jumpjumpsame[i + 4] == 0)) {
                fourcount++;
                if (fourcount > 1) return true;
            }
            //+00+0B|
            if ((surChessPieces.adjempty[i + 4] == 0 && surChessPieces.jumpsame[i + 4] == 0
                 && surChessPieces.jumpempty[i + 4] == 0 && surChessPieces.jumpjumpsame[i + 4] == 0)
                && surChessPieces.adjempty[i] == 1 && surChessPieces.jumpsame[i] == 2
                && surChessPieces.jumpempty[i] > 0) {
                fourcount++;
                if (fourcount > 1) return true;
            }
            //|00+0B+
            if (surChessPieces.adjempty[i + 4] > 0 && surChessPieces.adjempty[i] == 1 && surChessPieces.jumpsame[i] == 2
                && (surChessPieces.jumpempty[i] == 0 && surChessPieces.jumpjumpsame[i] == 0)) {
                fourcount++;
                if (fourcount > 1) return true;
            }
        } else if (surChessPieces.adjsame[i] + surChessPieces.adjsame[i + 4] == 0) {
            //+B+000+
            if (surChessPieces.adjempty[i] > 0 && surChessPieces.adjempty[i + 4] == 1
                && surChessPieces.jumpsame[i + 4] == 3 && surChessPieces.jumpempty[i + 4] > 0) {
                fourcount++;
                if (fourcount > 1) return true;
            }
            //+000+B+
            else if (surChessPieces.adjempty[i + 4] > 0 && surChessPieces.adjempty[i] == 1
                     && surChessPieces.jumpsame[i] == 3 && surChessPieces.jumpempty[i] > 0) {
                fourcount++;
                if (fourcount > 1) return true;
            }
            //冲四
            //|B+000+
            else if ((surChessPieces.adjempty[i] == 0 && surChessPieces.jumpsame[i] == 0
                      && surChessPieces.jumpempty[i] == 0 && surChessPieces.jumpjumpsame[i] == 0)
                     && surChessPieces.adjempty[i + 4] == 1 && surChessPieces.jumpsame[i + 4] == 3
                     && surChessPieces.jumpempty[i + 4] > 0) {
                fourcount++;
                if (fourcount > 1) return true;
            }
            //+B+000|
            else if (surChessPieces.adjempty[i] > 0 && surChessPieces.adjempty[i + 4] == 1
                     && surChessPieces.jumpsame[i + 4] == 3
                     && (surChessPieces.jumpempty[i + 4] == 0 && surChessPieces.jumpjumpsame[i + 4] == 0)) {
                fourcount++;
                if (fourcount > 1) return true;
            }
            //+000+B|
            else if ((surChessPieces.adjempty[i + 4] == 0 && surChessPieces.jumpsame[i + 4] == 0
                      && surChessPieces.jumpempty[i + 4] == 0 && surChessPieces.jumpjumpsame[i + 4] == 0)
                     && surChessPieces.adjempty[i] == 1 && surChessPieces.jumpsame[i] == 3
                     && surChessPieces.jumpempty[i] > 0) {
                fourcount++;
                if (fourcount > 1) return true;
            }
            //|000+B+
            else if (surChessPieces.adjempty[i + 4] > 0 && surChessPieces.adjempty[i] == 1
                     && surChessPieces.jumpsame[i] == 3
                     && (surChessPieces.jumpempty[i] == 0 && surChessPieces.jumpjumpsame[i] == 0)) {
                fourcount++;
                if (fourcount > 1) return true;
            }
        }
    }
    if (fourcount > 1) return true;
    return false;
}

// 判断点(x, y)在棋盘chessboard上是否构成禁手，并返回结果
// chessboard[15][15]为当前棋盘
// x为点的横坐标 也就是列数
// y为点的纵坐标，也就是行数
// 返回值为判定结果 无禁手 三三禁手 四四禁手 长连禁手

int ForbiddenCheck(uint8_t chessboard[15][15], int x, int y)
{
    //1.搜索
    SurChessPieces surChessPieces = calSurChessPieces(chessboard, x, y);

    //2.先检查是否成连五，若成连五，黑棋获胜，不构成禁手
    for (int i = 0; i < 4; i++) {
        if (surChessPieces.adjsame[i] + surChessPieces.adjsame[i + 4] == 4) //判断点(x, y)4个方向相邻黑子个数是否等于4
        {
            return NO_FORBIDDEN; //连成5 不构成禁手
        }
    }
    //3.禁手分析 先判断模型 再判断关键点

    for (int i = 0; i < 4; i++) {
        if (surChessPieces.adjsame[i] + surChessPieces.adjsame[i + 4] >= 5) //长连棋型
        {
            return LONG_FORBIDDEN;
        }
        //形成4子相连 需再判断关键点是否有棋子 以及是否在不同方向上都出现活四或活三才出现禁手
        //0---黑子 +表示空位 Q表示边界或白子  ？空位或边界
        //?0000?
        else if (surChessPieces.adjsame[i] + surChessPieces.adjsame[i + 4] == 3) //四字相连
        {
            if (surChessPieces.adjempty[i] > 0 && surChessPieces.adjempty[i + 4] > 0) //构成活四
            {
                if (hasJumpFour(chessboard, x, y)) { return FOUR_FOUR_FORBIDDEN; }
            }
            //构成冲四 |0000+
            else if ((surChessPieces.adjempty[i] == 0 && surChessPieces.jumpempty[i] == 0
                      && surChessPieces.jumpsame[i] == 0 && surChessPieces.jumpjumpsame[i] == 0)
                     && surChessPieces.adjempty[i + 4] > 0) {
                if (hasJumpFour(chessboard, x, y)) { return FOUR_FOUR_FORBIDDEN; }
            }
            //构成冲四 +0000|
            else if ((surChessPieces.adjempty[i + 4] == 0 && surChessPieces.jumpsame[i + 4] == 0
                      && surChessPieces.jumpempty[i + 4] == 0 && surChessPieces.jumpjumpsame[i + 4] == 0)
                     && surChessPieces.adjempty[i] > 0) {
                if (hasJumpFour(chessboard, x, y)) { return FOUR_FOUR_FORBIDDEN; }
            }
        } else if (surChessPieces.adjsame[i] + surChessPieces.adjsame[i + 4] == 2) // 三子相连 ?000?
        {
            //四四禁手判定
            //?+00B+0+?
            if (surChessPieces.adjempty[i] > 0 && surChessPieces.adjempty[i + 4] == 1
                && surChessPieces.jumpsame[i + 4] == 1 && surChessPieces.jumpempty[i + 4] > 0) {
                if (hasJumpFour(chessboard, x, y)) { return FOUR_FOUR_FORBIDDEN; }
            }
            //?+0+B00+?
            else if (surChessPieces.adjempty[i + 4] > 0 && surChessPieces.adjempty[i] == 1
                     && surChessPieces.jumpsame[i] == 1 && surChessPieces.jumpempty[i] > 0) {
                if (hasJumpFour(chessboard, x, y)) { return FOUR_FOUR_FORBIDDEN; }
            }
            //+0B0+0+
            else if (surChessPieces.adjempty[i] > 0 && surChessPieces.adjsame[i] == 1
                     && surChessPieces.adjempty[i + 4] == 1 && surChessPieces.jumpsame[i + 4] == 1
                     && surChessPieces.jumpempty[i + 4] > 0) {
                if (hasJumpFour(chessboard, x, y)) { return FOUR_FOUR_FORBIDDEN; }
            }
            //+0+0B0+
            else if (surChessPieces.adjempty[i + 4] > 0 && surChessPieces.adjsame[i + 4] == 1
                     && surChessPieces.adjempty[i] == 1 && surChessPieces.jumpsame[i] == 1
                     && surChessPieces.jumpempty[i] > 0) {
                if (hasJumpFour(chessboard, x, y)) { return FOUR_FOUR_FORBIDDEN; }
            }
            //+B00+0+
            else if (surChessPieces.adjempty[i] > 0 && surChessPieces.adjempty[i + 4] == 1
                     && surChessPieces.jumpsame[i + 4] == 1 && surChessPieces.jumpempty[i + 4] > 0) {
                if (hasJumpFour(chessboard, x, y)) { return FOUR_FOUR_FORBIDDEN; }
            }
            //+0+00B+
            else if (surChessPieces.adjempty[i + 4] > 0 && surChessPieces.adjempty[i] == 1
                     && surChessPieces.jumpsame[i] == 1 && surChessPieces.jumpempty[i] > 0) {
                if (hasJumpFour(chessboard, x, y)) { return FOUR_FOUR_FORBIDDEN; }
            }
            //冲四判定
            //1.?|00B+0+?
            else if ((surChessPieces.adjempty[i] == 0 && surChessPieces.jumpsame[i] == 0
                      && surChessPieces.jumpempty[i] == 0 && surChessPieces.jumpjumpsame[i] == 0)
                     && surChessPieces.adjempty[i + 4] == 1 && surChessPieces.jumpsame[i + 4] == 1
                     && surChessPieces.jumpempty[i + 4] > 0) {
                if (hasJumpFour(chessboard, x, y)) { return FOUR_FOUR_FORBIDDEN; }
            }
            //?+00B+0|?
            else if (surChessPieces.adjempty[i] > 0 && surChessPieces.adjempty[i + 4] == 1
                     && surChessPieces.jumpsame[i + 4] == 1
                     && (surChessPieces.jumpempty[i + 4] == 0 && surChessPieces.jumpjumpsame[i + 4] == 0)) {
                if (hasJumpFour(chessboard, x, y)) { return FOUR_FOUR_FORBIDDEN; }
            }
            //?+0+B00|?
            else if ((surChessPieces.adjempty[i + 4] == 0 && surChessPieces.jumpsame[i + 4] == 0
                      && surChessPieces.jumpempty[i + 4] == 0 && surChessPieces.jumpjumpsame[i + 4] == 0)
                     && surChessPieces.adjempty[i] == 1 && surChessPieces.jumpsame[i] == 1
                     && surChessPieces.jumpempty[i] > 0) {
                if (hasJumpFour(chessboard, x, y)) { return FOUR_FOUR_FORBIDDEN; }
            }
            //?|0+B00+?
            else if (surChessPieces.adjempty[i + 4] > 0
                     && (surChessPieces.adjempty[i] == 1 && surChessPieces.jumpsame[i] == 1
                         && surChessPieces.jumpempty[i] == 0 && surChessPieces.jumpjumpsame[i] == 0)) {
                if (hasJumpFour(chessboard, x, y)) { return FOUR_FOUR_FORBIDDEN; }
            }
            //+0B0+0|
            else if (surChessPieces.adjempty[i] > 0 && surChessPieces.adjsame[i] == 1
                     && surChessPieces.adjempty[i + 4] == 1 && surChessPieces.jumpsame[i + 4] == 1
                     && (surChessPieces.jumpempty[i + 4] == 0 && surChessPieces.jumpjumpsame[i + 4] == 0)) {
                if (hasJumpFour(chessboard, x, y)) { return FOUR_FOUR_FORBIDDEN; }
            }
            //|0B0+0+
            else if ((surChessPieces.adjempty[i] == 0 && surChessPieces.jumpsame[i] == 0
                      && surChessPieces.jumpempty[i] == 0 && surChessPieces.jumpjumpsame[i] == 0)
                     && surChessPieces.adjsame[i] == 1 && surChessPieces.adjempty[i + 4] == 1
                     && surChessPieces.jumpsame[i + 4] == 1 && surChessPieces.jumpempty[i + 4] > 0) {
                if (hasJumpFour(chessboard, x, y)) { return FOUR_FOUR_FORBIDDEN; }
            }
            //+0+0B0|
            else if ((surChessPieces.adjempty[i + 4] == 0 && surChessPieces.jumpsame[i + 4] == 0
                      && surChessPieces.jumpempty[i + 4] == 0 && surChessPieces.jumpjumpsame[i + 4] == 0)
                     && surChessPieces.adjsame[i + 4] == 1 && surChessPieces.adjempty[i] == 1
                     && surChessPieces.jumpsame[i] == 1 && surChessPieces.jumpempty[i] > 0) {
                if (hasJumpFour(chessboard, x, y)) { return FOUR_FOUR_FORBIDDEN; }
            }
            //+|0+0B0+
            else if (surChessPieces.adjempty[i + 4] > 0 && surChessPieces.adjsame[i + 4] == 1
                     && surChessPieces.adjempty[i] == 1 && surChessPieces.jumpsame[i] == 1
                     && (surChessPieces.jumpempty[i] == 0 && surChessPieces.jumpjumpsame[i] == 0)) {
                if (hasJumpFour(chessboard, x, y)) { return FOUR_FOUR_FORBIDDEN; }
            }
            //|B00+0+
            else if ((surChessPieces.adjempty[i] == 0 && surChessPieces.jumpsame[i] == 0
                      && surChessPieces.jumpempty[i] == 0 && surChessPieces.jumpjumpsame[i] == 0)
                     && surChessPieces.adjempty[i + 4] == 1 && surChessPieces.jumpsame[i + 4] == 1
                     && surChessPieces.jumpempty[i + 4] > 0) {
                if (hasJumpFour(chessboard, x, y)) { return FOUR_FOUR_FORBIDDEN; }
            }
            //+B00+0|
            else if (surChessPieces.adjempty[i] > 0 && surChessPieces.adjempty[i + 4] == 1
                     && surChessPieces.jumpsame[i + 4] == 1
                     && (surChessPieces.jumpempty[i + 4] == 0 && surChessPieces.jumpjumpsame[i + 4] == 0)) {
                if (hasJumpFour(chessboard, x, y)) { return FOUR_FOUR_FORBIDDEN; }
            }
            //|0+00B+
            else if (surChessPieces.adjempty[i + 4] > 0 && surChessPieces.adjempty[i] == 1
                     && surChessPieces.jumpsame[i] == 1
                     && (surChessPieces.jumpempty[i] == 0 && surChessPieces.jumpjumpsame[i] == 0)) {
                if (hasJumpFour(chessboard, x, y)) { return FOUR_FOUR_FORBIDDEN; }
            }
            //+0+00B|
            else if ((surChessPieces.adjempty[i + 4] == 0 && surChessPieces.jumpsame[i + 4] == 0
                      && surChessPieces.jumpempty[i + 4] == 0 && surChessPieces.jumpjumpsame[i + 4] == 0)
                     && surChessPieces.adjempty[i] == 1 && surChessPieces.jumpsame[i] == 1
                     && surChessPieces.jumpempty[i] > 0) {
                if (hasJumpFour(chessboard, x, y)) { return FOUR_FOUR_FORBIDDEN; }
            }
            //三三禁手判定
            if (surChessPieces.adjempty[i] > 0 && surChessPieces.adjempty[i + 4] > 0) //构成活三
            {
                if (hasJumpThree(chessboard, x, y)) { return THREE_THREE_FORBIDDEN; }
            }
        } else if (surChessPieces.adjsame[i] + surChessPieces.adjsame[i + 4] == 1) //二子相连 ?00?
        {
            //四四禁手判定
            //0B+00+
            if (surChessPieces.adjsame[i] == 1 && surChessPieces.adjempty[i] > 0 && surChessPieces.adjempty[i + 4] == 1
                && surChessPieces.jumpsame[i + 4] == 2 && surChessPieces.jumpempty[i + 4] > 0) {
                if (hasJumpFour(chessboard, x, y)) { return FOUR_FOUR_FORBIDDEN; }
            }
            //+00+B0
            else if (surChessPieces.adjsame[i + 4] == 1 && surChessPieces.adjempty[i + 4] > 0
                     && surChessPieces.adjempty[i] == 1 && surChessPieces.jumpsame[i] == 2
                     && surChessPieces.jumpempty[i] > 0) {
                if (hasJumpFour(chessboard, x, y)) { return FOUR_FOUR_FORBIDDEN; }
            }
            //+B0+00+
            if (surChessPieces.adjempty[i] > 0 && surChessPieces.adjempty[i + 4] == 1
                && surChessPieces.jumpsame[i + 4] == 2 && surChessPieces.jumpempty[i + 4] > 0) {
                if (hasJumpFour(chessboard, x, y)) { return FOUR_FOUR_FORBIDDEN; }
            }
            //+00+0B+
            if (surChessPieces.adjempty[i + 4] > 0 && surChessPieces.adjempty[i] == 1 && surChessPieces.jumpsame[i] == 2
                && surChessPieces.jumpempty[i] > 0) {
                if (hasJumpFour(chessboard, x, y)) { return FOUR_FOUR_FORBIDDEN; }
            }
            //冲四
            //|0B+00+
            else if (surChessPieces.adjsame[i] == 1
                     && (surChessPieces.adjempty[i] == 0 && surChessPieces.jumpsame[i] == 0
                         && surChessPieces.jumpempty[i] == 0 && surChessPieces.jumpjumpsame[i] == 0)
                     && surChessPieces.adjempty[i + 4] == 1 && surChessPieces.jumpsame[i + 4] == 2
                     && surChessPieces.jumpempty[i + 4] > 0) {
                if (hasJumpFour(chessboard, x, y)) { return FOUR_FOUR_FORBIDDEN; }
            }
            //+0B+00|
            else if (surChessPieces.adjsame[i] == 1 && surChessPieces.adjempty[i] > 0
                     && surChessPieces.adjempty[i + 4] == 1 && surChessPieces.jumpsame[i + 4] == 2
                     && (surChessPieces.jumpempty[i + 4] == 0 && surChessPieces.jumpjumpsame[i + 4] == 0)) {
                if (hasJumpFour(chessboard, x, y)) { return FOUR_FOUR_FORBIDDEN; }
            }
            //+00+B0|
            else if (surChessPieces.adjsame[i + 4] == 1
                     && (surChessPieces.adjempty[i + 4] == 0 && surChessPieces.jumpsame[i + 4] == 0
                         && surChessPieces.jumpempty[i + 4] == 0 && surChessPieces.jumpjumpsame[i + 4] == 0)
                     && surChessPieces.adjempty[i] == 1 && surChessPieces.jumpsame[i] == 2
                     && surChessPieces.jumpempty[i] > 0) {
                if (hasJumpFour(chessboard, x, y)) { return FOUR_FOUR_FORBIDDEN; }
            }
            //|00+B0+
            else if (surChessPieces.adjsame[i + 4] == 1 && surChessPieces.adjempty[i + 4] > 0
                     && surChessPieces.adjempty[i] == 1 && surChessPieces.jumpsame[i] == 2
                     && (surChessPieces.jumpempty[i] == 0 && surChessPieces.jumpjumpsame[i] == 0)) {
                if (hasJumpFour(chessboard, x, y)) { return FOUR_FOUR_FORBIDDEN; }
            }
            //|B0+00+
            if ((surChessPieces.adjempty[i] == 0 && surChessPieces.jumpsame[i] == 0 && surChessPieces.jumpempty[i] == 0
                 && surChessPieces.jumpjumpsame[i] == 0)
                && surChessPieces.adjempty[i + 4] == 1 && surChessPieces.jumpsame[i + 4] == 2
                && surChessPieces.jumpempty[i + 4] > 0) {
                if (hasJumpFour(chessboard, x, y)) { return FOUR_FOUR_FORBIDDEN; }
            }
            //+B0+00|
            if (surChessPieces.adjempty[i] > 0 && surChessPieces.adjempty[i + 4] == 1
                && surChessPieces.jumpsame[i + 4] == 2
                && (surChessPieces.jumpempty[i + 4] == 0 && surChessPieces.jumpjumpsame[i + 4] == 0)) {
                if (hasJumpFour(chessboard, x, y)) { return FOUR_FOUR_FORBIDDEN; }
            }
            //+00+0B|
            if ((surChessPieces.adjempty[i + 4] == 0 && surChessPieces.jumpsame[i + 4] == 0
                 && surChessPieces.jumpempty[i + 4] == 0 && surChessPieces.jumpjumpsame[i + 4] == 0)
                && surChessPieces.adjempty[i] == 1 && surChessPieces.jumpsame[i] == 2
                && surChessPieces.jumpempty[i] > 0) {
                if (hasJumpFour(chessboard, x, y)) { return FOUR_FOUR_FORBIDDEN; }
            }
            //|00+0B+
            if (surChessPieces.adjempty[i + 4] > 0 && surChessPieces.adjempty[i] == 1 && surChessPieces.jumpsame[i] == 2
                && (surChessPieces.jumpempty[i] == 0 && surChessPieces.jumpjumpsame[i] == 0)) {
                if (hasJumpFour(chessboard, x, y)) { return FOUR_FOUR_FORBIDDEN; }
            }
            //三三禁手判定
            //?+0B+0+?
            if (surChessPieces.adjempty[i + 4] == 1 && surChessPieces.jumpsame[i + 4] == 1
                && surChessPieces.jumpempty[i + 4] > 0 && surChessPieces.adjempty[i] > 1) {
                if (hasJumpThree(chessboard, x, y)) { return THREE_THREE_FORBIDDEN; }
            }
            //?+0+B0+?
            else if (surChessPieces.adjempty[i] == 1 && surChessPieces.jumpsame[i] == 1
                     && surChessPieces.jumpempty[i] > 0 && surChessPieces.adjempty[i + 4] > 1) {
                if (hasJumpThree(chessboard, x, y)) { return THREE_THREE_FORBIDDEN; }
            }
            //?+B0+0+?
            if (surChessPieces.adjempty[i] > 0 && surChessPieces.adjempty[i + 4] == 1
                && surChessPieces.jumpsame[i + 4] == 1 && surChessPieces.jumpempty[i + 4] > 0) {
                if (hasJumpThree(chessboard, x, y)) { return THREE_THREE_FORBIDDEN; }
            }
            //?+0+0B+?
            if (surChessPieces.adjempty[i + 4] > 0 && surChessPieces.adjempty[i] == 1 && surChessPieces.jumpsame[i] == 1
                && surChessPieces.jumpempty[i] > 0) {
                if (hasJumpThree(chessboard, x, y)) { return THREE_THREE_FORBIDDEN; }
            }
        } else if (surChessPieces.adjsame[i] + surChessPieces.adjsame[i + 4] == 0) {
            //四四禁手判定
            //+B+000+
            if (surChessPieces.adjempty[i] > 0 && surChessPieces.adjempty[i + 4] == 1
                && surChessPieces.jumpsame[i + 4] == 3 && surChessPieces.jumpempty[i + 4] > 0) {
                if (hasJumpFour(chessboard, x, y)) { return FOUR_FOUR_FORBIDDEN; }
            }
            //+000+B+
            else if (surChessPieces.adjempty[i + 4] > 0 && surChessPieces.adjempty[i] == 1
                     && surChessPieces.jumpsame[i] == 3 && surChessPieces.jumpempty[i] > 0) {
                if (hasJumpFour(chessboard, x, y)) { return FOUR_FOUR_FORBIDDEN; }
            }
            //冲四判定
            //|B+000+
            else if ((surChessPieces.adjempty[i] == 0 && surChessPieces.jumpsame[i] == 0
                      && surChessPieces.jumpempty[i] == 0 && surChessPieces.jumpjumpsame[i] == 0)
                     && surChessPieces.adjempty[i + 4] == 1 && surChessPieces.jumpsame[i + 4] == 3
                     && surChessPieces.jumpempty[i + 4] > 0) {
                if (hasJumpFour(chessboard, x, y)) { return FOUR_FOUR_FORBIDDEN; }
            }
            //+B+000|
            else if (surChessPieces.adjempty[i] > 0 && surChessPieces.adjempty[i + 4] == 1
                     && surChessPieces.jumpsame[i + 4] == 3
                     && (surChessPieces.jumpempty[i + 4] == 0 && surChessPieces.jumpjumpsame[i + 4] == 0)) {
                if (hasJumpFour(chessboard, x, y)) { return FOUR_FOUR_FORBIDDEN; }
            }
            //+000+B|
            else if ((surChessPieces.adjempty[i + 4] == 0 && surChessPieces.jumpsame[i + 4] == 0
                      && surChessPieces.jumpempty[i + 4] == 0 && surChessPieces.jumpjumpsame[i + 4] == 0)
                     && surChessPieces.adjempty[i] == 1 && surChessPieces.jumpsame[i] == 3
                     && surChessPieces.jumpempty[i] > 0) {
                if (hasJumpFour(chessboard, x, y)) { return FOUR_FOUR_FORBIDDEN; }
            }
            //|000+B+
            else if (surChessPieces.adjempty[i + 4] > 0 && surChessPieces.adjempty[i] == 1
                     && surChessPieces.jumpsame[i] == 3
                     && (surChessPieces.jumpempty[i] == 0 && surChessPieces.jumpjumpsame[i] == 0)) {
                if (hasJumpFour(chessboard, x, y)) { return FOUR_FOUR_FORBIDDEN; }
            }
            //三三禁手判定
            //+B+00+
            if (surChessPieces.adjempty[i] > 0 && surChessPieces.adjempty[i + 4] == 1
                && surChessPieces.jumpsame[i + 4] == 2 && surChessPieces.jumpempty[i + 4] > 0) {
                if (hasJumpThree(chessboard, x, y)) { return THREE_THREE_FORBIDDEN; }
            }
            //+00+B+
            if (surChessPieces.adjempty[i + 4] > 0 && surChessPieces.adjempty[i] == 1 && surChessPieces.jumpsame[i] == 2
                && surChessPieces.jumpempty[i] > 0) {
                if (hasJumpThree(chessboard, x, y)) { return THREE_THREE_FORBIDDEN; }
            }
        }
    }
    //禁手分析结束
    return NO_FORBIDDEN;
}
