#pragma once

#include <string>
#include <map>
#include <mutex>

namespace fps_monitor {

/**
 * @brief Configuration file parser and manager
 * 
 * Parses INI-format configuration files with validation and default values.
 * Supports hot-reload and thread-safe access to settings.
 */
class Config {
public:
    /**
     * @brief Enumeration for overlay positions
     */
    enum class Position {
        TopLeft,
        TopRight,
        BottomLeft,
        BottomRight,
        Custom
    };

    /**
     * @brief Structure containing all display settings
     */
    struct DisplaySettings {
        Position position;
        std::string theme;
        double opacity;
        int width;
        int height;
        int customX;  ///< For Position::Custom
        int customY;  ///< For Position::Custom
    };

    /**
     * @brief Structure containing graph settings
     */
    struct GraphSettings {
        double historySeconds;
        bool showGrid;
        double lineWidth;
        bool antiAliasing;
        std::string colorMode;
    };

    /**
     * @brief Structure containing detection settings
     */
    struct DetectionSettings {
        double dropThresholdPercent;
        bool showDropMarkers;
        bool flashOnDrop;
        int flashDurationMs;
    };

    /**
     * @brief Structure containing performance settings
     */
    struct PerformanceSettings {
        int updateRateMs;
        int statsUpdateMs;
    };

    /**
     * @brief Structure containing control settings
     */
    struct ControlSettings {
        std::string toggleHotkey;
        std::string dragModifier;
    };

    /**
     * @brief Structure containing game detection settings
     */
    struct GameDetectionSettings {
        bool autoDetect;
        std::string whitelist;
        std::string blacklist;
    };

    /**
     * @brief Construct a new Config object
     * 
     * Initializes with default values.
     */
    Config();

    /**
     * @brief Destroy the Config object
     */
    ~Config();

    /**
     * @brief Load configuration from file
     * 
     * @param filename Path to config file (default: "config.ini")
     * @return true if loaded successfully
     * @return false if file not found or invalid (uses defaults)
     */
    bool load(const std::string& filename = "config.ini");

    /**
     * @brief Save current configuration to file
     * 
     * @param filename Path to config file (default: "config.ini")
     * @return true if saved successfully
     * @return false otherwise
     */
    bool save(const std::string& filename = "config.ini");

    /**
     * @brief Reload configuration from file
     * 
     * @return true if reloaded successfully
     * @return false otherwise
     */
    bool reload();

    /**
     * @brief Get display settings
     * 
     * @return const DisplaySettings& Reference to display settings
     */
    const DisplaySettings& getDisplaySettings() const;

    /**
     * @brief Get graph settings
     * 
     * @return const GraphSettings& Reference to graph settings
     */
    const GraphSettings& getGraphSettings() const;

    /**
     * @brief Get detection settings
     * 
     * @return const DetectionSettings& Reference to detection settings
     */
    const DetectionSettings& getDetectionSettings() const;

    /**
     * @brief Get performance settings
     * 
     * @return const PerformanceSettings& Reference to performance settings
     */
    const PerformanceSettings& getPerformanceSettings() const;

    /**
     * @brief Get control settings
     * 
     * @return const ControlSettings& Reference to control settings
     */
    const ControlSettings& getControlSettings() const;

    /**
     * @brief Get game detection settings
     * 
     * @return const GameDetectionSettings& Reference to game detection settings
     */
    const GameDetectionSettings& getGameDetectionSettings() const;

    /**
     * @brief Set display settings
     * 
     * @param settings New display settings
     */
    void setDisplaySettings(const DisplaySettings& settings);

    /**
     * @brief Set graph settings
     * 
     * @param settings New graph settings
     */
    void setGraphSettings(const GraphSettings& settings);

    /**
     * @brief Set detection settings
     * 
     * @param settings New detection settings
     */
    void setDetectionSettings(const DetectionSettings& settings);

private:
    /**
     * @brief Parse INI file into key-value map
     * 
     * @param filename Path to INI file
     * @param data Output map of section.key -> value
     * @return true if parsed successfully
     * @return false otherwise
     */
    bool parseIniFile(const std::string& filename, std::map<std::string, std::string>& data);

    /**
     * @brief Trim whitespace from string
     * 
     * @param str String to trim
     * @return std::string Trimmed string
     */
    std::string trim(const std::string& str) const;

    /**
     * @brief Convert position string to enum
     * 
     * @param posStr Position string (e.g., "top_right")
     * @return Position Corresponding enum value
     */
    Position stringToPosition(const std::string& posStr) const;

    /**
     * @brief Convert position enum to string
     * 
     * @param pos Position enum value
     * @return std::string Position string
     */
    std::string positionToString(Position pos) const;

    /**
     * @brief Initialize default settings
     */
    void initializeDefaults();

    DisplaySettings m_displaySettings;
    GraphSettings m_graphSettings;
    DetectionSettings m_detectionSettings;
    PerformanceSettings m_performanceSettings;
    ControlSettings m_controlSettings;
    GameDetectionSettings m_gameDetectionSettings;
    std::string m_lastFilename;
    mutable std::mutex m_mutex;
};

} // namespace fps_monitor
