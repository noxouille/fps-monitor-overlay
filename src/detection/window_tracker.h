#pragma once

#include <windows.h>
#include <functional>

namespace fps_monitor {

/**
 * @brief Game window state tracking
 * 
 * Monitors window state changes (focus, minimize, move, resize).
 * Provides event-driven notifications for overlay management.
 */
class WindowTracker {
public:
    /**
     * @brief Window event types
     */
    enum class EventType {
        Minimized,
        Restored,
        Moved,
        Resized,
        FocusLost,
        FocusGained,
        Closed
    };

    /**
     * @brief Event callback type
     */
    using EventCallback = std::function<void(EventType, HWND)>;

    /**
     * @brief Construct a new Window Tracker
     */
    WindowTracker();

    /**
     * @brief Destroy the Window Tracker
     */
    ~WindowTracker();

    /**
     * @brief Start tracking a window
     * 
     * @param hwnd Window handle to track
     * @return true if started successfully
     * @return false otherwise
     */
    bool startTracking(HWND hwnd);

    /**
     * @brief Stop tracking
     */
    void stopTracking();

    /**
     * @brief Check for window events
     * 
     * Should be called periodically.
     */
    void update();

    /**
     * @brief Set event callback
     * 
     * @param callback Callback function for events
     */
    void setEventCallback(EventCallback callback);

    /**
     * @brief Check if window is minimized
     * 
     * @return true if minimized
     * @return false otherwise
     */
    bool isMinimized() const;

    /**
     * @brief Check if window has focus
     * 
     * @return true if focused
     * @return false otherwise
     */
    bool hasFocus() const;

private:
    /**
     * @brief Check window state changes
     */
    void checkStateChanges();

    HWND m_trackedWindow;           ///< Window being tracked
    EventCallback m_callback;       ///< Event callback
    bool m_wasMinimized;            ///< Previous minimized state
    bool m_hadFocus;                ///< Previous focus state
    RECT m_lastRect;                ///< Previous window rect
    bool m_tracking;                ///< Tracking active
};

} // namespace fps_monitor
