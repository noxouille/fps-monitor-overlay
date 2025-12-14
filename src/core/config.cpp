#include "config.h"
#include <fstream>
#include <sstream>
#include <algorithm>

namespace fps_monitor {

Config::Config() {
    initializeDefaults();
}

Config::~Config() = default;

void Config::initializeDefaults() {
    // Display defaults
    m_displaySettings.position = Position::TopRight;
    m_displaySettings.theme = "matrix_green";
    m_displaySettings.opacity = 0.7;
    m_displaySettings.width = 280;
    m_displaySettings.height = 160;
    m_displaySettings.customX = 0;
    m_displaySettings.customY = 0;

    // Graph defaults
    m_graphSettings.historySeconds = 2.0;
    m_graphSettings.showGrid = false;
    m_graphSettings.lineWidth = 2.0;
    m_graphSettings.antiAliasing = true;
    m_graphSettings.colorMode = "solid";

    // Detection defaults
    m_detectionSettings.dropThresholdPercent = 15.0;
    m_detectionSettings.showDropMarkers = true;
    m_detectionSettings.flashOnDrop = true;
    m_detectionSettings.flashDurationMs = 200;

    // Performance defaults
    m_performanceSettings.updateRateMs = 16;  // 60 FPS
    m_performanceSettings.statsUpdateMs = 500;

    // Control defaults
    m_controlSettings.toggleHotkey = "VK_F12";
    m_controlSettings.dragModifier = "CTRL+SHIFT";

    // Game detection defaults
    m_gameDetectionSettings.autoDetect = true;
    m_gameDetectionSettings.whitelist = "";
    m_gameDetectionSettings.blacklist = "explorer.exe, taskmgr.exe";
}

bool Config::load(const std::string& filename) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    m_lastFilename = filename;
    std::map<std::string, std::string> data;

    if (!parseIniFile(filename, data)) {
        // File not found or invalid, use defaults
        return false;
    }

    // Parse Display settings
    if (data.count("Display.position")) {
        m_displaySettings.position = stringToPosition(data["Display.position"]);
    }
    if (data.count("Display.theme")) {
        m_displaySettings.theme = data["Display.theme"];
    }
    if (data.count("Display.opacity")) {
        m_displaySettings.opacity = std::stod(data["Display.opacity"]);
    }
    if (data.count("Display.width")) {
        m_displaySettings.width = std::stoi(data["Display.width"]);
    }
    if (data.count("Display.height")) {
        m_displaySettings.height = std::stoi(data["Display.height"]);
    }

    // Parse Graph settings
    if (data.count("Graph.history_seconds")) {
        m_graphSettings.historySeconds = std::stod(data["Graph.history_seconds"]);
    }
    if (data.count("Graph.show_grid")) {
        m_graphSettings.showGrid = (data["Graph.show_grid"] == "true");
    }
    if (data.count("Graph.line_width")) {
        m_graphSettings.lineWidth = std::stod(data["Graph.line_width"]);
    }
    if (data.count("Graph.anti_aliasing")) {
        m_graphSettings.antiAliasing = (data["Graph.anti_aliasing"] == "true");
    }
    if (data.count("Graph.color_mode")) {
        m_graphSettings.colorMode = data["Graph.color_mode"];
    }

    // Parse Detection settings
    if (data.count("Detection.drop_threshold_percent")) {
        m_detectionSettings.dropThresholdPercent = std::stod(data["Detection.drop_threshold_percent"]);
    }
    if (data.count("Detection.show_drop_markers")) {
        m_detectionSettings.showDropMarkers = (data["Detection.show_drop_markers"] == "true");
    }
    if (data.count("Detection.flash_on_drop")) {
        m_detectionSettings.flashOnDrop = (data["Detection.flash_on_drop"] == "true");
    }
    if (data.count("Detection.flash_duration_ms")) {
        m_detectionSettings.flashDurationMs = std::stoi(data["Detection.flash_duration_ms"]);
    }

    // Parse Performance settings
    if (data.count("Performance.update_rate_ms")) {
        m_performanceSettings.updateRateMs = std::stoi(data["Performance.update_rate_ms"]);
    }
    if (data.count("Performance.stats_update_ms")) {
        m_performanceSettings.statsUpdateMs = std::stoi(data["Performance.stats_update_ms"]);
    }

    // Parse Control settings
    if (data.count("Controls.toggle_hotkey")) {
        m_controlSettings.toggleHotkey = data["Controls.toggle_hotkey"];
    }
    if (data.count("Controls.drag_modifier")) {
        m_controlSettings.dragModifier = data["Controls.drag_modifier"];
    }

    // Parse GameDetection settings
    if (data.count("GameDetection.auto_detect")) {
        m_gameDetectionSettings.autoDetect = (data["GameDetection.auto_detect"] == "true");
    }
    if (data.count("GameDetection.whitelist")) {
        m_gameDetectionSettings.whitelist = data["GameDetection.whitelist"];
    }
    if (data.count("GameDetection.blacklist")) {
        m_gameDetectionSettings.blacklist = data["GameDetection.blacklist"];
    }

    return true;
}

bool Config::save(const std::string& filename) {
    std::lock_guard<std::mutex> lock(m_mutex);

    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }

    // Write Display section
    file << "[Display]\n";
    file << "position = " << positionToString(m_displaySettings.position) << "\n";
    file << "theme = " << m_displaySettings.theme << "\n";
    file << "opacity = " << m_displaySettings.opacity << "\n";
    file << "width = " << m_displaySettings.width << "\n";
    file << "height = " << m_displaySettings.height << "\n";
    file << "\n";

    // Write Graph section
    file << "[Graph]\n";
    file << "history_seconds = " << m_graphSettings.historySeconds << "\n";
    file << "show_grid = " << (m_graphSettings.showGrid ? "true" : "false") << "\n";
    file << "line_width = " << m_graphSettings.lineWidth << "\n";
    file << "anti_aliasing = " << (m_graphSettings.antiAliasing ? "true" : "false") << "\n";
    file << "color_mode = " << m_graphSettings.colorMode << "\n";
    file << "\n";

    // Write Detection section
    file << "[Detection]\n";
    file << "drop_threshold_percent = " << m_detectionSettings.dropThresholdPercent << "\n";
    file << "show_drop_markers = " << (m_detectionSettings.showDropMarkers ? "true" : "false") << "\n";
    file << "flash_on_drop = " << (m_detectionSettings.flashOnDrop ? "true" : "false") << "\n";
    file << "flash_duration_ms = " << m_detectionSettings.flashDurationMs << "\n";
    file << "\n";

    // Write Performance section
    file << "[Performance]\n";
    file << "update_rate_ms = " << m_performanceSettings.updateRateMs << "\n";
    file << "stats_update_ms = " << m_performanceSettings.statsUpdateMs << "\n";
    file << "\n";

    // Write Controls section
    file << "[Controls]\n";
    file << "toggle_hotkey = " << m_controlSettings.toggleHotkey << "\n";
    file << "drag_modifier = " << m_controlSettings.dragModifier << "\n";
    file << "\n";

    // Write GameDetection section
    file << "[GameDetection]\n";
    file << "auto_detect = " << (m_gameDetectionSettings.autoDetect ? "true" : "false") << "\n";
    file << "whitelist = " << m_gameDetectionSettings.whitelist << "\n";
    file << "blacklist = " << m_gameDetectionSettings.blacklist << "\n";

    return true;
}

bool Config::reload() {
    return load(m_lastFilename);
}

bool Config::parseIniFile(const std::string& filename, std::map<std::string, std::string>& data) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }

    std::string line;
    std::string currentSection;

    while (std::getline(file, line)) {
        line = trim(line);

        // Skip empty lines and comments
        if (line.empty() || line[0] == '#' || line[0] == ';') {
            continue;
        }

        // Check for section header
        if (line[0] == '[' && line.back() == ']') {
            currentSection = line.substr(1, line.length() - 2);
            continue;
        }

        // Parse key-value pair
        size_t eqPos = line.find('=');
        if (eqPos != std::string::npos) {
            std::string key = trim(line.substr(0, eqPos));
            std::string value = trim(line.substr(eqPos + 1));
            
            // Store as section.key
            if (!currentSection.empty()) {
                data[currentSection + "." + key] = value;
            }
        }
    }

    return true;
}

std::string Config::trim(const std::string& str) const {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) {
        return "";
    }
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, last - first + 1);
}

Config::Position Config::stringToPosition(const std::string& posStr) const {
    if (posStr == "top_left") return Position::TopLeft;
    if (posStr == "top_right") return Position::TopRight;
    if (posStr == "bottom_left") return Position::BottomLeft;
    if (posStr == "bottom_right") return Position::BottomRight;
    if (posStr == "custom") return Position::Custom;
    return Position::TopRight; // Default
}

std::string Config::positionToString(Position pos) const {
    switch (pos) {
        case Position::TopLeft: return "top_left";
        case Position::TopRight: return "top_right";
        case Position::BottomLeft: return "bottom_left";
        case Position::BottomRight: return "bottom_right";
        case Position::Custom: return "custom";
        default: return "top_right";
    }
}

const Config::DisplaySettings& Config::getDisplaySettings() const {
    return m_displaySettings;
}

const Config::GraphSettings& Config::getGraphSettings() const {
    return m_graphSettings;
}

const Config::DetectionSettings& Config::getDetectionSettings() const {
    return m_detectionSettings;
}

const Config::PerformanceSettings& Config::getPerformanceSettings() const {
    return m_performanceSettings;
}

const Config::ControlSettings& Config::getControlSettings() const {
    return m_controlSettings;
}

const Config::GameDetectionSettings& Config::getGameDetectionSettings() const {
    return m_gameDetectionSettings;
}

void Config::setDisplaySettings(const DisplaySettings& settings) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_displaySettings = settings;
}

void Config::setGraphSettings(const GraphSettings& settings) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_graphSettings = settings;
}

void Config::setDetectionSettings(const DetectionSettings& settings) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_detectionSettings = settings;
}

} // namespace fps_monitor
