#pragma once

#include <d2d1.h>
#include <dwrite.h>
#include <string>

#pragma comment(lib, "dwrite.lib")

namespace fps_monitor {

/**
 * @brief DirectWrite text rendering for stats
 * 
 * Renders FPS values and statistics with custom fonts and colors.
 * Includes drop shadow for readability.
 */
class TextRenderer {
public:
    /**
     * @brief Construct a new Text Renderer
     */
    TextRenderer();

    /**
     * @brief Destroy the Text Renderer
     */
    ~TextRenderer();

    /**
     * @brief Initialize the text renderer
     * 
     * @param renderTarget D2D render target
     * @param fontFamily Font family name (e.g., "Consolas")
     * @param fontSize Font size
     * @return true if initialized successfully
     * @return false otherwise
     */
    bool initialize(ID2D1HwndRenderTarget* renderTarget, const std::wstring& fontFamily, float fontSize);

    /**
     * @brief Render main FPS display (large, prominent)
     * 
     * @param fps FPS value to display
     * @param x X position
     * @param y Y position
     * @param brush Text color brush
     */
    void renderFPS(double fps, float x, float y, ID2D1SolidColorBrush* brush);

    /**
     * @brief Render statistics text
     * 
     * @param label Label text (e.g., "AVG:")
     * @param value Value to display
     * @param x X position
     * @param y Y position
     * @param brush Text color brush
     */
    void renderStat(const std::wstring& label, double value, float x, float y, ID2D1SolidColorBrush* brush);

    /**
     * @brief Render arbitrary text
     * 
     * @param text Text to render
     * @param x X position
     * @param y Y position
     * @param brush Text color brush
     * @param withShadow Add drop shadow
     */
    void renderText(const std::wstring& text, float x, float y, ID2D1SolidColorBrush* brush, bool withShadow = true);

    /**
     * @brief Set shadow color
     * 
     * @param brush Shadow brush
     */
    void setShadowBrush(ID2D1SolidColorBrush* brush);

private:
    IDWriteFactory* m_writeFactory;           ///< DirectWrite factory
    IDWriteTextFormat* m_textFormat;          ///< Normal text format
    IDWriteTextFormat* m_largeTextFormat;     ///< Large text format (for FPS)
    ID2D1HwndRenderTarget* m_renderTarget;    ///< Render target
    ID2D1SolidColorBrush* m_shadowBrush;      ///< Shadow brush
    float m_fontSize;                         ///< Base font size
};

} // namespace fps_monitor
