#pragma once

#include <string>
#include <map>

namespace fps_monitor {

/**
 * @brief Theme loading and management
 * 
 * Parses JSON theme files and provides color values.
 * Supports theme switching without restart.
 */
class ThemeManager {
public:
    /**
     * @brief RGBA color structure
     */
    struct Color {
        float r; ///< Red (0.0-1.0)
        float g; ///< Green (0.0-1.0)
        float b; ///< Blue (0.0-1.0)
        float a; ///< Alpha (0.0-1.0)
    };

    /**
     * @brief Construct a new Theme Manager
     */
    ThemeManager();

    /**
     * @brief Destroy the Theme Manager
     */
    ~ThemeManager();

    /**
     * @brief Load a theme from file
     * 
     * @param themeName Theme name (without .json extension)
     * @return true if loaded successfully
     * @return false otherwise
     */
    bool loadTheme(const std::string& themeName);

    /**
     * @brief Get a color by name
     * 
     * @param colorName Color name (e.g., "graph_line")
     * @return Color The color value (black if not found)
     */
    Color getColor(const std::string& colorName) const;

    /**
     * @brief Get a style property
     * 
     * @param propertyName Property name (e.g., "font_size")
     * @return std::string Property value (empty if not found)
     */
    std::string getStyleProperty(const std::string& propertyName) const;

    /**
     * @brief Get current theme name
     * 
     * @return std::string Current theme name
     */
    std::string getCurrentTheme() const;

private:
    /**
     * @brief Parse hex color string (#RRGGBB or #RRGGBBAA)
     * 
     * @param hexColor Hex color string
     * @return Color Parsed color
     */
    Color parseHexColor(const std::string& hexColor) const;

    /**
     * @brief Simple JSON value parser
     * 
     * @param json JSON string
     * @param key Key to find
     * @return std::string Value (empty if not found)
     */
    std::string parseJsonValue(const std::string& json, const std::string& key) const;

    /**
     * @brief Load default theme colors
     */
    void loadDefaultTheme();

    std::map<std::string, Color> m_colors;          ///< Color map
    std::map<std::string, std::string> m_styles;    ///< Style properties
    std::string m_currentTheme;                     ///< Current theme name
};

} // namespace fps_monitor
