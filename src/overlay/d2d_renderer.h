#pragma once

#include <windows.h>
#include <d2d1.h>
#include <memory>

#pragma comment(lib, "d2d1.lib")

namespace fps_monitor {

/**
 * @brief Direct2D rendering initialization and management
 * 
 * Manages Direct2D factory, render target, and brushes.
 * Handles hardware acceleration and device lost scenarios.
 */
class D2DRenderer {
public:
    /**
     * @brief Construct a new D2D Renderer
     */
    D2DRenderer();

    /**
     * @brief Destroy the D2D Renderer
     */
    ~D2DRenderer();

    /**
     * @brief Initialize Direct2D for the given window
     * 
     * @param hwnd Window handle
     * @return true if initialized successfully
     * @return false otherwise
     */
    bool initialize(HWND hwnd);

    /**
     * @brief Shutdown and release all resources
     */
    void shutdown();

    /**
     * @brief Begin drawing frame
     * 
     * Must be called before any drawing operations.
     */
    void beginDraw();

    /**
     * @brief End drawing frame and present
     * 
     * @return true if successful
     * @return false if device lost (requires reinitialization)
     */
    bool endDraw();

    /**
     * @brief Clear the render target
     * 
     * @param r Red (0.0-1.0)
     * @param g Green (0.0-1.0)
     * @param b Blue (0.0-1.0)
     * @param a Alpha (0.0-1.0)
     */
    void clear(float r, float g, float b, float a);

    /**
     * @brief Create a solid color brush
     * 
     * @param r Red (0.0-1.0)
     * @param g Green (0.0-1.0)
     * @param b Blue (0.0-1.0)
     * @param a Alpha (0.0-1.0)
     * @return ID2D1SolidColorBrush* Brush pointer (managed by renderer)
     */
    ID2D1SolidColorBrush* createSolidBrush(float r, float g, float b, float a);

    /**
     * @brief Get the render target
     * 
     * @return ID2D1HwndRenderTarget* Render target pointer
     */
    ID2D1HwndRenderTarget* getRenderTarget() const;

    /**
     * @brief Get the D2D factory
     * 
     * @return ID2D1Factory* Factory pointer
     */
    ID2D1Factory* getFactory() const;

    /**
     * @brief Handle window resize
     * 
     * @param width New width
     * @param height New height
     * @return true if successful
     * @return false otherwise
     */
    bool resize(UINT width, UINT height);

    /**
     * @brief Check if initialized
     * 
     * @return true if ready to render
     * @return false otherwise
     */
    bool isInitialized() const;

private:
    /**
     * @brief Create render target for window
     * 
     * @param hwnd Window handle
     * @return true if successful
     * @return false otherwise
     */
    bool createRenderTarget(HWND hwnd);

    /**
     * @brief Release render target and brushes
     */
    void releaseRenderTarget();

    ID2D1Factory* m_factory;                  ///< D2D factory
    ID2D1HwndRenderTarget* m_renderTarget;    ///< Render target
    HWND m_hwnd;                              ///< Associated window
    bool m_initialized;                       ///< Initialization state
};

} // namespace fps_monitor
