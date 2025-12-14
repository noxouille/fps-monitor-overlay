#pragma once

#include <windows.h>
#include <shellapi.h>
#include <string>
#include <functional>

namespace fps_monitor {

/**
 * @brief System tray icon manager
 * 
 * Manages a system tray icon with context menu for application control.
 * Provides callbacks for menu actions.
 */
class TrayIcon {
public:
    /**
     * @brief Callback function type for menu actions
     */
    using MenuCallback = std::function<void()>;

    TrayIcon();
    ~TrayIcon();

    /**
     * @brief Initialize the tray icon
     * @param hwnd Parent window handle
     * @param tooltip Tooltip text to display
     * @return True if successful
     */
    bool initialize(HWND hwnd, const std::wstring& tooltip);

    /**
     * @brief Remove the tray icon
     */
    void cleanup();

    /**
     * @brief Update tooltip text
     * @param tooltip New tooltip text
     */
    void setTooltip(const std::wstring& tooltip);

    /**
     * @brief Update icon (for state indication)
     * @param hIcon New icon handle
     */
    void setIcon(HICON hIcon);

    /**
     * @brief Show balloon notification
     * @param title Notification title
     * @param message Notification message
     * @param timeout Timeout in milliseconds
     */
    void showNotification(const std::wstring& title, const std::wstring& message, DWORD timeout = 3000);

    /**
     * @brief Handle tray icon messages
     * @param msg Message ID
     * @param wParam WPARAM
     * @param lParam LPARAM
     * @return True if message was handled
     */
    bool handleMessage(UINT msg, WPARAM wParam, LPARAM lParam);

    /**
     * @brief Set callback for toggle overlay action
     */
    void setToggleCallback(MenuCallback callback);

    /**
     * @brief Set callback for settings action
     */
    void setSettingsCallback(MenuCallback callback);

    /**
     * @brief Set callback for about action
     */
    void setAboutCallback(MenuCallback callback);

    /**
     * @brief Set callback for exit action
     */
    void setExitCallback(MenuCallback callback);

    /**
     * @brief Set the overlay visibility state for menu checkmark
     * @param visible True if overlay is visible
     */
    void setOverlayVisible(bool visible);

private:
    /**
     * @brief Show context menu at cursor position
     */
    void showContextMenu();

    /**
     * @brief Create the tray icon
     */
    bool createIcon();

    HWND m_hwnd;
    NOTIFYICONDATAW m_notifyIconData;
    HICON m_hIcon;
    bool m_isInitialized;
    bool m_overlayVisible;

    // Menu item IDs
    static constexpr UINT WM_TRAYICON = WM_USER + 1;
    static constexpr UINT ID_MENU_TOGGLE = 1001;
    static constexpr UINT ID_MENU_SETTINGS = 1002;
    static constexpr UINT ID_MENU_ABOUT = 1003;
    static constexpr UINT ID_MENU_EXIT = 1004;

    // Callbacks
    MenuCallback m_toggleCallback;
    MenuCallback m_settingsCallback;
    MenuCallback m_aboutCallback;
    MenuCallback m_exitCallback;
};

} // namespace fps_monitor
