#pragma once

#include <functional>
#include <vector>
#include <chrono>

namespace fps_monitor {

/**
 * @brief FPS drop detection with configurable thresholds
 * 
 * Detects when FPS drops significantly below the rolling average.
 * Provides debouncing to prevent alert spam and tracks drop history
 * for visualization.
 */
class DropDetector {
public:
    /**
     * @brief Structure representing a detected FPS drop
     */
    struct Drop {
        std::chrono::steady_clock::time_point timestamp; ///< When the drop occurred
        double averageFPS;                                ///< Average FPS before drop
        double currentFPS;                                ///< FPS during drop
        double magnitude;                                 ///< Drop percentage (0.0 - 1.0)
    };

    /**
     * @brief Callback type for drop notifications
     * 
     * @param drop The detected drop information
     */
    using DropCallback = std::function<void(const Drop&)>;

    /**
     * @brief Construct a new Drop Detector
     * 
     * @param thresholdPercent Threshold percentage for drop detection (e.g., 15.0 = 15%)
     */
    explicit DropDetector(double thresholdPercent = 15.0);

    /**
     * @brief Destroy the Drop Detector
     */
    ~DropDetector();

    /**
     * @brief Update detector with current FPS values
     * 
     * Should be called each frame with current and average FPS.
     * 
     * @param currentFPS Current instantaneous FPS
     * @param averageFPS Rolling average FPS
     */
    void update(double currentFPS, double averageFPS);

    /**
     * @brief Check if a drop is currently occurring
     * 
     * @param currentFPS Current instantaneous FPS
     * @param averageFPS Rolling average FPS
     * @return true if drop detected
     * @return false otherwise
     */
    bool checkForDrop(double currentFPS, double averageFPS) const;

    /**
     * @brief Get the history of detected drops
     * 
     * @return const std::vector<Drop>& Vector of all detected drops
     */
    const std::vector<Drop>& getDrops() const;

    /**
     * @brief Get recent drops within a time window
     * 
     * @param seconds Time window in seconds
     * @return std::vector<Drop> Drops within the specified window
     */
    std::vector<Drop> getRecentDrops(double seconds) const;

    /**
     * @brief Set the drop threshold percentage
     * 
     * @param thresholdPercent New threshold (5.0 - 50.0)
     */
    void setThreshold(double thresholdPercent);

    /**
     * @brief Get the current threshold percentage
     * 
     * @return double Current threshold
     */
    double getThreshold() const;

    /**
     * @brief Register a callback for drop events
     * 
     * @param callback Function to call when drop detected
     */
    void setDropCallback(DropCallback callback);

    /**
     * @brief Clear all drop history
     */
    void clearHistory();

private:
    double m_thresholdPercent;                        ///< Drop threshold percentage
    std::vector<Drop> m_drops;                        ///< History of detected drops
    DropCallback m_callback;                          ///< Optional drop callback
    std::chrono::steady_clock::time_point m_lastDrop; ///< Last drop timestamp for debouncing
    static constexpr double DEBOUNCE_SECONDS = 0.5;   ///< Minimum time between drops
    static constexpr size_t MAX_DROP_HISTORY = 100;   ///< Maximum drops to keep in history
};

} // namespace fps_monitor
