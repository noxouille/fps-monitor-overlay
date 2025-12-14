#include "graph_renderer.h"
#include <algorithm>

namespace fps_monitor {

GraphRenderer::GraphRenderer()
    : m_renderTarget(nullptr)
    , m_lineColor(nullptr)
    , m_fillColor(nullptr)
    , m_dropMarker(nullptr)
    , m_gridColor(nullptr)
    , m_showGrid(false)
    , m_lineWidth(2.0f)
    , m_smoothMinFPS(0.0)
    , m_smoothMaxFPS(60.0)
{
}

GraphRenderer::~GraphRenderer() = default;

bool GraphRenderer::initialize(ID2D1HwndRenderTarget* renderTarget) {
    if (!renderTarget) {
        return false;
    }

    m_renderTarget = renderTarget;

    // Create default grid brush
    m_renderTarget->CreateSolidColorBrush(
        D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.1f),
        &m_gridColor
    );

    return true;
}

void GraphRenderer::render(const std::vector<double>& samples, float x, float y, float width, float height) {
    if (!m_renderTarget || !m_lineColor || samples.empty()) {
        return;
    }

    // Calculate scale
    double minFPS, maxFPS;
    calculateScale(samples, minFPS, maxFPS);

    // Render grid if enabled
    if (m_showGrid && m_gridColor) {
        renderGrid(x, y, width, height, minFPS, maxFPS);
    }

    // Calculate point spacing
    float xStep = width / static_cast<float>(samples.size() - 1);
    if (samples.size() == 1) {
        xStep = 0.0f;
    }

    // Render line graph
    for (size_t i = 1; i < samples.size(); ++i) {
        double fps1 = samples[i - 1];
        double fps2 = samples[i];

        // Clamp to scale
        fps1 = std::max(minFPS, std::min(fps1, maxFPS));
        fps2 = std::max(minFPS, std::min(fps2, maxFPS));

        // Convert to screen coordinates
        float x1 = x + (i - 1) * xStep;
        float x2 = x + i * xStep;
        float y1 = y + height - static_cast<float>((fps1 - minFPS) / (maxFPS - minFPS) * height);
        float y2 = y + height - static_cast<float>((fps2 - minFPS) / (maxFPS - minFPS) * height);

        // Draw line segment
        m_renderTarget->DrawLine(
            D2D1::Point2F(x1, y1),
            D2D1::Point2F(x2, y2),
            m_lineColor,
            m_lineWidth
        );
    }
}

void GraphRenderer::setColors(ID2D1SolidColorBrush* lineColor, ID2D1SolidColorBrush* fillColor) {
    m_lineColor = lineColor;
    m_fillColor = fillColor;
}

void GraphRenderer::setShowGrid(bool enabled) {
    m_showGrid = enabled;
}

void GraphRenderer::setLineWidth(float width) {
    m_lineWidth = width;
}

void GraphRenderer::setDropMarkerBrush(ID2D1SolidColorBrush* brush) {
    m_dropMarker = brush;
}

void GraphRenderer::calculateScale(const std::vector<double>& samples, double& minFPS, double& maxFPS) {
    if (samples.empty()) {
        minFPS = 0.0;
        maxFPS = 60.0;
        return;
    }

    // Find min and max
    minFPS = *std::min_element(samples.begin(), samples.end());
    maxFPS = *std::max_element(samples.begin(), samples.end());

    // Add padding (10%)
    double range = maxFPS - minFPS;
    minFPS = std::max(0.0, minFPS - range * 0.1);
    maxFPS = maxFPS + range * 0.1;

    // Ensure minimum range
    if (maxFPS - minFPS < 10.0) {
        double center = (minFPS + maxFPS) / 2.0;
        minFPS = center - 5.0;
        maxFPS = center + 5.0;
    }

    // Smooth scale transitions
    m_smoothMinFPS += (minFPS - m_smoothMinFPS) * SCALE_SMOOTH_FACTOR;
    m_smoothMaxFPS += (maxFPS - m_smoothMaxFPS) * SCALE_SMOOTH_FACTOR;

    minFPS = m_smoothMinFPS;
    maxFPS = m_smoothMaxFPS;
}

void GraphRenderer::renderGrid(float x, float y, float width, float height, double minFPS, double maxFPS) {
    if (!m_gridColor) {
        return;
    }

    // Draw horizontal grid lines (every 30 FPS or appropriate interval)
    double fpsRange = maxFPS - minFPS;
    double interval = 30.0;
    
    if (fpsRange < 60.0) {
        interval = 15.0;
    } else if (fpsRange > 120.0) {
        interval = 60.0;
    }

    for (double fps = minFPS; fps <= maxFPS; fps += interval) {
        if (fps < minFPS) continue;
        
        float lineY = y + height - static_cast<float>((fps - minFPS) / (maxFPS - minFPS) * height);
        
        m_renderTarget->DrawLine(
            D2D1::Point2F(x, lineY),
            D2D1::Point2F(x + width, lineY),
            m_gridColor,
            1.0f
        );
    }
}

} // namespace fps_monitor
