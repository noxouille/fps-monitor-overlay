#pragma once

#include <windows.h>
#include <memory>
#include <vector>
#include "ring_buffer.h"

namespace fps_monitor {

/**
 * @brief High-precision FPS calculation engine
 * 
 * Uses QueryPerformanceCounter for microsecond precision timing.
 * Calculates instantaneous FPS and maintains a rolling average.
 * Stores samples in a ring buffer for graph visualization.
 */
class FpsCalculator {
public:
    /**
     * @brief Construct a new FPS Calculator
     * 
     * @param historySize Number of samples to keep for averaging and graphing
     */
    explicit FpsCalculator(size_t historySize = 120);

    /**
     * @brief Destroy the FPS Calculator
     */
    ~FpsCalculator();

    /**
     * @brief Update the FPS calculator with a new frame
     * 
     * Should be called once per frame. Calculates delta time and FPS.
     * 
     * @param deltaTime Frame time in seconds (from high-resolution timer)
     */
    void update(double deltaTime);

    /**
     * @brief Get the current instantaneous FPS
     * 
     * @return double Current FPS value
     */
    double getCurrentFPS() const;

    /**
     * @brief Get the rolling average FPS
     * 
     * Averaged over the configured history window.
     * 
     * @return double Average FPS value
     */
    double getAverageFPS() const;

    /**
     * @brief Get all FPS samples for graph rendering
     * 
     * Returns samples in chronological order (oldest to newest).
     * 
     * @return std::vector<double> Vector of FPS samples
     */
    std::vector<double> getSamples() const;

    /**
     * @brief Get the number of samples currently stored
     * 
     * @return size_t Number of samples
     */
    size_t getSampleCount() const;

    /**
     * @brief Reset the calculator, clearing all samples
     */
    void reset();

private:
    /**
     * @brief Calculate average from current samples
     * 
     * @return double The calculated average FPS
     */
    double calculateAverage() const;

    static constexpr size_t MAX_HISTORY = 600; ///< Maximum samples (10 seconds at 60 FPS)
    
    std::unique_ptr<RingBuffer<double, MAX_HISTORY>> m_samples; ///< FPS sample storage
    double m_currentFPS;                                         ///< Current instantaneous FPS
    double m_averageFPS;                                         ///< Rolling average FPS
    size_t m_historySize;                                        ///< Configured history size
    LARGE_INTEGER m_frequency;                                   ///< QueryPerformanceFrequency result
};

} // namespace fps_monitor
