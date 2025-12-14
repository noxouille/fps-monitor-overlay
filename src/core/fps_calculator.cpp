#include "fps_calculator.h"
#include <algorithm>
#include <numeric>

namespace fps_monitor {

FpsCalculator::FpsCalculator(size_t historySize)
    : m_currentFPS(0.0)
    , m_averageFPS(0.0)
    , m_historySize(std::min(historySize, MAX_HISTORY))
{
    m_samples = std::make_unique<RingBuffer<double, MAX_HISTORY>>();
    
    // Initialize high-resolution timer frequency
    QueryPerformanceFrequency(&m_frequency);
}

FpsCalculator::~FpsCalculator() = default;

void FpsCalculator::update(double deltaTime) {
    static constexpr double MAX_FPS = 1000.0;
    
    // Avoid division by zero
    if (deltaTime <= 0.0) {
        return;
    }

    // Calculate instantaneous FPS: 1.0 / deltaTime
    m_currentFPS = 1.0 / deltaTime;

    // Clamp to reasonable values (0-MAX_FPS)
    m_currentFPS = std::max(0.0, std::min(m_currentFPS, MAX_FPS));

    // Store sample in ring buffer
    m_samples->push(m_currentFPS);

    // Update rolling average
    m_averageFPS = calculateAverage();
}

double FpsCalculator::getCurrentFPS() const {
    return m_currentFPS;
}

double FpsCalculator::getAverageFPS() const {
    return m_averageFPS;
}

std::vector<double> FpsCalculator::getSamples() const {
    return m_samples->getAll();
}

size_t FpsCalculator::getSampleCount() const {
    return m_samples->size();
}

void FpsCalculator::reset() {
    m_samples->clear();
    m_currentFPS = 0.0;
    m_averageFPS = 0.0;
}

double FpsCalculator::calculateAverage() const {
    auto samples = m_samples->getAll();
    if (samples.empty()) {
        return 0.0;
    }

    // Calculate mean of all samples
    double sum = std::accumulate(samples.begin(), samples.end(), 0.0);
    return sum / samples.size();
}

} // namespace fps_monitor
