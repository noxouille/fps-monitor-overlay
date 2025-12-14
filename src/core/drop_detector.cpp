#include "drop_detector.h"
#include <algorithm>

namespace fps_monitor {

DropDetector::DropDetector(double thresholdPercent)
    : m_thresholdPercent(thresholdPercent)
    , m_lastDrop(std::chrono::steady_clock::now())
{
}

DropDetector::~DropDetector() = default;

void DropDetector::update(double currentFPS, double averageFPS) {
    if (checkForDrop(currentFPS, averageFPS)) {
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration<double>(now - m_lastDrop).count();

        // Debouncing: only record drop if enough time has passed
        if (elapsed >= DEBOUNCE_SECONDS) {
            Drop drop;
            drop.timestamp = now;
            drop.averageFPS = averageFPS;
            drop.currentFPS = currentFPS;
            drop.magnitude = (averageFPS - currentFPS) / averageFPS;

            m_drops.push_back(drop);
            m_lastDrop = now;

            // Limit history size
            if (m_drops.size() > MAX_DROP_HISTORY) {
                m_drops.erase(m_drops.begin());
            }

            // Invoke callback if registered
            if (m_callback) {
                m_callback(drop);
            }
        }
    }
}

bool DropDetector::checkForDrop(double currentFPS, double averageFPS) const {
    // Need sufficient average FPS to detect drops meaningfully
    if (averageFPS < 10.0) {
        return false;
    }

    // Calculate drop percentage
    double dropPercent = ((averageFPS - currentFPS) / averageFPS) * 100.0;

    return dropPercent >= m_thresholdPercent;
}

const std::vector<DropDetector::Drop>& DropDetector::getDrops() const {
    return m_drops;
}

std::vector<DropDetector::Drop> DropDetector::getRecentDrops(double seconds) const {
    auto now = std::chrono::steady_clock::now();
    std::vector<Drop> recentDrops;

    for (const auto& drop : m_drops) {
        auto elapsed = std::chrono::duration<double>(now - drop.timestamp).count();
        if (elapsed <= seconds) {
            recentDrops.push_back(drop);
        }
    }

    return recentDrops;
}

void DropDetector::setThreshold(double thresholdPercent) {
    // Clamp to reasonable range (5.0 - 50.0)
    m_thresholdPercent = std::max(5.0, std::min(thresholdPercent, 50.0));
}

double DropDetector::getThreshold() const {
    return m_thresholdPercent;
}

void DropDetector::setDropCallback(DropCallback callback) {
    m_callback = std::move(callback);
}

void DropDetector::clearHistory() {
    m_drops.clear();
}

} // namespace fps_monitor
