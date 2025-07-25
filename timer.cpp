#include "timer.h"
#include <iostream>

Timer::Timer() : totalTime(0), isRunning(false) {}

//开始计时
void Timer::start()
{
    if (!isRunning) {
        startTime = std::chrono::steady_clock::now();
        isRunning = true;
    }
}

//停止计时
long long Timer::stop()
{
    if (isRunning) {
        auto endTime = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        totalTime += elapsed;
        isRunning = false;
        return elapsed.count();
    }
    return 0;
}

//获取累积计时
void Timer::getTotalTime() const
{
    long long elapsedMs = totalTime.count();
    std::cout << "对手下棋累计时间: " << elapsedMs << " 毫秒 (" << (elapsedMs / 1000) << " 秒)" << std::endl
              << std::endl;
}

//检查是否超时
bool Timer::isTimeout() const
{
    return totalTime.count() >= MAX_TIME_MS;
}

//重制计时器
void Timer::reset()
{
    totalTime = std::chrono::milliseconds(0);
    isRunning = false;
}
