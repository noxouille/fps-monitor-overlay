#include "theme_manager.h"
#include <fstream>
#include <sstream>
#include <algorithm>

namespace fps_monitor {

ThemeManager::ThemeManager()
    : m_currentTheme("default")
{
    loadDefaultTheme();
}

ThemeManager::~ThemeManager() = default;

bool ThemeManager::loadTheme(const std::string& themeName) {
    std::string filename = "resources/themes/" + themeName + ".json";
    std::ifstream file(filename);

    if (!file.is_open()) {
        loadDefaultTheme();
        return false;
    }

    // Read entire file
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string json = buffer.str();

    // Parse colors (simple parsing for hex colors)
    m_colors["background"] = parseHexColor(parseJsonValue(json, "background"));
    m_colors["graph_line"] = parseHexColor(parseJsonValue(json, "graph_line"));
    m_colors["graph_fill"] = parseHexColor(parseJsonValue(json, "graph_fill"));
    m_colors["text_primary"] = parseHexColor(parseJsonValue(json, "text_primary"));
    m_colors["text_secondary"] = parseHexColor(parseJsonValue(json, "text_secondary"));
    m_colors["text_shadow"] = parseHexColor(parseJsonValue(json, "text_shadow"));
    m_colors["drop_marker"] = parseHexColor(parseJsonValue(json, "drop_marker"));
    m_colors["grid_line"] = parseHexColor(parseJsonValue(json, "grid_line"));
    m_colors["border"] = parseHexColor(parseJsonValue(json, "border"));

    // Parse style properties
    m_styles["font_family"] = parseJsonValue(json, "font_family");
    m_styles["font_size"] = parseJsonValue(json, "font_size");
    m_styles["border_width"] = parseJsonValue(json, "border_width");
    m_styles["corner_radius"] = parseJsonValue(json, "corner_radius");
    m_styles["graph_glow"] = parseJsonValue(json, "graph_glow");

    m_currentTheme = themeName;
    return true;
}

ThemeManager::Color ThemeManager::getColor(const std::string& colorName) const {
    auto it = m_colors.find(colorName);
    if (it != m_colors.end()) {
        return it->second;
    }
    return {0.0f, 0.0f, 0.0f, 1.0f}; // Default to black
}

std::string ThemeManager::getStyleProperty(const std::string& propertyName) const {
    auto it = m_styles.find(propertyName);
    if (it != m_styles.end()) {
        return it->second;
    }
    return "";
}

std::string ThemeManager::getCurrentTheme() const {
    return m_currentTheme;
}

ThemeManager::Color ThemeManager::parseHexColor(const std::string& hexColor) const {
    if (hexColor.empty() || hexColor[0] != '#') {
        return {0.0f, 0.0f, 0.0f, 1.0f};
    }

    std::string hex = hexColor.substr(1); // Remove #
    
    // Parse RGB or RGBA
    unsigned int r = 0, g = 0, b = 0, a = 255;

    if (hex.length() >= 6) {
        r = std::stoul(hex.substr(0, 2), nullptr, 16);
        g = std::stoul(hex.substr(2, 2), nullptr, 16);
        b = std::stoul(hex.substr(4, 2), nullptr, 16);
    }

    if (hex.length() >= 8) {
        a = std::stoul(hex.substr(6, 2), nullptr, 16);
    }

    return {
        r / 255.0f,
        g / 255.0f,
        b / 255.0f,
        a / 255.0f
    };
}

std::string ThemeManager::parseJsonValue(const std::string& json, const std::string& key) const {
    // Simple JSON parser - finds "key": "value" or "key": value
    std::string searchKey = "\"" + key + "\"";
    size_t pos = json.find(searchKey);

    if (pos == std::string::npos) {
        return "";
    }

    // Find the colon
    size_t colonPos = json.find(':', pos);
    if (colonPos == std::string::npos) {
        return "";
    }

    // Skip whitespace after colon
    size_t valueStart = colonPos + 1;
    while (valueStart < json.length() && (json[valueStart] == ' ' || json[valueStart] == '\t' || json[valueStart] == '\n')) {
        valueStart++;
    }

    // Check if value is quoted
    bool quoted = (json[valueStart] == '"');
    if (quoted) {
        valueStart++;
    }

    // Find end of value
    size_t valueEnd = valueStart;
    if (quoted) {
        valueEnd = json.find('"', valueStart);
    } else {
        // Find comma or closing brace
        while (valueEnd < json.length() && json[valueEnd] != ',' && json[valueEnd] != '}' && json[valueEnd] != '\n') {
            valueEnd++;
        }
    }

    if (valueEnd == std::string::npos || valueEnd <= valueStart) {
        return "";
    }

    std::string value = json.substr(valueStart, valueEnd - valueStart);
    
    // Trim whitespace
    value.erase(0, value.find_first_not_of(" \t\n\r"));
    value.erase(value.find_last_not_of(" \t\n\r") + 1);

    return value;
}

void ThemeManager::loadDefaultTheme() {
    // Matrix Green theme (default)
    m_colors["background"] = {0.0f, 0.0f, 0.0f, 0.7f};
    m_colors["graph_line"] = {0.0f, 1.0f, 0.0f, 1.0f};
    m_colors["graph_fill"] = {0.0f, 1.0f, 0.0f, 0.2f};
    m_colors["text_primary"] = {0.0f, 1.0f, 0.0f, 1.0f};
    m_colors["text_secondary"] = {0.0f, 0.87f, 0.0f, 1.0f};
    m_colors["text_shadow"] = {0.0f, 0.0f, 0.0f, 1.0f};
    m_colors["drop_marker"] = {1.0f, 1.0f, 0.0f, 1.0f};
    m_colors["grid_line"] = {0.0f, 1.0f, 0.0f, 0.2f};
    m_colors["border"] = {0.0f, 1.0f, 0.0f, 1.0f};

    m_styles["font_family"] = "Consolas";
    m_styles["font_size"] = "14";
    m_styles["border_width"] = "0";
    m_styles["corner_radius"] = "4";
    m_styles["graph_glow"] = "true";

    m_currentTheme = "default";
}

} // namespace fps_monitor
