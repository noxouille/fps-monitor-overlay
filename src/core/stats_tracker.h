#pragma once

#include <vector>
#include <chrono>

namespace fps_monitor {

/**
 * @brief Performance statistics calculator
 * 
 * Calculates percentile-based metrics (0.1% lows, 1% lows) and
 * basic statistics (min, max, average) from FPS samples.
 * Updates periodically for performance efficiency.
 */
class StatsTracker {
public:
    /**
     * @brief Structure containing calculated statistics
     */
    struct Stats {
        double average;        ///< Mean FPS
        double min;            ///< Minimum FPS
        double max;            ///< Maximum FPS
        double percentile01;   ///< 0.1% low FPS
        double percentile1;    ///< 1% low FPS
    };

    /**
     * @brief Construct a new Stats Tracker
     * 
     * @param updateIntervalMs Milliseconds between stats updates (default: 500ms)
     */
    explicit StatsTracker(int updateIntervalMs = 500);

    /**
     * @brief Destroy the Stats Tracker
     */
    ~StatsTracker();

    /**
     * @brief Update tracker with new FPS samples
     * 
     * Only recalculates statistics if update interval has elapsed.
     * 
     * @param samples Vector of FPS samples to analyze
     */
    void update(const std::vector<double>& samples);

    /**
     * @brief Get the current statistics
     * 
     * @return const Stats& Reference to current stats
     */
    const Stats& getStats() const;

    /**
     * @brief Get 0.1% low FPS value
     * 
     * @return double 0.1% percentile
     */
    double get01PercentLow() const;

    /**
     * @brief Get 1% low FPS value
     * 
     * @return double 1% percentile
     */
    double get1PercentLow() const;

    /**
     * @brief Get minimum FPS
     * 
     * @return double Minimum value
     */
    double getMin() const;

    /**
     * @brief Get maximum FPS
     * 
     * @return double Maximum value
     */
    double getMax() const;

    /**
     * @brief Get average FPS
     * 
     * @return double Average value
     */
    double getAverage() const;

    /**
     * @brief Reset all statistics
     */
    void reset();

private:
    /**
     * @brief Calculate all statistics from samples
     * 
     * @param samples Samples to analyze
     */
    void calculateStats(const std::vector<double>& samples);

    /**
     * @brief Calculate percentile value from sorted samples
     * 
     * @param sortedSamples Pre-sorted sample vector
     * @param percentile Percentile to calculate (0.0 - 1.0)
     * @return double The percentile value
     */
    double calculatePercentile(const std::vector<double>& sortedSamples, double percentile) const;

    Stats m_stats;                                        ///< Current statistics
    std::chrono::steady_clock::time_point m_lastUpdate;   ///< Last update timestamp
    std::chrono::milliseconds m_updateInterval;           ///< Update interval
};

} // namespace fps_monitor
