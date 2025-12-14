#include <windows.h>
#include <memory>
#include <string>

// Core modules
#include "core/config.h"
#include "core/fps_calculator.h"
#include "core/drop_detector.h"
#include "core/stats_tracker.h"

// Overlay modules
#include "overlay/window_manager.h"
#include "overlay/d2d_renderer.h"
#include "overlay/graph_renderer.h"
#include "overlay/text_renderer.h"
#include "overlay/theme_manager.h"

// Detection modules
#include "detection/game_detector.h"
#include "detection/window_tracker.h"

// Utils modules
#include "utils/timer.h"
#include "utils/logger.h"
#include "utils/tray_icon.h"

using namespace fps_monitor;

/**
 * @brief Main application class
 */
class FPSMonitorApp {
public:
    FPSMonitorApp() : m_running(false), m_visible(true) {}

    bool initialize() {
        // 1. Load configuration
        LOG_INFO("Loading configuration...");
        m_config = std::make_unique<Config>();
        if (!m_config->load("config.ini")) {
            LOG_WARNING("Failed to load config.ini, using defaults");
        }

        // 2. Initialize logger
        LOG_INFO("FPS Monitor Overlay starting...");

        // 3. Initialize theme manager
        m_themeManager = std::make_unique<ThemeManager>();
        const auto& displaySettings = m_config->getDisplaySettings();
        if (!m_themeManager->loadTheme(displaySettings.theme)) {
            LOG_WARNING("Failed to load theme, using default");
        }

        // 4. Create high-resolution timer
        m_timer = std::make_unique<Timer>();

        // 5. Initialize FPS calculator
        const auto& graphSettings = m_config->getGraphSettings();
        size_t historySize = static_cast<size_t>(graphSettings.historySeconds * 60.0);
        m_fpsCalculator = std::make_unique<FpsCalculator>(historySize);

        // 6. Initialize stats tracker
        const auto& perfSettings = m_config->getPerformanceSettings();
        m_statsTracker = std::make_unique<StatsTracker>(perfSettings.statsUpdateMs);

        // 7. Initialize drop detector
        const auto& detectionSettings = m_config->getDetectionSettings();
        m_dropDetector = std::make_unique<DropDetector>(detectionSettings.dropThresholdPercent);

        // Set drop callback for logging
        m_dropDetector->setDropCallback([](const DropDetector::Drop& drop) {
            LOG_WARNING("FPS drop detected: " + std::to_string(drop.magnitude * 100.0) + "%");
        });

        // 8. Initialize game detector (optional for Phase 1)
        m_gameDetector = std::make_unique<GameDetector>();
        const auto& gameSettings = m_config->getGameDetectionSettings();
        m_gameDetector->setAutoDetect(gameSettings.autoDetect);
        m_gameDetector->setWhitelist(gameSettings.whitelist);
        m_gameDetector->setBlacklist(gameSettings.blacklist);

        // 9. Create overlay window
        m_windowManager = std::make_unique<WindowManager>();
        
        // Calculate position based on config
        int x = 0, y = 0;
        calculateWindowPosition(x, y);

        if (!m_windowManager->create(displaySettings.width, displaySettings.height, x, y)) {
            MessageBoxA(nullptr, "Failed to create overlay window", "Error", MB_OK | MB_ICONERROR);
            return false;
        }

        // 10. Initialize Direct2D renderer
        m_d2dRenderer = std::make_unique<D2DRenderer>();
        if (!m_d2dRenderer->initialize(m_windowManager->getHandle())) {
            MessageBoxA(nullptr, "Failed to initialize Direct2D", "Error", MB_OK | MB_ICONERROR);
            return false;
        }

        // 11. Initialize graph renderer
        m_graphRenderer = std::make_unique<GraphRenderer>();
        if (!m_graphRenderer->initialize(m_d2dRenderer->getRenderTarget())) {
            MessageBoxA(nullptr, "Failed to initialize graph renderer", "Error", MB_OK | MB_ICONERROR);
            return false;
        }

        // Configure graph renderer
        m_graphRenderer->setShowGrid(graphSettings.showGrid);
        m_graphRenderer->setLineWidth(static_cast<float>(graphSettings.lineWidth));

        // 12. Initialize text renderer
        std::wstring fontFamily = L"Consolas";
        std::string fontFamilyStr = m_themeManager->getStyleProperty("font_family");
        if (!fontFamilyStr.empty()) {
            fontFamily = std::wstring(fontFamilyStr.begin(), fontFamilyStr.end());
        }

        float fontSize = 14.0f;
        std::string fontSizeStr = m_themeManager->getStyleProperty("font_size");
        if (!fontSizeStr.empty()) {
            try {
                fontSize = std::stof(fontSizeStr);
            } catch (const std::exception&) {
                // Keep default on parse error
            }
        }

        m_textRenderer = std::make_unique<TextRenderer>();
        if (!m_textRenderer->initialize(m_d2dRenderer->getRenderTarget(), fontFamily, fontSize)) {
            MessageBoxA(nullptr, "Failed to initialize text renderer", "Error", MB_OK | MB_ICONERROR);
            return false;
        }

        // 13. Register hotkey (F12 by default)
        if (!m_windowManager->registerHotkey(VK_F12)) {
            LOG_WARNING("Failed to register F12 hotkey");
        }

        // 14. Initialize system tray icon
        m_trayIcon = std::make_unique<TrayIcon>();
        if (!m_trayIcon->initialize(m_windowManager->getHandle(), L"FPS Monitor Overlay - Running")) {
            LOG_WARNING("Failed to create tray icon");
        } else {
            // Set up tray icon callbacks
            m_trayIcon->setToggleCallback([this]() {
                m_visible = !m_visible;
                m_windowManager->setVisible(m_visible);
                m_trayIcon->setTooltip(m_visible ? L"FPS Monitor Overlay - Running" : L"FPS Monitor Overlay - Hidden");
                m_trayIcon->setOverlayVisible(m_visible);
            });

            m_trayIcon->setSettingsCallback([this]() {
                MessageBoxW(m_windowManager->getHandle(),
                    L"Settings GUI coming in Phase 2!\n\nFor now, edit config.ini manually.",
                    L"Settings",
                    MB_OK | MB_ICONINFORMATION);
            });

            m_trayIcon->setAboutCallback([this]() {
                MessageBoxW(m_windowManager->getHandle(),
                    L"FPS Monitor Overlay v1.0\n\n"
                    L"High-performance FPS monitoring with live graph visualization.\n\n"
                    L"Features:\n"
                    L"• Real-time FPS counter\n"
                    L"• Live scrolling graph\n"
                    L"• Drop detection\n"
                    L"• Customizable themes\n\n"
                    L"Licensed under MIT\n"
                    L"https://github.com/noxouille/fps-monitor-overlay",
                    L"About FPS Monitor Overlay",
                    MB_OK | MB_ICONINFORMATION);
            });

            m_trayIcon->setExitCallback([this]() {
                // Ask for confirmation
                int result = MessageBoxW(m_windowManager->getHandle(),
                    L"Are you sure you want to exit FPS Monitor Overlay?",
                    L"Confirm Exit",
                    MB_YESNO | MB_ICONQUESTION);
                if (result == IDYES) {
                    m_running = false;
                    PostQuitMessage(0);
                }
            });

            // Show notification on startup
            m_trayIcon->showNotification(
                L"FPS Monitor Overlay",
                L"FPS Monitor is running in system tray. Press F12 to toggle visibility.",
                3000
            );

            // Set up message callback for tray icon messages (after tray icon is initialized)
            m_windowManager->setMessageCallback([this](HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) -> LRESULT {
                // Handle tray icon messages
                if (m_trayIcon && m_trayIcon->handleMessage(msg, wParam, lParam)) {
                    return 0;
                }
                // Return non-zero to indicate message not handled
                return 1;
            });
        }

        // Create brushes for rendering
        createBrushes();

        // Show window
        m_windowManager->show();

        LOG_INFO("Initialization complete");
        return true;
    }

    void run() {
        m_running = true;
        m_timer->start();

        const auto& perfSettings = m_config->getPerformanceSettings();
        const double targetFrameTime = perfSettings.updateRateMs / 1000.0;

        LOG_INFO("Entering main loop...");

        while (m_running) {
            // Process Windows messages
            if (!m_windowManager->processMessages()) {
                m_running = false;
                break;
            }

            // Update timer and calculate delta time
            double deltaTime = m_timer->getDeltaTime();

            // Update FPS calculator
            m_fpsCalculator->update(deltaTime);

            // Update stats tracker
            auto samples = m_fpsCalculator->getSamples();
            m_statsTracker->update(samples);

            // Check for drops
            m_dropDetector->update(m_fpsCalculator->getCurrentFPS(), m_fpsCalculator->getAverageFPS());

            // Render overlay if visible
            if (m_windowManager->isVisible()) {
                render();
            }

            // Sleep to maintain target FPS (60 FPS)
            Sleep(static_cast<DWORD>(targetFrameTime * 1000));
        }

        LOG_INFO("Main loop exited");
    }

    void shutdown() {
        LOG_INFO("Shutting down...");

        // Clean up in reverse order
        m_trayIcon.reset();
        m_textRenderer.reset();
        m_graphRenderer.reset();
        m_d2dRenderer.reset();
        m_windowManager.reset();
        m_gameDetector.reset();
        m_dropDetector.reset();
        m_statsTracker.reset();
        m_fpsCalculator.reset();
        m_timer.reset();
        m_themeManager.reset();
        m_config.reset();

        // Release brushes
        releaseBrushes();

        LOG_INFO("Shutdown complete");
    }

private:
    void calculateWindowPosition(int& x, int& y) {
        const auto& displaySettings = m_config->getDisplaySettings();
        
        // Get primary monitor dimensions
        int screenWidth = GetSystemMetrics(SM_CXSCREEN);
        int screenHeight = GetSystemMetrics(SM_CYSCREEN);

        const int margin = 20;

        switch (displaySettings.position) {
            case Config::Position::TopLeft:
                x = margin;
                y = margin;
                break;
            case Config::Position::TopRight:
                x = screenWidth - displaySettings.width - margin;
                y = margin;
                break;
            case Config::Position::BottomLeft:
                x = margin;
                y = screenHeight - displaySettings.height - margin;
                break;
            case Config::Position::BottomRight:
                x = screenWidth - displaySettings.width - margin;
                y = screenHeight - displaySettings.height - margin;
                break;
            case Config::Position::Custom:
                x = displaySettings.customX;
                y = displaySettings.customY;
                break;
            default:
                x = screenWidth - displaySettings.width - margin;
                y = margin;
                break;
        }
    }

    void createBrushes() {
        auto bg = m_themeManager->getColor("background");
        auto line = m_themeManager->getColor("graph_line");
        auto textPrimary = m_themeManager->getColor("text_primary");
        auto textSecondary = m_themeManager->getColor("text_secondary");

        m_bgBrush = m_d2dRenderer->createSolidBrush(bg.r, bg.g, bg.b, bg.a);
        m_lineBrush = m_d2dRenderer->createSolidBrush(line.r, line.g, line.b, line.a);
        m_textBrush = m_d2dRenderer->createSolidBrush(textPrimary.r, textPrimary.g, textPrimary.b, textPrimary.a);
        m_textSecondaryBrush = m_d2dRenderer->createSolidBrush(textSecondary.r, textSecondary.g, textSecondary.b, textSecondary.a);
    }

    void releaseBrushes() {
        if (m_bgBrush) m_bgBrush->Release();
        if (m_lineBrush) m_lineBrush->Release();
        if (m_textBrush) m_textBrush->Release();
        if (m_textSecondaryBrush) m_textSecondaryBrush->Release();
    }

    void render() {
        if (!m_d2dRenderer || !m_d2dRenderer->isInitialized()) {
            return;
        }

        const auto& displaySettings = m_config->getDisplaySettings();

        m_d2dRenderer->beginDraw();

        // Clear background
        auto bg = m_themeManager->getColor("background");
        m_d2dRenderer->clear(bg.r, bg.g, bg.b, bg.a);

        // Render graph
        auto samples = m_fpsCalculator->getSamples();
        if (!samples.empty()) {
            m_graphRenderer->setColors(m_lineBrush, nullptr);
            m_graphRenderer->render(samples, 10.0f, 50.0f, 
                                   static_cast<float>(displaySettings.width) - 20.0f, 80.0f);
        }

        // Render FPS text
        double currentFPS = m_fpsCalculator->getCurrentFPS();
        m_textRenderer->renderFPS(currentFPS, 10.0f, 5.0f, m_textBrush);

        // Render stats
        const auto& stats = m_statsTracker->getStats();
        float statsY = 140.0f;
        m_textRenderer->renderStat(L"AVG:", stats.average, 10.0f, statsY, m_textSecondaryBrush);
        m_textRenderer->renderStat(L"MIN:", stats.min, 80.0f, statsY, m_textSecondaryBrush);
        m_textRenderer->renderStat(L"MAX:", stats.max, 150.0f, statsY, m_textSecondaryBrush);

        // End drawing
        if (!m_d2dRenderer->endDraw()) {
            LOG_ERROR("Direct2D device lost, attempting recovery...");
            // Device lost - would need to recreate resources
        }
    }

    // Core components
    std::unique_ptr<Config> m_config;
    std::unique_ptr<FpsCalculator> m_fpsCalculator;
    std::unique_ptr<DropDetector> m_dropDetector;
    std::unique_ptr<StatsTracker> m_statsTracker;
    std::unique_ptr<Timer> m_timer;

    // Overlay components
    std::unique_ptr<WindowManager> m_windowManager;
    std::unique_ptr<D2DRenderer> m_d2dRenderer;
    std::unique_ptr<GraphRenderer> m_graphRenderer;
    std::unique_ptr<TextRenderer> m_textRenderer;
    std::unique_ptr<ThemeManager> m_themeManager;

    // Detection components
    std::unique_ptr<GameDetector> m_gameDetector;
    std::unique_ptr<WindowTracker> m_windowTracker;

    // Tray icon
    std::unique_ptr<TrayIcon> m_trayIcon;

    // Brushes
    ID2D1SolidColorBrush* m_bgBrush = nullptr;
    ID2D1SolidColorBrush* m_lineBrush = nullptr;
    ID2D1SolidColorBrush* m_textBrush = nullptr;
    ID2D1SolidColorBrush* m_textSecondaryBrush = nullptr;

    // State
    bool m_running;
    bool m_visible;
};

/**
 * @brief WinMain entry point
 */
int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, 
                   _In_ LPSTR lpCmdLine, _In_ int nShowCmd) {
    // Suppress unused parameter warnings
    (void)hInstance;
    (void)hPrevInstance;
    (void)lpCmdLine;
    (void)nShowCmd;

    // Initialize logger
    Logger::getInstance().initialize("fps_monitor.log");
    LOG_INFO("=== FPS Monitor Overlay v1.0.0 ===");

    // Create and run application
    FPSMonitorApp app;

    if (!app.initialize()) {
        MessageBoxA(nullptr, "Failed to initialize application", "Error", MB_OK | MB_ICONERROR);
        Logger::getInstance().shutdown();
        return 1;
    }

    app.run();
    app.shutdown();

    Logger::getInstance().shutdown();
    return 0;
}
