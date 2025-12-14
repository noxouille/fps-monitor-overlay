#include "timer.h"

namespace fps_monitor {

Timer::Timer() {
    QueryPerformanceFrequency(&m_frequency);
    start();
}

Timer::~Timer() = default;

void Timer::start() {
    QueryPerformanceCounter(&m_startTime);
    m_lastTime = m_startTime;
}

void Timer::reset() {
    start();
}

double Timer::getDeltaTime() {
    LARGE_INTEGER currentTime;
    QueryPerformanceCounter(&currentTime);

    // Calculate delta in seconds
    double deltaSeconds = static_cast<double>(currentTime.QuadPart - m_lastTime.QuadPart) 
                         / static_cast<double>(m_frequency.QuadPart);

    // Update last time
    m_lastTime = currentTime;

    return deltaSeconds;
}

double Timer::getElapsedTime() const {
    LARGE_INTEGER currentTime;
    QueryPerformanceCounter(&currentTime);

    // Calculate elapsed time in seconds
    double elapsedSeconds = static_cast<double>(currentTime.QuadPart - m_startTime.QuadPart)
                           / static_cast<double>(m_frequency.QuadPart);

    return elapsedSeconds;
}

} // namespace fps_monitor
