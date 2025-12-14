#include "tray_icon.h"
#include <cstring>

namespace fps_monitor {

TrayIcon::TrayIcon()
    : m_hwnd(nullptr)
    , m_hIcon(nullptr)
    , m_isInitialized(false)
    , m_overlayVisible(true)
    , m_ownsIcon(false)
{
    std::memset(&m_notifyIconData, 0, sizeof(NOTIFYICONDATAW));
}

TrayIcon::~TrayIcon() {
    cleanup();
}

bool TrayIcon::initialize(HWND hwnd, const std::wstring& tooltip) {
    if (m_isInitialized) {
        return true;
    }

    m_hwnd = hwnd;

    // Initialize NOTIFYICONDATAW structure
    m_notifyIconData.cbSize = sizeof(NOTIFYICONDATAW);
    m_notifyIconData.hWnd = m_hwnd;
    m_notifyIconData.uID = 1;
    m_notifyIconData.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    m_notifyIconData.uCallbackMessage = WM_TRAYICON;

    // Load default application icon (shared resource, don't destroy)
    m_hIcon = LoadIcon(nullptr, IDI_APPLICATION);
    m_notifyIconData.hIcon = m_hIcon;
    m_ownsIcon = false;

    // Set tooltip
    wcsncpy_s(m_notifyIconData.szTip, tooltip.c_str(), _TRUNCATE);

    // Add the icon to the system tray
    if (!Shell_NotifyIconW(NIM_ADD, &m_notifyIconData)) {
        return false;
    }

    // Set version for proper behavior on Windows 7+
    m_notifyIconData.uVersion = NOTIFYICON_VERSION_4;
    Shell_NotifyIconW(NIM_SETVERSION, &m_notifyIconData);

    m_isInitialized = true;
    return true;
}

void TrayIcon::cleanup() {
    if (m_isInitialized) {
        Shell_NotifyIconW(NIM_DELETE, &m_notifyIconData);
        m_isInitialized = false;
    }

    // Only destroy icon if we own it (custom icons)
    if (m_hIcon && m_ownsIcon) {
        DestroyIcon(m_hIcon);
    }
    m_hIcon = nullptr;
    m_ownsIcon = false;
}

void TrayIcon::setTooltip(const std::wstring& tooltip) {
    if (!m_isInitialized) {
        return;
    }

    wcsncpy_s(m_notifyIconData.szTip, tooltip.c_str(), _TRUNCATE);
    m_notifyIconData.uFlags = NIF_TIP;
    Shell_NotifyIconW(NIM_MODIFY, &m_notifyIconData);
    m_notifyIconData.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
}

void TrayIcon::setIcon(HICON hIcon) {
    if (!m_isInitialized) {
        return;
    }

    m_notifyIconData.hIcon = hIcon;
    m_notifyIconData.uFlags = NIF_ICON;
    Shell_NotifyIconW(NIM_MODIFY, &m_notifyIconData);
    m_notifyIconData.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;

    // Destroy old icon if we owned it
    if (m_hIcon && m_ownsIcon) {
        DestroyIcon(m_hIcon);
    }
    
    // Assume caller owns the new icon, so we should destroy it
    m_hIcon = hIcon;
    m_ownsIcon = true;
}

void TrayIcon::showNotification(const std::wstring& title, const std::wstring& message, DWORD timeout) {
    if (!m_isInitialized) {
        return;
    }

    wcsncpy_s(m_notifyIconData.szInfoTitle, title.c_str(), _TRUNCATE);
    wcsncpy_s(m_notifyIconData.szInfo, message.c_str(), _TRUNCATE);
    m_notifyIconData.uTimeout = timeout;
    m_notifyIconData.dwInfoFlags = NIIF_INFO;
    m_notifyIconData.uFlags |= NIF_INFO;

    Shell_NotifyIconW(NIM_MODIFY, &m_notifyIconData);

    // Reset flags after notification
    m_notifyIconData.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
}

bool TrayIcon::handleMessage(UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_TRAYICON) {
        switch (LOWORD(lParam)) {
            case WM_LBUTTONUP:
                // Left-click: Toggle overlay visibility
                if (m_toggleCallback) {
                    m_toggleCallback();
                }
                return true;

            case WM_RBUTTONUP:
                // Right-click: Show context menu
                showContextMenu();
                return true;

            case NIN_SELECT:
            case NIN_KEYSELECT:
                // Single-click or keyboard activation
                if (m_toggleCallback) {
                    m_toggleCallback();
                }
                return true;

            default:
                break;
        }
        return false;
    }

    // Handle menu commands
    if (msg == WM_COMMAND) {
        switch (LOWORD(wParam)) {
            case ID_MENU_TOGGLE:
                if (m_toggleCallback) {
                    m_toggleCallback();
                }
                return true;

            case ID_MENU_SETTINGS:
                if (m_settingsCallback) {
                    m_settingsCallback();
                }
                return true;

            case ID_MENU_ABOUT:
                if (m_aboutCallback) {
                    m_aboutCallback();
                }
                return true;

            case ID_MENU_EXIT:
                if (m_exitCallback) {
                    m_exitCallback();
                }
                return true;

            default:
                break;
        }
    }

    return false;
}

void TrayIcon::showContextMenu() {
    if (!m_isInitialized) {
        return;
    }

    // Get cursor position
    POINT cursorPos;
    GetCursorPos(&cursorPos);

    // Create popup menu
    HMENU hMenu = CreatePopupMenu();
    if (!hMenu) {
        return;
    }

    // Add menu items
    UINT flags = MF_STRING;
    if (m_overlayVisible) {
        flags |= MF_CHECKED;
    }
    AppendMenuW(hMenu, flags, ID_MENU_TOGGLE, L"Show/Hide Overlay (F12)");
    AppendMenuW(hMenu, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(hMenu, MF_STRING, ID_MENU_SETTINGS, L"Settings...");
    AppendMenuW(hMenu, MF_STRING, ID_MENU_ABOUT, L"About FPS Monitor Overlay");
    AppendMenuW(hMenu, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(hMenu, MF_STRING, ID_MENU_EXIT, L"Exit");

    // Make "Exit" bold
    MENUITEMINFOW mii = {};
    mii.cbSize = sizeof(MENUITEMINFOW);
    mii.fMask = MIIM_STATE;
    mii.fState = MFS_DEFAULT;
    SetMenuItemInfoW(hMenu, ID_MENU_EXIT, FALSE, &mii);

    // Required for proper context menu behavior
    SetForegroundWindow(m_hwnd);

    // Display the menu
    TrackPopupMenu(hMenu, TPM_RIGHTBUTTON, cursorPos.x, cursorPos.y, 0, m_hwnd, nullptr);

    // Clean up
    DestroyMenu(hMenu);

    // Post a dummy message to ensure the menu closes properly
    PostMessage(m_hwnd, WM_NULL, 0, 0);
}

void TrayIcon::setToggleCallback(MenuCallback callback) {
    m_toggleCallback = std::move(callback);
}

void TrayIcon::setSettingsCallback(MenuCallback callback) {
    m_settingsCallback = std::move(callback);
}

void TrayIcon::setAboutCallback(MenuCallback callback) {
    m_aboutCallback = std::move(callback);
}

void TrayIcon::setExitCallback(MenuCallback callback) {
    m_exitCallback = std::move(callback);
}

void TrayIcon::setOverlayVisible(bool visible) {
    m_overlayVisible = visible;
}

} // namespace fps_monitor
