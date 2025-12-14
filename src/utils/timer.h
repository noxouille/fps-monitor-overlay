#pragma once

#include <windows.h>

namespace fps_monitor {

/**
 * @brief High-resolution timer wrapper
 * 
 * Wraps QueryPerformanceCounter/QueryPerformanceFrequency for
 * microsecond-precision timing. Used for accurate frame timing.
 */
class Timer {
public:
    /**
     * @brief Construct a new Timer object
     * 
     * Initializes the timer and starts counting.
     */
    Timer();

    /**
     * @brief Destroy the Timer object
     */
    ~Timer();

    /**
     * @brief Start or restart the timer
     * 
     * Resets the timer to zero and begins counting.
     */
    void start();

    /**
     * @brief Reset the timer to zero
     * 
     * Equivalent to calling start().
     */
    void reset();

    /**
     * @brief Get time elapsed since last getDeltaTime() call
     * 
     * Returns delta time in seconds and updates internal timestamp.
     * 
     * @return double Delta time in seconds
     */
    double getDeltaTime();

    /**
     * @brief Get total time elapsed since start()
     * 
     * Does not update internal timestamp.
     * 
     * @return double Elapsed time in seconds
     */
    double getElapsedTime() const;

private:
    LARGE_INTEGER m_frequency;   ///< QueryPerformanceFrequency result
    LARGE_INTEGER m_startTime;   ///< Timer start timestamp
    LARGE_INTEGER m_lastTime;    ///< Last getDeltaTime() timestamp
};

} // namespace fps_monitor
