#include "game_detector.h"
#include <psapi.h>
#include <sstream>
#include <algorithm>

#pragma comment(lib, "psapi.lib")

namespace fps_monitor {

GameDetector::GameDetector()
    : m_gameWindow(nullptr)
    , m_autoDetect(true)
{
}

GameDetector::~GameDetector() = default;

HWND GameDetector::detectGame() {
    if (!m_autoDetect) {
        return m_gameWindow;
    }

    m_gameWindow = nullptr;
    EnumWindows(enumWindowsCallback, reinterpret_cast<LPARAM>(this));
    return m_gameWindow;
}

bool GameDetector::isGameRunning(const std::string& processName) {
    HWND game = detectGame();
    if (!game) {
        return false;
    }

    std::string currentProcess = getProcessName(game);
    return currentProcess == processName;
}

HWND GameDetector::getGameWindow() const {
    return m_gameWindow;
}

void GameDetector::setWhitelist(const std::string& whitelist) {
    m_whitelist = parseList(whitelist);
}

void GameDetector::setBlacklist(const std::string& blacklist) {
    m_blacklist = parseList(blacklist);
}

void GameDetector::setAutoDetect(bool enabled) {
    m_autoDetect = enabled;
}

bool GameDetector::isFullscreen(HWND hwnd) {
    if (!hwnd) {
        return false;
    }

    // Check if window is visible
    if (!IsWindowVisible(hwnd)) {
        return false;
    }

    // Get window rect
    RECT windowRect;
    GetWindowRect(hwnd, &windowRect);

    // Get monitor info
    HMONITOR monitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
    MONITORINFO monitorInfo = {};
    monitorInfo.cbSize = sizeof(MONITORINFO);
    GetMonitorInfo(monitor, &monitorInfo);

    // Check if window covers entire monitor
    return (windowRect.left <= monitorInfo.rcMonitor.left &&
            windowRect.top <= monitorInfo.rcMonitor.top &&
            windowRect.right >= monitorInfo.rcMonitor.right &&
            windowRect.bottom >= monitorInfo.rcMonitor.bottom);
}

std::string GameDetector::getProcessName(HWND hwnd) {
    DWORD processId = 0;
    GetWindowThreadProcessId(hwnd, &processId);

    if (processId == 0) {
        return "";
    }

    HANDLE process = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processId);
    if (!process) {
        return "";
    }

    char processName[MAX_PATH] = {0};
    HMODULE module;
    DWORD bytesNeeded;

    if (EnumProcessModules(process, &module, sizeof(module), &bytesNeeded)) {
        GetModuleBaseNameA(process, module, processName, sizeof(processName));
    }

    CloseHandle(process);
    return std::string(processName);
}

bool GameDetector::isWhitelisted(const std::string& processName) {
    if (m_whitelist.empty()) {
        return true; // No whitelist means allow all
    }

    return std::find(m_whitelist.begin(), m_whitelist.end(), processName) != m_whitelist.end();
}

bool GameDetector::isBlacklisted(const std::string& processName) {
    return std::find(m_blacklist.begin(), m_blacklist.end(), processName) != m_blacklist.end();
}

std::vector<std::string> GameDetector::parseList(const std::string& list) {
    std::vector<std::string> result;
    std::stringstream ss(list);
    std::string item;

    while (std::getline(ss, item, ',')) {
        // Trim whitespace
        item.erase(0, item.find_first_not_of(" \t"));
        item.erase(item.find_last_not_of(" \t") + 1);
        
        if (!item.empty()) {
            result.push_back(item);
        }
    }

    return result;
}

BOOL CALLBACK GameDetector::enumWindowsCallback(HWND hwnd, LPARAM lParam) {
    auto* detector = reinterpret_cast<GameDetector*>(lParam);

    // Check if fullscreen
    if (!detector->isFullscreen(hwnd)) {
        return TRUE; // Continue enumeration
    }

    // Get process name
    std::string processName = detector->getProcessName(hwnd);
    if (processName.empty()) {
        return TRUE;
    }

    // Check blacklist
    if (detector->isBlacklisted(processName)) {
        return TRUE;
    }

    // Check whitelist
    if (!detector->isWhitelisted(processName)) {
        return TRUE;
    }

    // Found a valid game
    detector->m_gameWindow = hwnd;
    return FALSE; // Stop enumeration
}

} // namespace fps_monitor
