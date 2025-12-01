#pragma once

#include <chrono>

class Timer
{
public:
    Timer()
    {
        m_StartTimePoint = std::chrono::high_resolution_clock::now();
    }

    void Reset()
    {
        m_StartTimePoint = std::chrono::high_resolution_clock::now();
    }

    double ElapsedMillis() const
    {
        const auto now = std::chrono::high_resolution_clock::now();
        return static_cast<double>(std::chrono::duration_cast<std::chrono::milliseconds>(now - m_StartTimePoint).count());
    }

private:
    std::chrono::time_point<std::chrono::high_resolution_clock> m_StartTimePoint;
};