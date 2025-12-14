#include "window_tracker.h"

namespace fps_monitor {

WindowTracker::WindowTracker()
    : m_trackedWindow(nullptr)
    , m_wasMinimized(false)
    , m_hadFocus(false)
    , m_tracking(false)
{
    m_lastRect = {0, 0, 0, 0};
}

WindowTracker::~WindowTracker() {
    stopTracking();
}

bool WindowTracker::startTracking(HWND hwnd) {
    if (!hwnd || !IsWindow(hwnd)) {
        return false;
    }

    m_trackedWindow = hwnd;
    m_tracking = true;

    // Initialize state
    m_wasMinimized = IsIconic(hwnd);
    m_hadFocus = (GetForegroundWindow() == hwnd);
    GetWindowRect(hwnd, &m_lastRect);

    return true;
}

void WindowTracker::stopTracking() {
    m_tracking = false;
    m_trackedWindow = nullptr;
}

void WindowTracker::update() {
    if (!m_tracking || !m_trackedWindow) {
        return;
    }

    // Check if window still exists
    if (!IsWindow(m_trackedWindow)) {
        if (m_callback) {
            m_callback(EventType::Closed, m_trackedWindow);
        }
        stopTracking();
        return;
    }

    checkStateChanges();
}

void WindowTracker::setEventCallback(EventCallback callback) {
    m_callback = std::move(callback);
}

bool WindowTracker::isMinimized() const {
    if (!m_trackedWindow) {
        return false;
    }
    return IsIconic(m_trackedWindow);
}

bool WindowTracker::hasFocus() const {
    if (!m_trackedWindow) {
        return false;
    }
    return GetForegroundWindow() == m_trackedWindow;
}

void WindowTracker::checkStateChanges() {
    if (!m_trackedWindow) {
        return;
    }

    // Check minimized state
    bool nowMinimized = IsIconic(m_trackedWindow);
    if (nowMinimized != m_wasMinimized) {
        if (m_callback) {
            m_callback(nowMinimized ? EventType::Minimized : EventType::Restored, m_trackedWindow);
        }
        m_wasMinimized = nowMinimized;
    }

    // Check focus state
    bool nowHasFocus = (GetForegroundWindow() == m_trackedWindow);
    if (nowHasFocus != m_hadFocus) {
        if (m_callback) {
            m_callback(nowHasFocus ? EventType::FocusGained : EventType::FocusLost, m_trackedWindow);
        }
        m_hadFocus = nowHasFocus;
    }

    // Check window position/size
    RECT currentRect;
    GetWindowRect(m_trackedWindow, &currentRect);

    bool moved = (currentRect.left != m_lastRect.left || currentRect.top != m_lastRect.top);
    bool resized = (currentRect.right - currentRect.left != m_lastRect.right - m_lastRect.left ||
                    currentRect.bottom - currentRect.top != m_lastRect.bottom - m_lastRect.top);

    if (moved && m_callback) {
        m_callback(EventType::Moved, m_trackedWindow);
    }

    if (resized && m_callback) {
        m_callback(EventType::Resized, m_trackedWindow);
    }

    m_lastRect = currentRect;
}

} // namespace fps_monitor
