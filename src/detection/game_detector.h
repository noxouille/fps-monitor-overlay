#pragma once

#include <windows.h>
#include <string>
#include <vector>

namespace fps_monitor {

/**
 * @brief Game window detection and attachment
 * 
 * Detects fullscreen games and borderless windowed applications.
 * Supports whitelist/blacklist filtering.
 */
class GameDetector {
public:
    /**
     * @brief Construct a new Game Detector
     */
    GameDetector();

    /**
     * @brief Destroy the Game Detector
     */
    ~GameDetector();

    /**
     * @brief Detect a fullscreen game
     * 
     * @return HWND Handle to detected game window (nullptr if none)
     */
    HWND detectGame();

    /**
     * @brief Check if a specific game is running
     * 
     * @param processName Process name to check
     * @return true if running
     * @return false otherwise
     */
    bool isGameRunning(const std::string& processName);

    /**
     * @brief Get the current game window
     * 
     * @return HWND Current game window handle
     */
    HWND getGameWindow() const;

    /**
     * @brief Set process whitelist
     * 
     * @param whitelist Comma-separated list of process names
     */
    void setWhitelist(const std::string& whitelist);

    /**
     * @brief Set process blacklist
     * 
     * @param blacklist Comma-separated list of process names
     */
    void setBlacklist(const std::string& blacklist);

    /**
     * @brief Enable/disable auto-detection
     * 
     * @param enabled Auto-detection state
     */
    void setAutoDetect(bool enabled);

private:
    /**
     * @brief Check if window is fullscreen
     * 
     * @param hwnd Window handle
     * @return true if fullscreen
     * @return false otherwise
     */
    bool isFullscreen(HWND hwnd);

    /**
     * @brief Get process name from window
     * 
     * @param hwnd Window handle
     * @return std::string Process name
     */
    std::string getProcessName(HWND hwnd);

    /**
     * @brief Check if process is in whitelist
     * 
     * @param processName Process name
     * @return true if in whitelist (or whitelist empty)
     * @return false otherwise
     */
    bool isWhitelisted(const std::string& processName);

    /**
     * @brief Check if process is in blacklist
     * 
     * @param processName Process name
     * @return true if in blacklist
     * @return false otherwise
     */
    bool isBlacklisted(const std::string& processName);

    /**
     * @brief Parse comma-separated list
     * 
     * @param list Comma-separated string
     * @return std::vector<std::string> Vector of items
     */
    std::vector<std::string> parseList(const std::string& list);

    /**
     * @brief Static enum callback for window enumeration
     * 
     * @param hwnd Window handle
     * @param lParam User data (GameDetector*)
     * @return BOOL Continue enumeration
     */
    static BOOL CALLBACK enumWindowsCallback(HWND hwnd, LPARAM lParam);

    HWND m_gameWindow;                      ///< Current game window
    bool m_autoDetect;                      ///< Auto-detection enabled
    std::vector<std::string> m_whitelist;   ///< Process whitelist
    std::vector<std::string> m_blacklist;   ///< Process blacklist
};

} // namespace fps_monitor
