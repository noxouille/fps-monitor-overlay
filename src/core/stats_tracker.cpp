#include "stats_tracker.h"
#include <algorithm>
#include <numeric>

namespace fps_monitor {

StatsTracker::StatsTracker(int updateIntervalMs)
    : m_stats{0.0, 0.0, 0.0, 0.0, 0.0}
    , m_lastUpdate(std::chrono::steady_clock::now())
    , m_updateInterval(updateIntervalMs)
{
}

StatsTracker::~StatsTracker() = default;

void StatsTracker::update(const std::vector<double>& samples) {
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_lastUpdate);

    // Only update if interval has elapsed
    if (elapsed >= m_updateInterval) {
        calculateStats(samples);
        m_lastUpdate = now;
    }
}

const StatsTracker::Stats& StatsTracker::getStats() const {
    return m_stats;
}

double StatsTracker::get01PercentLow() const {
    return m_stats.percentile01;
}

double StatsTracker::get1PercentLow() const {
    return m_stats.percentile1;
}

double StatsTracker::getMin() const {
    return m_stats.min;
}

double StatsTracker::getMax() const {
    return m_stats.max;
}

double StatsTracker::getAverage() const {
    return m_stats.average;
}

void StatsTracker::reset() {
    m_stats = {0.0, 0.0, 0.0, 0.0, 0.0};
    m_lastUpdate = std::chrono::steady_clock::now();
}

void StatsTracker::calculateStats(const std::vector<double>& samples) {
    if (samples.empty()) {
        m_stats = {0.0, 0.0, 0.0, 0.0, 0.0};
        return;
    }

    // Sort samples for percentile calculation
    std::vector<double> sorted = samples;
    std::sort(sorted.begin(), sorted.end());

    // Calculate basic stats
    m_stats.min = sorted.front();
    m_stats.max = sorted.back();
    m_stats.average = std::accumulate(sorted.begin(), sorted.end(), 0.0) / sorted.size();

    // Calculate percentiles
    m_stats.percentile01 = calculatePercentile(sorted, 0.001);
    m_stats.percentile1 = calculatePercentile(sorted, 0.01);
}

double StatsTracker::calculatePercentile(const std::vector<double>& sortedSamples, double percentile) const {
    if (sortedSamples.empty()) {
        return 0.0;
    }

    if (sortedSamples.size() == 1) {
        return sortedSamples[0];
    }

    // Calculate the index for the percentile
    double index = percentile * (sortedSamples.size() - 1);
    size_t lowerIndex = static_cast<size_t>(std::floor(index));
    size_t upperIndex = static_cast<size_t>(std::ceil(index));

    // Clamp indices
    lowerIndex = std::min(lowerIndex, sortedSamples.size() - 1);
    upperIndex = std::min(upperIndex, sortedSamples.size() - 1);

    // Linear interpolation between the two nearest values
    if (lowerIndex == upperIndex) {
        return sortedSamples[lowerIndex];
    }

    double weight = index - lowerIndex;
    return sortedSamples[lowerIndex] * (1.0 - weight) + sortedSamples[upperIndex] * weight;
}

} // namespace fps_monitor
