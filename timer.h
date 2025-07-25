#pragma once
#include <chrono>

class Timer
{
public:
    Timer();

    // 开始计时
    void start();

    // 停止计时并返回本次耗时(毫秒)
    long long stop();

    // 获取累计时间(毫秒)
    void getTotalTime() const;

    // 检查是否超时(15分钟)
    bool isTimeout() const;

    // 重置计时器
    void reset();

private:
    std::chrono::steady_clock::time_point startTime;         //记录开始时间点
    std::chrono::milliseconds totalTime;                     //记录累积时间
    bool isRunning;                                          //计时器是否正在运行
    static constexpr long long MAX_TIME_MS = 15 * 60 * 1000; // 15分钟
};
