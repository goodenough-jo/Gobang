#include "gametree.h"
#include "fivenaction.h"
#include <ios>
using std::ios;
#include <fstream>
using std::ifstream;
using std::ofstream;
#include <iostream>
using std::cin;
using std::cout;
using std::endl;
#include <sstream>
using std::ostringstream;
#include <string>
using std::string;
#include <time.h>
// #include <memory>

using std::pair;
using std::vector;

int ForbiddenCheck(uint8_t chessboard[15][15], int x, int y);
void designatedStartP(); //指定开局先手
void designatedStartF(); //指定开局后手

// 关于比赛信息-----------
string getTime() //获取系统当前时间
{
    time_t timep;
    time(&timep);
    char tmp[64];
    strftime(tmp, sizeof(tmp), "%Y-%m-%d %H:%M:%S", localtime(&timep));
    return tmp;
}
string readFileIntoBuf(string filename)
{
    ifstream ifile(filename);
    ostringstream buf;
    char ch;
    while (buf && ifile.get(ch)) {
        buf.put(ch);
    }
    ifile.close();
    return buf.str();
}
void outPut(string filename)
{
    string info;
    info = readFileIntoBuf(filename); //文件中之前的内容
    //要写入到文件开头的内容
    ofstream out(filename, ios::out | ios::trunc);
    out << "{";
    string s;
    cout << "请输入比赛信息 格式如[C5][贝壳五子棋][五目之魂][后手胜]" << endl;
    cin >> s;
    //1.将比赛信息写入文件；
    out << s;
    //2.将时间写入文件
    out << "[";
    out << getTime();
    out << "线上";
    out << "]";
    //3.将年份写入文件
    out << "[";
    out << "2024 CCGC";
    out << "];" << endl;
    //4.将文件之前双方着点信息写入文件
    out << "文件之前的内容 ";
    out << info;
    out << "}";
}
// 关于比赛信息-----------

vector<pair<uint8_t, uint8_t>> goback; //存放回溯需要的点的坐标,用于输错坐标后的后退
int main()
{
    //选择先手或者后手
    printf("请选择先手或者后手? (0表示先手，1表示后手)\n");
    char choice = getchar();
    if (choice == '0') {
        designatedStartP(); //指定开局
    } else {
        designatedStartF(); //指定开局后手
    }
    return 0;
}

//先手
void designatedStartP()
{
    system("rm /root/博弈项目/out1.txt");
    //输出着点到文件
    ofstream out("/root/博弈项目/out1.txt");
    if (out.fail()) {
        cout << "打开文件失败" << endl;
    }

    //(6斜月 13彗星 22丘月 平衡开局)
    uint32_t identifier = 0;
    cout << "选择指定开局 0表示松月 1表示瑞星 2表示银月 3表示斜月 4表示彗星 5表示丘月:" << endl;
    cin >> identifier;
    uint8_t board[15][15]{};

    switch (identifier) {
    case 0: {
        board[7][7] = 'B';
        out << "B(H,8);";
        board[6][7] = 'W';
        out << "W(H,9);";
        board[8][7] = 'B';
        out << "B(H,7);";
        break;
    }
    case 1: {
        board[7][7] = 'B';
        out << "B(H,8);";
        board[6][7] = 'W';
        out << "W(H,9);";
        board[9][7] = 'B';
        out << "B(H,6);";
        break;
    }
    case 2: {
        board[7][7] = 'B';
        out << "B(H,8);";
        board[6][8] = 'W';
        out << "W(I,9);";
        board[8][7] = 'B';
        out << "B(H,7);";
        break;
    }
    case 3: {
        board[7][7] = 'B';
        out << "B(H,8);";
        board[6][8] = 'W';
        out << "W(I,9);";
        board[8][6] = 'B';
        out << "B(G,7);";
        break;
    }
    case 4: {
        board[7][7] = 'B';
        out << "B(H,8);";
        board[6][8] = 'W';
        out << "W(I,9);";
        board[9][5] = 'B';
        out << "B(F,6);";
        break;
    }
    case 5: {
        board[7][7] = 'B';
        out << "B(H,8);";
        board[6][7] = 'W';
        out << "W(H,9);";
        board[8][8] = 'B';
        out << "B(I,7);";
        break;
    }
    }
    GameTree gt = GameTree(8, 3, board); //修改了探索深度与半径：原本是9,2------------------------------
    gt.showBoard(1); //显示棋盘

    //指定5手N打中N的值
    int N;
    cout << "请问5手N打中N的值为: " << endl;
    cin >> N;
    //输入差错判断
    while (N < 2 || N > 5) {
        cout << "N的值不在正确范围内, 2<=N<=5, 请重新输入: \n";
        cout << "N的值为：";
        cin >> N;
        cout << '\n';
    }

    //询问白方是否进行三手交换
    int answer = 1;
    cout << "请问白方是否交换? 0表示交换 非0的数字表示不交换: " << endl;
    cin >> answer;

    //控制循环过程
    uint32_t x = 0, y = 0;
    uint8_t _x = 0;
    uint32_t _y = 0;
    //u_int32_t _y?

    //不交换,我方执黑棋,需要加五手N打
    if (answer) {
        //第四步 白方下子
        cout << endl << "请输入白方的落子位置\n";
        cout << "白方棋子的横坐标,纵坐标分别为：";
        cin >> _x;
        cin >> _y;
        x = (uint32_t) (15 - _y);
        y = (uint32_t) (_x - 'A');
        cout << "(检测)x = " << x << "," << "y = " << y << endl;

        while (board[x][y] != 0 || x > 14 || y > 14) {
            cout << "白方您的落子点位置不合法,请重新输入\n";
            cout << "白方您方棋子的横坐标,纵坐标分别为：";
            cin >> _x;
            cin >> _y;
            x = (uint32_t) (15 - _y);
            y = (uint32_t) (_x - 'A');
            cout << "\n";
        }

        //在判定之后输出到文件
        out << "W(" << _x << "," << _y << ");";

        board[x][y] = 'W'; //第四步为白棋子

        //第五步棋子 需要同时出现N个棋子 并且需要询问白方选择的棋子
        GameTree gtFive = GameTree(8, 3, board); //修改了探索深度与半径：原本是9,2------------------------
        uint8_t result = gtFive.game(0, 0, N);
        if (result == 'B') {
            cout << "Black Win !" << endl;
            return;
        } else if (result == 'W') {
            cout << "White Win !" << endl;
            return;
        }
        //开局就赢？可能吗？酌情考虑删除----------------------------------------

        cout << "机器打点坐标：" << endl;
        gtFive.showNextPos(0, N); //显示N各棋子的坐标,机器打点
        gtFive.showBoard(0);      //显示棋盘,进入if(five==0)显示五手N打

        //需要询问白方选择的棋子
        cout << "请问白方您选择棋子的横坐标,纵坐标分别为：";
        cin >> _x;
        cin >> _y;
        x = (uint32_t) (15 - _y);
        y = (uint32_t) (_x - 'A');
        cout << '\n';
        while (board[x][y] != 0 || x > 14 || y > 14) {
            cout << "白方您的落子点位置不合法， 请重新输入： \n";
            cout << "白方您棋子的横坐标,纵坐标分别为：";
            cin >> _x;
            cin >> _y;
            x = (uint32_t) (15 - _y);
            y = (uint32_t) (_x - 'A');
            cout << "\n";
        }

        //在判定之后输出到文件
        out << "B(" << _x << "," << _y << ");";

        board[x][y] = 'B'; //记录白方选择的落子点坐标值

        //最佳子节点---------------------------------------------------------------
        gtFive.nodeBest->board[gtFive.nodeBest->fX][gtFive.nodeBest->fY] = 0;
        gtFive.nodeBest->board[x][y] = 'B'; //将最佳子节点设为白方指定的点

        //将多余的点清空为0
        auto pos = gtFive.getNextPos(0, N);
        for (int i = 0; i < N; i++) {
            if (pos[i].first != x && pos[i].second != y) { board[pos[i].first][pos[i].second] = 0; }
        }
        //显示去除点后的棋盘
        gtFive.showBoard(1); //无所谓的值:1,非0即可
        //五手n打结束---------------------------------

        //第六步为白棋子
        cout << "白方您棋子的横坐标,纵坐标分别为：";
        cin >> _x;
        cin >> _y;
        x = (uint32_t) (15 - _y);
        y = (uint32_t) (_x - 'A');
        cout << '\n';
        while (board[x][y] != 0 || x > 14 || y > 14) {
            cout << "白方您的落子点位置不合法， 请重新输入： \n";
            cout << "白方您方棋子的横坐标,纵坐标分别为：";
            cin >> _x;
            cin >> _y;
            x = (uint32_t) (15 - _y);
            y = (uint32_t) (_x - 'A');
            cout << "\n";
        }
        //在判定之后输出到文件
        out << "W(" << _x << "," << _y << ");";

        board[x][y] = 'W'; //第六步为白棋子

        //第7步之后(包括第七步)  第10步才能实现回退-------------------------------???
        for (uint8_t k = 0; k < 220; k++) {
            //记录当前棋局最后两步棋的点的坐标 先白后黑 白点就是根节点的最后落子点 黑点是博弈计算得出的最佳子节点
            GameTree gt = GameTree(8, 3, board); //每循环一次复制一颗博弈树，修改（9,2）为 8,3-----------------------------
            uint8_t result = gt.game(0, 1, 1);
            //创建博弈树，控制博弈搜索过程 每一个子节点到达深度最大时计算估值函数 直到所有叶子节点均已计算出得分 后搜索最大得分的路径，寻找出最佳子节点为nodeBest;

            if (result == 'B') {
                cout << "Black Win !" << endl;
                outPut("/root/博弈项目/out1.txt");
                out.close();
                return;
            } else if (result == 'W') {
                cout << "White Win !" << endl;
                outPut("/root/博弈项目/out1.txt");
                out.close();
                return;
            }
            gt.showNextPos(1, 1); //打印下一步落子点坐标

            //输出到文件
            out << "B(" << (uint8_t) (gt.nodeBest->fY + 'A') << "," << 15 - gt.nodeBest->fX << ");";

            goback.emplace_back(pair<uint8_t, uint8_t>((uint8_t) gt.nodeBest->fX, (uint8_t) gt.nodeBest->fY));

            uint8_t result2 = gt.showBoard(1); //打印棋盘 第一步为黑棋
            if (result2 == 'B') {
                cout << "Black Win !" << endl;
                outPut("/root/博弈项目/out1.txt");
                out.close();
                return;
            } else if (result2 == 'W') {
                cout << "White Win !" << endl;
                outPut("/root/博弈项目/out1.txt");
                return;
            }

            auto pos = gt.getNextPos(1, 1);
            if (pos[0].first != 255 && pos[0].second != 255) {
                board[pos[0].first][pos[0].second] = 'B'; //记录当前棋局最后落子点坐标值为'B'
            }

            //白方输入落子点
            cout << "白方请输入您的落子位置\n";
            cout << "白方您方棋子的横坐标,纵坐标分别为：";
            cin >> _x;
            cin >> _y;
            int flag = 1; //判定是否回退1步(0表示回退1步,1表示不回退)
            x = (uint32_t) (15 - _y);
            y = (uint32_t) (_x - 'A');
            //输错的措施：回退一步：Q键
            if (_x == 'Q') {
                cout << "回溯1步" << endl;
                board[goback[0].first][goback[0].second] = 0;
                board[goback[1].first][goback[1].second] = 0;
                gt.nodeBest->board[goback[0].first][goback[0].second] = 0;
                gt.nodeBest->board[goback[1].first][goback[1].second] = 0;
                //重现棋盘
                gt.showBoard(1);
                //再次询问修改后的落子点坐标
                cout << "白方请输入您的落子位置\n";
                cout << "白方您方棋子的横坐标,纵坐标分别为：";
                cin >> _x;
                cin >> _y;
                x = (uint32_t) (15 - _y);
                y = (uint32_t) (_x - 'A');
                cout << '\n';
                while ((board[x][y] != 0 || x > 14 || y > 14) && flag == 1) {
                    cout << "白方您的落子点位置不合法， 请重新输入： \n";
                    cout << "白方您方棋子的横坐标,纵坐标分别为：";
                    cin >> _x;
                    cin >> _y;
                    x = (uint32_t) (15 - _y);
                    y = (uint32_t) (_x - 'A');
                    cout << "\n";
                }
                //在回溯之后输出到文件
                out << "回溯一步 需要删除前面两个坐标 修改后白方的落子位置为：";
                out << "W(" << _x << "," << _y << ");";
                out << "修改完成;";
                board[x][y] = 'W';
                flag = 0;
            }
            cout << '\n';
            while ((board[x][y] != 0 || x > 14 || y > 14) && flag == 1) {
                cout << "白方您的落子点位置不合法， 请重新输入： \n";
                cout << "白方您方棋子的横坐标,纵坐标分别为：";
                cin >> _x;
                cin >> _y;
                x = (uint32_t) (15 - _y);
                y = (uint32_t) (_x - 'A');
                cout << "\n";
            }

            if (_x != 'Q') {
                out << "W(" << _x << "," << _y << ");";
                board[x][y] = 'W'; //第二步为白棋子
                goback.clear();
                goback.emplace_back(pair<uint8_t, uint8_t>((uint8_t) x, (uint8_t) y)); //将输入的白棋的点的坐标放入容器
            }
        }
    }

    //交换 实现三手交换，我方执白棋，第6步需要选择黑方的落子点（敌方实现五手n打）
    int i = 0; //设置第一次不输入点坐标
    for (uint8_t k = 2; k < 225; k++) {
        if (i != 0 && i != 1) //非第四步和第五步 从第7步输入黑方坐标开始
        {
            cout << "黑方请输入您的落子位置\n";
            cout << "黑方您方棋子的横坐标,纵坐标分别为：";
            cin >> _x;
            cin >> _y;
            int flag = 1; //判定是否回退1步 0表示回退1步， 1表示不回退
            x = (uint32_t) (15 - _y);
            y = (uint32_t) (_x - 'A');
            cout << "\n";
            //输错的措施：回退一步：Q键
            if (_x == 'Q') {
                cout << "回溯1步" << endl;
                //重现棋盘
                board[goback[0].first][goback[0].second] = 0;
                board[goback[1].first][goback[1].second] = 0;

                GameTree gtGoback = GameTree(8, 3, board); //修改（9,2）为 8,3----------------------------
                gtGoback.showBoard(1);

                //再次询问修改后的落子点坐标
                cout << "黑方请输入修改后您的落子位置\n";
                cout << "黑方您方棋子的横坐标,纵坐标分别为：";
                cin >> _x;
                cin >> _y;
                x = (uint32_t) (15 - _y);
                y = (uint32_t) (_x - 'A');
                cout << '\n';
                while ((board[x][y] != 0 || x > 14 || y > 14) && flag == 1) {
                    cout << "黑方您的落子点位置不合法， 请重新输入： \n";
                    cout << "黑方您方棋子的横坐标,纵坐标分别为：";
                    cin >> _x;
                    cin >> _y;
                    x = (uint32_t) (15 - _y);
                    y = (uint32_t) (_x - 'A');
                    cout << "\n";
                }
                //在回溯之后输出到文件
                out << "回溯一步 需要删除前面两个坐标 修改后黑方的落子位置为：";
                out << "B(" << _x << "," << _y << ");";
                out << "修改完成;";

                board[x][y] = 'B';
                flag = 0;
            }
            cout << "\n";
            while ((board[x][y] != 0 || x > 14 || y > 14) && flag == 1) {
                cout << "黑方您的落子点位置不合法,请重新输入： \n";
                cout << "黑方您方棋子的横坐标,纵坐标分别为：";
                cin >> _x;
                cin >> _y;
                x = (uint32_t) (15 - _y);
                y = (uint32_t) (_x - 'A');
                cout << "\n";
            }

            if (_x != 'Q') {
                //在判定之后输出到文件
                out << "B(" << _x << "," << _y << ");";
                board[x][y] = 'B';
                goback.clear();
                goback.emplace_back(pair<uint8_t, uint8_t>((uint8_t) x, (uint8_t) y)); //将输入的黑棋的点的坐标放入容器
            }
        }

        //输入黑方的打点坐标，选择一个棋子
        else if (i == 1) //第5步
        {
            vector<pair<uint8_t, uint8_t>> needSelection; //存放黑方的N个打点坐标

            //---------------自己加的
            fiveNAction judge;
            std::vector<std::pair<int, int>> positions; //存储棋盘的棋子
            judge.getBlackPostions(board);              //获取未打点前的所有黑棋坐标
            //遍历棋盘，存放在position中,以检查对称
            //原先的棋盘x,y与i.first,i.second相对应
            for (int i = 0; i < 15; ++i) {
                for (int j = 0; j < 15; ++j) {
                    if (board[i][j] != 0) { // 非空格子即为落子
                        positions.emplace_back(i, j);
                    }
                }
            }
            // //调试信息:打印未打点前棋盘的坐标
            // std::cout << "打印未打点前棋盘的坐标:\n";
            // for (std::pair<int, int> i : positions) {
            //     cout << "(" << (uint8_t) (i.second + 'A') << "," << 15 - i.first << ")" << endl;
            //     std::cout << i.second << ", " << i.first << "\n";
            // }
            // //调试信息:打印黑子的坐标
            // std::cout << "打印未打点前黑子棋盘的坐标:\n";
            // for (std::pair<int, int> i : judge.blackPositions) {
            //     cout << "(" << (uint8_t) (i.second + 'A') << "," << 15 - i.first << ")" << endl;
            //     std::cout << i.second << ", " << i.first << "\n";
            // }
            //计算原有棋盘的重心
            judge.getSymmetricPoint(positions);
            //计算黑子的重心
            judge.getBlackSymmetricPoint();
            // //调试信息:输出原棋盘的重心,以棋盘x,y为准
            // std::cout << "原棋盘的重心:\n";
            // std::cout << "SymmetriPoint:" << judge.symmetricPoint.second << ", " << judge.symmetricPoint.first << "\n";
            // std::cout << "原棋盘黑子对称点:\n";
            // std::cout << "SymmetriPoint:" << judge.blackSymmetricPoint.second << ", " << judge.blackSymmetricPoint.first
            //           << "\n";
            //------------------------
            cout << "请输入黑方的打点坐标" << endl;
            cout << "黑方棋子的横坐标,纵坐标分别为：" << endl;
            for (uint32_t i = 0; i < N; i++) {
                cin >> _x;
                cin >> _y;
                x = (uint32_t) (15 - _y);
                y = (uint32_t) (_x - 'A');
                //存储转换后的坐标值
                needSelection.emplace_back(pair<uint8_t, uint8_t>((uint8_t) x, (uint8_t) y));

                //-----------将打点放入postions中,以判断对称 保证了positions没有因x,y的变换而污染
                positions.emplace_back(pair<int, int>((int) x, (int) y));
                //同时添加打点到黑棋中
                judge.blackPositions.emplace_back(pair<int, int>((int) x, (int) y));
                // //调试信息:判断打点位置是否正确
                // std::cout << "打印打点棋盘的坐标:\n";
                // for (std::pair<int, int> i : positions) {
                //     cout << "(" << (uint8_t) (i.second + 'A') << "," << 15 - i.first << ")" << endl;
                //     std::cout << i.second << ", " << i.first << "\n";
                // }
                // //调试信息:判断收集的黑子是否正确
                // std::cout << "打印黑子棋盘的坐标:\n";
                // for (std::pair<int, int> i : judge.blackPositions) {
                //     cout << "(" << (uint8_t) (i.second + 'A') << "," << 15 - i.first << ")" << endl;
                //     std::cout << i.second << ", " << i.first << "\n";
                // }
                //------------------
                while (board[x][y] != 0 || x > 14 || y > 14) {
                    cout << "黑方落子点位置不合法,请重新输入： \n";
                    cout << "黑方棋子的横坐标,纵坐标分别为：";
                    cin >> _x;
                    cin >> _y;
                    x = (uint32_t) (15 - _y);
                    y = (uint32_t) (_x - 'A');
                    needSelection.emplace_back(pair<uint8_t, uint8_t>((uint8_t) x, (uint8_t) y));
                    cout << "\n";
                }
            }
            cout << "输入结束" << endl;

            //--------------已经收集完成打点位置与棋盘棋子的所有坐标
            //先判断棋盘是否对称
            if (judge.isSymmetric(positions)) {
                //如果重心与黑子对称点相同则该打点也是对称的
                if (judge.blackSymmetricPoint == judge.symmetricPoint) {
                    std::cout << "检测到对方对称!\n";
                } else {
                    if ((judge.isBlackSymmetric(judge.blackPositions) && judge.isSymmetric(positions))) {
                        std::cout << "检查到对方对称!\n";
                    } else {
                        std::cout << "未检测到对方对称!\n";
                    }
                }
            } else {
                std::cout << "未检查到对方打点对称.\n";
            }
            //检查打点位置是否对称
            //---------------
            /*选择性保留，有关于文件复盘时的价值
            //检查黑方打点位置是否对称---------------------------------------------------------------------------
            int ack = 1;
            auto vp = std::make_unique<fiveNAction>(); //访问指针
            bool check = vp->checkSymmetry(needSelection);
            if (check) {
                cout << "对手黑方打点出现对称，是否继续行棋？(0表示结束，1表示继续)" << endl;
                cin >> ack;
            }
            if (ack == 0) {
                cout << "黑方打点对称，白方胜利，比赛结束!" << endl;
                outPut("/root/博弈项目/out1.txt");
                out.close();
                break;
            }
            //检查黑方打点位置是否对称---------------------------------------------------------------------------
            */

            //计算黑方每一个N点 确定每一个点下之后白方选择最优节点的估值 将估值放入一个容器中
            vector<int32_t> selectValues;
            for (unsigned long k = 0; k < needSelection.size(); k++) {
                board[needSelection[k].first][needSelection[k].second] = 'B'; //将此点设为'B'
                //调用评估函数 得到当前最佳子节点的估值
                GameTree gtSix = GameTree(5, 3, board); //（5,2）修改为5,3----------------------------------
                gtSix.game(1, 1, 1);
                //gtSix.showNextPos(1, 1);//打印下一步落子点坐标
                //将最佳落子点的估值放入容器中
                selectValues.push_back(gtSix.nodeBest->value);
                //输出最佳落子点的估值
                board[needSelection[k].first][needSelection[k].second] = 0; //将此点重置为0
            }
            //选择估值容器中最大估值的下标
            int32_t max = 0;
            for (unsigned long l = 0; l < selectValues.size(); l++) {
                if (selectValues[l] > selectValues[max]) max = l;
            }
            //计算对本方(白方)最有利的黑方落子点的坐标
            _x = needSelection[max].second + 'A';
            _y = 15 - needSelection[max].first;
            cout << "白方选择的点的坐标为"
                 << "(" << _x << "," << _y << ")" << endl;

            //在白方选择之后输出到文件
            out << "B(" << _x << "," << _y << ");";

            board[needSelection[max].first][needSelection[max].second] = 'B';
        }
        i++;
        //判断黑方是否出现禁手点
        int ack = 1;
        int forbidden = ForbiddenCheck(board, x, y);
        if (forbidden != 0) {
            switch (forbidden) {
            case 1: {
                cout << "THREE_THREE_FORBIDDEN(三三）" << endl;
                break;
            }
            case 2: {
                cout << "FOUR_FOUR_FORBIDDEN(四四）" << endl;
                break;
            }
            case 3: {
                cout << "LONG_FORBIDDEN(长连）" << endl;
                break;
            }
            }
            cout << "对手黑方已出现禁手是否继续行棋？(0表示结束，1表示继续)" << endl;
            cin >> ack;
        }
        if (ack == 0) {
            cout << "黑方出现禁手，白方胜利，比赛结束!" << endl;
            outPut("/root/博弈项目/out1.txt");
            out.close();
            break;
        } else {
            //第四步 第六步及以后
            GameTree gt = GameTree(8, 3, board); //原本数值-----------------------------------
            uint8_t result = gt.game(1, 1, 1);
            if (result == 'B') {
                cout << "Black Win !" << endl;
                return;
            } else if (result == 'W') {
                cout << "White Win !" << endl;
                return;
            }
            gt.showNextPos(1, 1); //打印下一步落子点坐标

            //输出到文件
            out << "W(" << (uint8_t) (gt.nodeBest->fY + 'A') << "," << 15 - gt.nodeBest->fX << ");";

            goback.emplace_back(pair<uint8_t, uint8_t>((uint8_t) gt.nodeBest->fX, (uint8_t) gt.nodeBest->fY));

            uint8_t result2 = gt.showBoard(1); //打印棋盘 第一步为白棋

            if (result2 == 'B') {
                cout << "Black Win !" << endl;
                outPut("/root/博弈项目/out1.txt");
                out.close();
                return;
            } else if (result2 == 'W') {
                cout << "White Win !" << endl;
                outPut("/root/博弈项目/out1.txt");
                out.close();
                return;
            }
            auto pos = gt.getNextPos(1, 1);
            if (pos[0].first != 255 && pos[0].second != 255) {
                board[pos[0].first][pos[0].second] = 'W'; //记录当前棋局最后落子点坐标值为'W'
            }
        }
    }
    out.close();
}

//后手，我方执白棋
void designatedStartF()
{
    //输出着点到文件
    system("rm /root/博弈项目/out1.txt");
    ofstream out("/root/博弈项目/out1.txt");
    //手动输入黑方第一步
    cout << endl;
    uint32_t x = 0, y = 0;
    uint8_t _x = 0;
    uint32_t _y = 0;
    uint8_t board[15][15]{};
    cout << "请输入黑1的落子位置\n";
    cout << "黑方棋子的横坐标,纵坐标分别为：";
    cin >> _x;
    cin >> _y;
    x = (uint32_t) (15 - _y);
    y = (uint32_t) (_x - 'A');
    cout << "\n";
    while (board[x][y] != 0 || x > 14 || y > 14) {
        cout << "黑1落子点位置不合法,请重新输入： \n";
        cout << "黑1棋子的横坐标,纵坐标分别为：";
        cin >> _x;
        cin >> _y;
        x = (uint32_t) (15 - _y);
        y = (uint32_t) (_x - 'A');
        cout << "\n";
    }
    //在判定之后输出到文件
    out << "B(" << _x << "," << _y << ");";

    board[x][y] = 'B';
    GameTree gt1 = GameTree(8, 3, board); //原本数值---------------------------------
    gt1.showBoard(1);
    //手动输入白方第二步
    cout << "请输入白2的落子位置\n";
    cout << "白2的横坐标,纵坐标分别为：";
    cin >> _x;
    cin >> _y;
    x = (uint32_t) (15 - _y);
    y = (uint32_t) (_x - 'A');
    cout << "\n";
    while (board[x][y] != 0 || x > 14 || y > 14) {
        cout << "白2落子点位置不合法,请重新输入： \n";
        cout << "白2棋子的横坐标,纵坐标分别为：";
        cin >> _x;
        cin >> _y;
        x = (uint32_t) (15 - _y);
        y = (uint32_t) (_x - 'A');
        cout << "\n";
    }
    //在判定之后输出到文件
    out << "W(" << _x << "," << _y << ");";

    board[x][y] = 'W';
    GameTree gt2 = GameTree(8, 3, board); //原本数值---------------------------------
    gt2.showBoard(1);
    //手动输入黑方第三步
    cout << "请输入黑3的落子位置\n";
    cout << "黑3棋子的横坐标,纵坐标分别为：";
    cin >> _x;
    cin >> _y;
    x = (uint32_t) (15 - _y);
    y = (uint32_t) (_x - 'A');
    cout << "\n";
    while (board[x][y] != 0 || x > 14 || y > 14) {
        cout << "黑3落子点位置不合法,请重新输入： \n";
        cout << "黑3棋子的横坐标,纵坐标分别为：";
        cin >> _x;
        cin >> _y;
        x = (uint32_t) (15 - _y);
        y = (uint32_t) (_x - 'A');
        cout << "\n";
    }
    //在判定之后输出到文件
    out << "B(" << _x << "," << _y << ");";

    board[x][y] = 'B';
    GameTree gt3 = GameTree(8, 3, board); //原本数值---------------------------------
    gt3.showBoard(1);

    //指定5手N打中N的值
    int N;
    cout << "请问5手N打中N的值为: " << endl;
    cin >> N;
    cout << '\n';
    while (N < 2 || N > 5) {
        cout << "N的值不在正确范围内, 2<=N<=5, 请重新输入: \n";
        cout << "N的值为：";
        cin >> N;
        cout << '\n';
    }

    //实现白方三手交换
    bool change = 1; //默认不交换
    int start;
    cout << "请输入黑方的指定开局类型:" << endl;
    cout << "1——浦月;2——恒星;3——云月;4——水月;5——峡月;6——斜月;7——长星;8——明星;" << endl;
    cout << "9——流星;10——岚月;11——银月;12——名月;13——彗星;14——花月;15——寒星;16——雨月;" << endl;
    cout << "17——金星;18——残月;19——新月;20——山月;21——溪月;22——丘月;23——松月;24——瑞星;" << endl;
    cout << "25——疏星;26——游星" << endl;
    cout << "请输入开局类型: (1——26)" << endl;
    cout << "测试:\n" << "提示，黑方指定开局为1,2,3,4,5,8,10,11,20,21,23,24,25,26时，黑方优势较大" << endl;
    cin >> start;
    //26：白方交换
    switch (start) {
    //黑方优势较大，交换，我方成为执黑棋的一方
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 8:
    case 10:
    case 11:
    // case 12:
    // case 14:
    // case 15:
    // case 16:
    // case 17:
    // case 18:
    // case 19:
    case 20:
    case 21:
    case 23:
    case 24:
    case 25:
    case 26: {
        cout << endl << "白方交换" << endl;
        change = 0;
        break;
    }
    //黑白平衡 不交换
    case 6:
    case 7:
    case 9:
    case 13:
    case 22:
    case 12:
    case 14:
    case 15:
    case 16:
    case 17:
    case 18:
    case 19: {
        cout << endl << "白方不交换 正在计算白方落子点 请等待" << endl;
        break;
    }
    }

    //白方不交换
    if (change) {
        //第四步 机器算出白方落子点
        GameTree gt4 = GameTree(8, 3, board); //原本数值---------------------------------
        gt4.game(1, 1, 1);
        gt4.showNextPos(1, 1); //打印白4落子点坐标

        //输出到文件
        out << "W(" << (uint8_t) (gt4.nodeBest->fY + 'A') << "," << 15 - gt4.nodeBest->fX << ");";

        gt4.showBoard(1); //打印棋盘

        auto pos = gt4.getNextPos(1, 1);
        if (pos[0].first != 255 && pos[0].second != 255) {
            board[pos[0].first][pos[0].second] = 'W'; //记录当前棋局白4坐标值为'W'
        }

        int i = 0; //记录第5步
        //第5步棋子之后(包括第5步)
        vector<pair<uint8_t, uint8_t>> needSelection; //存放黑方的N个打点坐标
        for (uint8_t k = 0; k < 250; k++) {
            if (i == 0) {
                //---------添加判断对称功能
                fiveNAction judge;
                std::vector<std::pair<int, int>> positions; //存储棋盘的棋子
                judge.getBlackPostions(board);              //获取未打点前的所有黑棋坐标
                //遍历棋盘，存放在position中,以检查对称
                //原先的棋盘x,y与i.first,i.second相对应
                for (int i = 0; i < 15; ++i) {
                    for (int j = 0; j < 15; ++j) {
                        if (board[i][j] != 0) { // 非空格子即为落子
                            positions.emplace_back(i, j);
                        }
                    }
                }
                // //调试信息:打印未打点前棋盘的坐标
                // std::cout << "打印未打点前棋盘的坐标:\n";
                // for (std::pair<int, int> i : positions) {
                //     cout << "(" << (uint8_t) (i.second + 'A') << "," << 15 - i.first << ")" << endl;
                //     std::cout << i.second << ", " << i.first << "\n";
                // }
                // //调试信息:打印黑子的坐标
                // std::cout << "打印未打点前黑子棋盘的坐标:\n";
                // for (std::pair<int, int> i : judge.blackPositions) {
                //     cout << "(" << (uint8_t) (i.second + 'A') << "," << 15 - i.first << ")" << endl;
                //     std::cout << i.second << ", " << i.first << "\n";
                // }
                //计算原有棋盘的重心
                judge.getSymmetricPoint(positions);
                //计算黑子的重心
                judge.getBlackSymmetricPoint();
                // //调试信息:输出原棋盘的重心,以棋盘x,y为准
                // std::cout << "原棋盘的重心:\n";
                // std::cout << "SymmetriPoint:" << judge.symmetricPoint.second << ", " << judge.symmetricPoint.first
                //           << "\n";
                // std::cout << "原棋盘黑子对称点:\n";
                // std::cout << "SymmetriPoint:" << judge.blackSymmetricPoint.second << ", "
                //           << judge.blackSymmetricPoint.first << "\n";
                //-----------
                //让对手输入打点位置
                cout << "请输入黑方的打点坐标" << endl;
                cout << "黑方棋子的横坐标,纵坐标分别为：" << endl;
                for (uint32_t i = 0; i < N; i++) {
                    cin >> _x;
                    cin >> _y;
                    x = (uint32_t) (15 - _y);
                    y = (uint32_t) (_x - 'A');
                    //存储转换后的坐标值
                    needSelection.emplace_back(pair<uint8_t, uint8_t>((uint8_t) x, (uint8_t) y));
                    cout << "\n";
                    //-----------将打点放入postions中,以判断对称 保证了positions没有因x,y的变换而污染
                    positions.emplace_back(pair<int, int>((int) x, (int) y));
                    //同时添加打点到黑棋中
                    judge.blackPositions.emplace_back(pair<int, int>((int) x, (int) y));
                    // //调试信息:判断打点位置是否正确
                    // std::cout << "打印打点棋盘的坐标:\n";
                    // for (std::pair<int, int> i : positions) {
                    //     cout << "(" << (uint8_t) (i.second + 'A') << "," << 15 - i.first << ")" << endl;
                    //     std::cout << i.second << ", " << i.first << "\n";
                    // }
                    // //调试信息:判断收集的黑子是否正确
                    // std::cout << "打印黑子棋盘的坐标:\n";
                    // for (std::pair<int, int> i : judge.blackPositions) {
                    //     cout << "(" << (uint8_t) (i.second + 'A') << "," << 15 - i.first << ")" << endl;
                    //     std::cout << i.second << ", " << i.first << "\n";
                    // }
                    //------------
                    while (board[x][y] != 0 || x > 14 || y > 14) {
                        cout << "黑方落子点位置不合法,请重新输入： \n";
                        cout << "黑方棋子的横坐标,纵坐标分别为：";
                        cin >> _x;
                        cin >> _y;
                        x = (uint32_t) (15 - _y);
                        y = (uint32_t) (_x - 'A');
                        needSelection.emplace_back(pair<uint8_t, uint8_t>((uint8_t) x, (uint8_t) y));
                        cout << "\n";
                    }
                }
                cout << "输入结束" << endl;

                //--------------已经收集完成打点位置与棋盘棋子的所有坐标
                //先判断棋盘是否对称
                if (judge.isSymmetric(positions)) {
                    //如果重心与黑子对称点相同则该打点也是对称的
                    if (judge.blackSymmetricPoint == judge.symmetricPoint) {
                        std::cout << "检测到对方对称!\n";
                    } else {
                        if ((judge.isBlackSymmetric(judge.blackPositions) && judge.isSymmetric(positions))) {
                            std::cout << "检查到对方对称!\n";
                        } else {
                            std::cout << "未检测到对方对称!\n";
                        }
                    }
                } else {
                    std::cout << "未检查到对方打点对称.\n";
                }
                //检查打点位置是否对称

                //---------------
                /*
                //检查黑方打点位置是否对称---------------------------------------------------------------------------
                int ack = 1;
                auto vp = std::make_unique<fiveNAction>(); //访问指针
                bool check = vp->checkSymmetry(needSelection);
                if (check) {
                    cout << "对手黑方打点出现对称，是否继续行棋？(0表示结束，1表示继续)" << endl;
                    cin >> ack;
                }
                if (ack == 0) {
                    cout << "黑方打点对称，白方胜利，比赛结束!" << endl;
                    outPut("/root/博弈项目/out1.txt");
                    out.close();
                    break;
                }
                //检查黑方打点位置是否对称---------------------------------------------------------------------------
*/

                //计算黑方每一个N点 确定每一个点下之后白方选择最优节点的估值 将估值放入一个容器中
                vector<int32_t> selectValues;
                for (unsigned long k = 0; k < needSelection.size(); k++) {
                    board[needSelection[k].first][needSelection[k].second] = 'B'; //将此点设为'B'
                    //调用评估函数 得到当前最佳子节点的估值
                    GameTree gtSix = GameTree(5, 3, board); //原本数值-----------------------------------
                    gtSix.game(1, 1, 1);
                    //gtSix.showNextPos(1, 1);//打印下一步落子点坐标
                    //将最佳落子点的估值放入容器中
                    selectValues.push_back(gtSix.nodeBest->value);
                    //输出最佳落子点的估值
                    //                cout << "第" << k << "个最佳落子点估值为" << gtSix.nodeBest->value << endl;
                    board[needSelection[k].first][needSelection[k].second] = 0; //将此点重置为0
                }
                //选择估值容器中最大估值的下标
                int32_t max = 0;
                for (unsigned long l = 0; l < selectValues.size(); l++) {
                    if (selectValues[l] > selectValues[max]) max = l;
                }
                _x = needSelection[max].second + 'A';
                _y = 15 - needSelection[max].first;
                cout << "白方选择的点的坐标为"
                     << "(" << _x << "," << _y << ")" << endl;

                //在判定之后输出到文件
                out << "B(" << _x << "," << _y << ");";

                board[needSelection[max].first][needSelection[max].second] = 'B';
                i++;
            }
            if (i == 1) {
                i++;
            } else {
                cout << "黑方请输入您的落子位置\n";
                cout << "黑方您方棋子的横坐标,纵坐标分别为：";
                cin >> _x;
                cin >> _y;
                int flag = 1; //判定是否回退1步 0表示回退1步， 1表示不回退
                x = (uint32_t) (15 - _y);
                y = (uint32_t) (_x - 'A');
                cout << "\n";
                //输错的措施：回退一步：Q键
                if (_x == 'Q') {
                    cout << "回溯1步" << endl;

                    //重现棋盘

                    board[goback[0].first][goback[0].second] = 0;
                    board[goback[1].first][goback[1].second] = 0;

                    GameTree gtGoback = GameTree(8, 3, board); //原本数值---------------------------------
                    gtGoback.showBoard(1);

                    //再次询问修改后的落子点坐标
                    cout << "黑方请输入修改后您的落子位置\n";
                    cout << "黑方您方棋子的横坐标,纵坐标分别为：";
                    cin >> _x;
                    cin >> _y;
                    x = (uint32_t) (15 - _y);
                    y = (uint32_t) (_x - 'A');
                    cout << '\n';
                    while ((board[x][y] != 0 || x > 14 || y > 14) && flag == 1) {
                        cout << "黑方您的落子点位置不合法， 请重新输入： \n";
                        cout << "黑方您方棋子的横坐标,纵坐标分别为：";
                        cin >> _x;
                        cin >> _y;
                        x = (uint32_t) (15 - _y);
                        y = (uint32_t) (_x - 'A');
                        cout << "\n";
                    }
                    //在判定之后输出到文件
                    out << "回溯一步 需要删除前面两个坐标 修改后黑方的落子位置为：";
                    out << "B(" << _x << "," << _y << ");";
                    out << "修改完成;";

                    board[x][y] = 'B';
                    flag = 0;
                }
                cout << "\n";
                while ((board[x][y] != 0 || x > 14 || y > 14) && flag == 1) {
                    cout << "黑方您的落子点位置不合法， 请重新输入： \n";
                    cout << "黑方您方棋子的横坐标,纵坐标分别为：";
                    cin >> _x;
                    cin >> _y;
                    x = (uint32_t) (15 - _y);
                    y = (uint32_t) (_x - 'A');
                    cout << "\n";
                }

                if (_x != 'Q') {
                    //在判定之后输出到文件
                    out << "B(" << _x << "," << _y << ");";
                    board[x][y] = 'B';
                    goback.clear();
                    goback.emplace_back(pair<uint8_t, uint8_t>((uint8_t) x, (uint8_t) y)); //将输入的黑棋的点的坐标放入容器
                }
            }
            //判断对方是否出现禁手
            int ack = 1;
            int forbidden = ForbiddenCheck(board, x, y);
            if (forbidden != 0) {
                switch (forbidden) {
                case 1: {
                    cout << "THREE_THREE_FORBIDDEN" << endl;
                    break;
                }
                case 2: {
                    cout << "FOUR_FOUR_FORBIDDEN" << endl;
                    break;
                }
                case 3: {
                    cout << "LONG_FORBIDDEN" << endl;
                    break;
                }
                }
                cout << "对手黑方已出现禁手是否继续行棋？(0表示结束，1表示继续)" << endl;
                cin >> ack;
            }
            if (ack == 0) {
                cout << "黑方出现禁手，白方胜利，比赛结束!" << endl;
                outPut("/root/博弈项目/out1.txt");
                out.close();
                break;
            } else {
                GameTree gt = GameTree(8, 3, board); //原本数值---------------------------------
                uint8_t result = gt.game(1, 1, 1);
                if (result == 'B') {
                    //gt.showBoard(false);
                    cout << "Black Win !" << endl;
                    outPut("/root/博弈项目/out1.txt");
                    out.close();
                    return;
                } else if (result == 'W') {
                    //gt.showBoard(false);
                    cout << "White Win !" << endl;
                    outPut("/root/博弈项目/out1.txt");
                    out.close();
                    return;
                }
                gt.showNextPos(1, 1); //打印下一步落子点坐标

                //输出到文件
                out << "W(" << (uint8_t) (gt.nodeBest->fY + 'A') << "," << 15 - gt.nodeBest->fX << ");";

                goback.emplace_back(pair<uint8_t, uint8_t>((uint8_t) gt.nodeBest->fX, (uint8_t) gt.nodeBest->fY));
                uint8_t result2 = gt.showBoard(1); //打印棋盘 第一步为白棋
                if (result2 == 'B') {
                    cout << "Black Win !" << endl;
                    outPut("/root/博弈项目/out1.txt");
                    out.close();
                    return;
                } else if (result2 == 'W') {
                    //gt.showBoard(false);
                    cout << "White Win !" << endl;
                    outPut("/root/博弈项目/out1.txt");
                    out.close();
                    return;
                }
                auto pos = gt.getNextPos(1, 1);
                if (pos[0].first != 255 && pos[0].second != 255) {
                    board[pos[0].first][pos[0].second] = 'W'; //记录当前棋局最后落子点坐标值为'W'
                    //            printf("%d's 循环， 当前深度%d\n", k, gt.nodeBest->depth);
                    //printf("pos.first is %d\n, pos.second is %d\n", pos.first, pos.second);
                    //初始位置为7,7
                }
            }
        }
    }
    //白方交换,我方执黑棋，要实现五手n打
    else {
        //1.交换后第四步 输入白方落子位置
        cout << "请输入白方的落子位置\n";
        cout << "白方棋子的横坐标,纵坐标分别为：";
        cin >> _x;
        cin >> _y;
        x = (uint32_t) (15 - _y);
        y = (uint32_t) (_x - 'A');

        cout << '\n';

        while (board[x][y] != 0 || x > 14 || y > 14) {
            cout << "白方您的落子点位置不合法， 请重新输入： \n";
            cout << "白方您方棋子的横坐标,纵坐标分别为：";
            cin >> _x;
            cin >> _y;
            x = (uint32_t) (15 - _y);
            y = (uint32_t) (_x - 'A');
            cout << "\n";
        }
        //在判定之后输出到文件
        out << "W(" << _x << "," << _y << ");";

        board[x][y] = 'W'; //第四步为白棋子
        //2.1 第五步棋子 第五步棋子 需要同时出现N个棋子 并且需要询问白方选择的棋子
        GameTree gtFive = GameTree(8, 3, board); //原本数值---------------------------------
        uint8_t result = gtFive.game(0, 0, N);
        if (result == 'B') {
            //gt.showBoard(false);
            cout << "Black Win !" << endl;
            return;
        } else if (result == 'W') {
            //gt.showBoard(false);
            cout << "White Win !" << endl;
            return;
        }
        cout << "机器打点坐标：" << endl;
        gtFive.showNextPos(0, N);
        gtFive.showBoard(0);
        //2.2 需要询问白方选择的棋子
        cout << "请问白方您选择棋子的横坐标,纵坐标分别为：";
        cin >> _x;
        cin >> _y;
        x = (uint32_t) (15 - _y);
        y = (uint32_t) (_x - 'A');
        cout << '\n';
        while (board[x][y] != 0 || x > 14 || y > 14) {
            cout << "白方您的落子点位置不合法， 请重新输入： \n";
            cout << "白方您棋子的横坐标,纵坐标分别为：";
            cin >> _x;
            cin >> _y;
            x = (uint32_t) (15 - _y);
            y = (uint32_t) (_x - 'A');
            cout << "\n";
        }

        //在判定之后输出到文件
        out << "B(" << _x << "," << _y << ");";

        board[x][y] = 'B'; //记录白方选择的落子点坐标值
        //显示去除点后的棋盘

        gtFive.nodeBest->board[gtFive.nodeBest->fX][gtFive.nodeBest->fY] = 0;
        gtFive.nodeBest->board[x][y] = 'B'; //将最佳子节点设为白方指定的点

        //将多余的点清空为0
        auto pos = gtFive.getNextPos(0, N);
        for (int i = 0; i < N; i++) {
            if (pos[i].first != x && pos[i].second != y) { board[pos[i].first][pos[i].second] = 0; }
        }
        //2.3显示去除点后的棋盘
        gtFive.showBoard(1);

        //3. 第六步为白棋子
        cout << "白方您棋子的横坐标,纵坐标分别为：";
        cin >> _x;
        cin >> _y;
        x = (uint32_t) (15 - _y);
        y = (uint32_t) (_x - 'A');

        cout << '\n';
        while (board[x][y] != 0 || x > 14 || y > 14) {
            cout << "白方您的落子点位置不合法， 请重新输入： \n";
            cout << "白方您方棋子的横坐标,纵坐标分别为：";
            cin >> _x;
            cin >> _y;
            x = (uint32_t) (15 - _y);
            y = (uint32_t) (_x - 'A');
            cout << "\n";
        }
        //在判定之后输出到文件
        out << "W(" << _x << "," << _y << ");";

        board[x][y] = 'W'; //第六步为白棋子
        //4. 第7步之后(包括第七步)  第10步才能实现回退------------------------???
        for (uint8_t k = 0; k < 220; k++) {
            //记录当前棋局最后两步棋的点的坐标 先白后黑 白点就是根节点的最后落子点 黑点是博弈计算得出的最佳子节点
            GameTree gt = GameTree(8, 3, board); //每循环一次复制一颗博弈树，修改（9,2）为 8,3---------------------------
            uint8_t result = gt.game(0, 1, 1);
            //创建博弈树，控制博弈搜索过程 每一个子节点到达深度最大时计算估值函数 直到所有叶子节点均已计算出得分 后搜索最大得分的路径，寻找出最佳子节点为nodeBest;
            if (result == 'B') {
                //gt.showBoard(false);
                cout << "Black Win !" << endl;
                outPut("/root/博弈项目/out1.txt");
                out.close();
                return;
            } else if (result == 'W') {
                //gt.showBoard(false);
                cout << "White Win !" << endl;
                outPut("/root/博弈项目/out1.txt");
                out.close();
                return;
            }
            gt.showNextPos(1, 1); //打印下一步落子点坐标

            //输出到文件
            out << "B(" << (uint8_t) (gt.nodeBest->fY + 'A') << "," << 15 - gt.nodeBest->fX << ");";

            goback.emplace_back(pair<uint8_t, uint8_t>((uint8_t) gt.nodeBest->fX, (uint8_t) gt.nodeBest->fY));

            uint8_t result2 = gt.showBoard(1); //打印棋盘 第一步为黑棋
            if (result2 == 'B') {
                cout << "Black Win !" << endl;
                outPut("/root/博弈项目/out1.txt");
                out.close();
                return;
            } else if (result2 == 'W') {
                //gt.showBoard(false);
                cout << "White Win !" << endl;
                outPut("/root/博弈项目/out1.txt");
                out.close();
                return;
            }
            auto pos = gt.getNextPos(1, 1);
            if (pos[0].first != 255 && pos[0].second != 255) {
                board[pos[0].first][pos[0].second] = 'B'; //记录当前棋局最后落子点坐标值为'B'
                //            printf("%d's 循环， 当前深度%d\n", k, gt.nodeBest->depth);
            }

            //白方输入落子点

            cout << "白方请输入您的落子位置\n";
            cout << "白方您方棋子的横坐标,纵坐标分别为：";
            cin >> _x;
            cin >> _y;
            int flag = 1; //判定是否回退1步 0表示回退1步， 1表示不回退
            x = (uint32_t) (15 - _y);
            y = (uint32_t) (_x - 'A');
            //输错的措施：回退一步：Q键
            if (_x == 'Q') {
                cout << "回溯1步" << endl;
                board[goback[0].first][goback[0].second] = 0;
                board[goback[1].first][goback[1].second] = 0;
                gt.nodeBest->board[goback[0].first][goback[0].second] = 0;
                gt.nodeBest->board[goback[1].first][goback[1].second] = 0;
                //重现棋盘
                gt.showBoard(1);
                //再次询问修改后的落子点坐标
                cout << "白方请输入您的落子位置\n";
                cout << "白方您方棋子的横坐标,纵坐标分别为：";
                cin >> _x;
                cin >> _y;
                x = (uint32_t) (15 - _y);
                y = (uint32_t) (_x - 'A');
                cout << '\n';
                while ((board[x][y] != 0 || x > 14 || y > 14) && flag == 1) {
                    cout << "白方您的落子点位置不合法， 请重新输入： \n";
                    cout << "白方您方棋子的横坐标,纵坐标分别为：";
                    cin >> _x;
                    cin >> _y;
                    x = (uint32_t) (15 - _y);
                    y = (uint32_t) (_x - 'A');
                    cout << "\n";
                }
                //在回溯之后输出到文件
                out << "回溯一步 需要删除前面两个坐标 修改后白方的落子位置为：";
                out << "W(" << _x << "," << _y << ");";
                out << "修改完成;";
                board[x][y] = 'W';
                flag = 0;
            }
            cout << '\n';
            while ((board[x][y] != 0 || x > 14 || y > 14) && flag == 1) {
                cout << "白方您的落子点位置不合法， 请重新输入： \n";
                cout << "白方您方棋子的横坐标,纵坐标分别为：";
                cin >> _x;
                cin >> _y;
                x = (uint32_t) (15 - _y);
                y = (uint32_t) (_x - 'A');
                cout << "\n";
            }

            if (_x != 'Q') {
                out << "W(" << _x << "," << _y << ");";
                board[x][y] = 'W'; //第二步为白棋子
                goback.clear();
                goback.emplace_back(pair<uint8_t, uint8_t>((uint8_t) x, (uint8_t) y)); //将输入的白棋的点的坐标放入容器
            }
        }
    }
}
