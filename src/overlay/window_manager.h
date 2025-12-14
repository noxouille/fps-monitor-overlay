#pragma once

#include <windows.h>
#include <functional>
#include <string>

namespace fps_monitor {

/**
 * @brief Transparent overlay window management
 * 
 * Creates and manages a transparent, topmost window for the overlay.
 * Handles hotkey registration, window positioning, and drag-to-reposition.
 */
class WindowManager {
public:
    /**
     * @brief Callback type for window events
     */
    using MessageCallback = std::function<LRESULT(HWND, UINT, WPARAM, LPARAM)>;

    /**
     * @brief Construct a new Window Manager
     */
    WindowManager();

    /**
     * @brief Destroy the Window Manager
     */
    ~WindowManager();

    /**
     * @brief Create the overlay window
     * 
     * @param width Window width
     * @param height Window height
     * @param x X position
     * @param y Y position
     * @return true if created successfully
     * @return false otherwise
     */
    bool create(int width, int height, int x, int y);

    /**
     * @brief Show the overlay window
     */
    void show();

    /**
     * @brief Hide the overlay window
     */
    void hide();

    /**
     * @brief Toggle visibility
     */
    void toggleVisibility();

    /**
     * @brief Set visibility state
     * 
     * @param visible True to show, false to hide
     */
    void setVisible(bool visible);

    /**
     * @brief Check if window is visible
     * 
     * @return true if visible
     * @return false if hidden
     */
    bool isVisible() const;

    /**
     * @brief Set window position
     * 
     * @param x X coordinate
     * @param y Y coordinate
     */
    void setPosition(int x, int y);

    /**
     * @brief Get window position
     * 
     * @param x Output X coordinate
     * @param y Output Y coordinate
     */
    void getPosition(int& x, int& y) const;

    /**
     * @brief Set window size
     * 
     * @param width Width
     * @param height Height
     */
    void setSize(int width, int height);

    /**
     * @brief Get window size
     * 
     * @param width Output width
     * @param height Output height
     */
    void getSize(int& width, int& height) const;

    /**
     * @brief Register a hotkey for toggling visibility
     * 
     * @param vkCode Virtual key code (e.g., VK_F12)
     * @param modifiers Modifier keys (e.g., MOD_CONTROL)
     * @return true if registered successfully
     * @return false otherwise
     */
    bool registerHotkey(UINT vkCode, UINT modifiers = 0);

    /**
     * @brief Unregister the hotkey
     */
    void unregisterHotkey();

    /**
     * @brief Process Windows messages
     * 
     * Non-blocking message processing.
     * 
     * @return true if should continue running
     * @return false if quit message received
     */
    bool processMessages();

    /**
     * @brief Get the window handle
     * 
     * @return HWND Window handle
     */
    HWND getHandle() const;

    /**
     * @brief Set a custom message callback
     * 
     * @param callback Callback function
     */
    void setMessageCallback(MessageCallback callback);

private:
    /**
     * @brief Static window procedure
     * 
     * @param hwnd Window handle
     * @param msg Message
     * @param wParam WPARAM
     * @param lParam LPARAM
     * @return LRESULT Result
     */
    static LRESULT CALLBACK windowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

    /**
     * @brief Instance window procedure
     * 
     * @param hwnd Window handle
     * @param msg Message
     * @param wParam WPARAM
     * @param lParam LPARAM
     * @return LRESULT Result
     */
    LRESULT handleMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

    /**
     * @brief Initialize window class
     * 
     * @return true if successful
     * @return false otherwise
     */
    bool initializeWindowClass();

    HWND m_hwnd;                      ///< Window handle
    HINSTANCE m_hInstance;            ///< Application instance
    bool m_visible;                   ///< Visibility state
    int m_width;                      ///< Window width
    int m_height;                     ///< Window height
    int m_x;                          ///< Window X position
    int m_y;                          ///< Window Y position
    bool m_hotkeyRegistered;          ///< Hotkey registration state
    UINT m_hotkeyId;                  ///< Hotkey ID
    MessageCallback m_messageCallback; ///< Custom message callback
    bool m_dragging;                  ///< Drag state
    POINT m_dragStart;                ///< Drag start position

    static constexpr const wchar_t* WINDOW_CLASS_NAME = L"FPSMonitorOverlay";
    static constexpr UINT HOTKEY_ID = 1;
};

} // namespace fps_monitor
