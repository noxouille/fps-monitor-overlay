#pragma once

#include <d2d1.h>
#include <vector>

namespace fps_monitor {

/**
 * @brief Live FPS graph rendering
 * 
 * Renders anti-aliased line graphs with auto-scaling and drop markers.
 * Optimized for smooth 60 FPS rendering.
 */
class GraphRenderer {
public:
    /**
     * @brief Construct a new Graph Renderer
     */
    GraphRenderer();

    /**
     * @brief Destroy the Graph Renderer
     */
    ~GraphRenderer();

    /**
     * @brief Initialize the graph renderer
     * 
     * @param renderTarget D2D render target
     * @return true if initialized successfully
     * @return false otherwise
     */
    bool initialize(ID2D1HwndRenderTarget* renderTarget);

    /**
     * @brief Render the FPS graph
     * 
     * @param samples FPS samples to render (oldest to newest)
     * @param x X position
     * @param y Y position
     * @param width Graph width
     * @param height Graph height
     */
    void render(const std::vector<double>& samples, float x, float y, float width, float height);

    /**
     * @brief Set graph colors
     * 
     * @param lineColor Line color brush
     * @param fillColor Fill color brush (optional)
     */
    void setColors(ID2D1SolidColorBrush* lineColor, ID2D1SolidColorBrush* fillColor = nullptr);

    /**
     * @brief Enable/disable grid rendering
     * 
     * @param enabled Grid enabled state
     */
    void setShowGrid(bool enabled);

    /**
     * @brief Set line width
     * 
     * @param width Line width in pixels
     */
    void setLineWidth(float width);

    /**
     * @brief Set drop marker brush
     * 
     * @param brush Brush for drop markers
     */
    void setDropMarkerBrush(ID2D1SolidColorBrush* brush);

private:
    /**
     * @brief Calculate auto-scale values
     * 
     * @param samples Sample data
     * @param minFPS Output minimum FPS
     * @param maxFPS Output maximum FPS
     */
    void calculateScale(const std::vector<double>& samples, double& minFPS, double& maxFPS);

    /**
     * @brief Render grid lines
     * 
     * @param x X position
     * @param y Y position
     * @param width Width
     * @param height Height
     * @param minFPS Minimum FPS value
     * @param maxFPS Maximum FPS value
     */
    void renderGrid(float x, float y, float width, float height, double minFPS, double maxFPS);

    ID2D1HwndRenderTarget* m_renderTarget;  ///< Render target
    ID2D1SolidColorBrush* m_lineColor;      ///< Line color
    ID2D1SolidColorBrush* m_fillColor;      ///< Fill color
    ID2D1SolidColorBrush* m_dropMarker;     ///< Drop marker color
    ID2D1SolidColorBrush* m_gridColor;      ///< Grid color
    bool m_showGrid;                        ///< Grid enabled
    float m_lineWidth;                      ///< Line width
    double m_smoothMinFPS;                  ///< Smoothed min for scale transitions
    double m_smoothMaxFPS;                  ///< Smoothed max for scale transitions
    static constexpr double SCALE_SMOOTH_FACTOR = 0.1; ///< Smoothing factor
};

} // namespace fps_monitor
