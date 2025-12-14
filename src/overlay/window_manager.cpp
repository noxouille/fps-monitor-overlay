#include "window_manager.h"

namespace fps_monitor {

WindowManager::WindowManager()
    : m_hwnd(nullptr)
    , m_hInstance(GetModuleHandle(nullptr))
    , m_visible(false)
    , m_width(280)
    , m_height(160)
    , m_x(0)
    , m_y(0)
    , m_hotkeyRegistered(false)
    , m_hotkeyId(HOTKEY_ID)
    , m_dragging(false)
{
    m_dragStart = {0, 0};
}

WindowManager::~WindowManager() {
    unregisterHotkey();
    if (m_hwnd) {
        DestroyWindow(m_hwnd);
    }
}

bool WindowManager::create(int width, int height, int x, int y) {
    m_width = width;
    m_height = height;
    m_x = x;
    m_y = y;

    if (!initializeWindowClass()) {
        return false;
    }

    // Create layered, topmost, transparent window
    m_hwnd = CreateWindowExW(
        WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW,
        WINDOW_CLASS_NAME,
        L"FPS Monitor Overlay",
        WS_POPUP,
        m_x, m_y, m_width, m_height,
        nullptr,
        nullptr,
        m_hInstance,
        this
    );

    if (!m_hwnd) {
        return false;
    }

    // Enable per-pixel alpha blending
    SetLayeredWindowAttributes(m_hwnd, 0, 255, LWA_ALPHA);

    return true;
}

void WindowManager::show() {
    if (m_hwnd) {
        ShowWindow(m_hwnd, SW_SHOW);
        UpdateWindow(m_hwnd);
        m_visible = true;
    }
}

void WindowManager::hide() {
    if (m_hwnd) {
        ShowWindow(m_hwnd, SW_HIDE);
        m_visible = false;
    }
}

void WindowManager::toggleVisibility() {
    if (m_visible) {
        hide();
    } else {
        show();
    }
}

bool WindowManager::isVisible() const {
    return m_visible;
}

void WindowManager::setPosition(int x, int y) {
    m_x = x;
    m_y = y;
    if (m_hwnd) {
        SetWindowPos(m_hwnd, HWND_TOPMOST, m_x, m_y, 0, 0, SWP_NOSIZE | SWP_NOACTIVATE);
    }
}

void WindowManager::getPosition(int& x, int& y) const {
    x = m_x;
    y = m_y;
}

void WindowManager::setSize(int width, int height) {
    m_width = width;
    m_height = height;
    if (m_hwnd) {
        SetWindowPos(m_hwnd, nullptr, 0, 0, m_width, m_height, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
    }
}

void WindowManager::getSize(int& width, int& height) const {
    width = m_width;
    height = m_height;
}

bool WindowManager::registerHotkey(UINT vkCode, UINT modifiers) {
    if (m_hotkeyRegistered) {
        unregisterHotkey();
    }

    if (RegisterHotKey(m_hwnd, m_hotkeyId, modifiers, vkCode)) {
        m_hotkeyRegistered = true;
        return true;
    }

    return false;
}

void WindowManager::unregisterHotkey() {
    if (m_hotkeyRegistered && m_hwnd) {
        UnregisterHotKey(m_hwnd, m_hotkeyId);
        m_hotkeyRegistered = false;
    }
}

bool WindowManager::processMessages() {
    MSG msg;
    while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
        if (msg.message == WM_QUIT) {
            return false;
        }

        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return true;
}

HWND WindowManager::getHandle() const {
    return m_hwnd;
}

void WindowManager::setMessageCallback(MessageCallback callback) {
    m_messageCallback = std::move(callback);
}

bool WindowManager::initializeWindowClass() {
    WNDCLASSEXW wc = {};
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WindowManager::windowProc;
    wc.hInstance = m_hInstance;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.lpszClassName = WINDOW_CLASS_NAME;

    if (!RegisterClassExW(&wc)) {
        DWORD error = GetLastError();
        if (error != ERROR_CLASS_ALREADY_EXISTS) {
            return false;
        }
    }

    return true;
}

LRESULT CALLBACK WindowManager::windowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    WindowManager* manager = nullptr;

    if (msg == WM_CREATE) {
        CREATESTRUCT* cs = reinterpret_cast<CREATESTRUCT*>(lParam);
        manager = reinterpret_cast<WindowManager*>(cs->lpCreateParams);
        SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(manager));
    } else {
        manager = reinterpret_cast<WindowManager*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
    }

    if (manager) {
        return manager->handleMessage(hwnd, msg, wParam, lParam);
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

LRESULT WindowManager::handleMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    // Call custom callback if registered
    if (m_messageCallback) {
        LRESULT result = m_messageCallback(hwnd, msg, wParam, lParam);
        if (result != 0) {
            return result;
        }
    }

    switch (msg) {
        case WM_HOTKEY:
            if (wParam == m_hotkeyId) {
                toggleVisibility();
                return 0;
            }
            break;

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;

        case WM_CLOSE:
            DestroyWindow(hwnd);
            return 0;

        default:
            break;
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

} // namespace fps_monitor
