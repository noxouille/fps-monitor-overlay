#include "text_renderer.h"
#include <sstream>
#include <iomanip>

namespace fps_monitor {

TextRenderer::TextRenderer()
    : m_writeFactory(nullptr)
    , m_textFormat(nullptr)
    , m_largeTextFormat(nullptr)
    , m_renderTarget(nullptr)
    , m_shadowBrush(nullptr)
    , m_fontSize(14.0f)
{
}

TextRenderer::~TextRenderer() {
    if (m_largeTextFormat) m_largeTextFormat->Release();
    if (m_textFormat) m_textFormat->Release();
    if (m_writeFactory) m_writeFactory->Release();
}

bool TextRenderer::initialize(ID2D1HwndRenderTarget* renderTarget, const std::wstring& fontFamily, float fontSize) {
    if (!renderTarget) {
        return false;
    }

    m_renderTarget = renderTarget;
    m_fontSize = fontSize;

    // Create DirectWrite factory
    HRESULT hr = DWriteCreateFactory(
        DWRITE_FACTORY_TYPE_SHARED,
        __uuidof(IDWriteFactory),
        reinterpret_cast<IUnknown**>(&m_writeFactory)
    );

    if (FAILED(hr)) {
        return false;
    }

    // Create normal text format
    hr = m_writeFactory->CreateTextFormat(
        fontFamily.c_str(),
        nullptr,
        DWRITE_FONT_WEIGHT_NORMAL,
        DWRITE_FONT_STYLE_NORMAL,
        DWRITE_FONT_STRETCH_NORMAL,
        fontSize,
        L"en-us",
        &m_textFormat
    );

    if (FAILED(hr)) {
        return false;
    }

    // Create large text format (for FPS display)
    hr = m_writeFactory->CreateTextFormat(
        fontFamily.c_str(),
        nullptr,
        DWRITE_FONT_WEIGHT_BOLD,
        DWRITE_FONT_STYLE_NORMAL,
        DWRITE_FONT_STRETCH_NORMAL,
        fontSize * 2.5f,
        L"en-us",
        &m_largeTextFormat
    );

    if (FAILED(hr)) {
        return false;
    }

    // Create default shadow brush
    m_renderTarget->CreateSolidColorBrush(
        D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.8f),
        &m_shadowBrush
    );

    return true;
}

void TextRenderer::renderFPS(double fps, float x, float y, ID2D1SolidColorBrush* brush) {
    if (!m_renderTarget || !m_largeTextFormat || !brush) {
        return;
    }

    std::wostringstream oss;
    oss << std::fixed << std::setprecision(0) << fps;
    std::wstring text = oss.str();

    D2D1_RECT_F rect = D2D1::RectF(x, y, x + 200.0f, y + 100.0f);

    // Draw shadow
    if (m_shadowBrush) {
        D2D1_RECT_F shadowRect = D2D1::RectF(x + 2, y + 2, x + 202.0f, y + 102.0f);
        m_renderTarget->DrawText(
            text.c_str(),
            static_cast<UINT32>(text.length()),
            m_largeTextFormat,
            shadowRect,
            m_shadowBrush
        );
    }

    // Draw text
    m_renderTarget->DrawText(
        text.c_str(),
        static_cast<UINT32>(text.length()),
        m_largeTextFormat,
        rect,
        brush
    );
}

void TextRenderer::renderStat(const std::wstring& label, double value, float x, float y, ID2D1SolidColorBrush* brush) {
    if (!m_renderTarget || !m_textFormat || !brush) {
        return;
    }

    std::wostringstream oss;
    oss << label << L" " << std::fixed << std::setprecision(1) << value;
    std::wstring text = oss.str();

    renderText(text, x, y, brush, true);
}

void TextRenderer::renderText(const std::wstring& text, float x, float y, ID2D1SolidColorBrush* brush, bool withShadow) {
    if (!m_renderTarget || !m_textFormat || !brush) {
        return;
    }

    D2D1_RECT_F rect = D2D1::RectF(x, y, x + 500.0f, y + 50.0f);

    // Draw shadow
    if (withShadow && m_shadowBrush) {
        D2D1_RECT_F shadowRect = D2D1::RectF(x + 1, y + 1, x + 501.0f, y + 51.0f);
        m_renderTarget->DrawText(
            text.c_str(),
            static_cast<UINT32>(text.length()),
            m_textFormat,
            shadowRect,
            m_shadowBrush
        );
    }

    // Draw text
    m_renderTarget->DrawText(
        text.c_str(),
        static_cast<UINT32>(text.length()),
        m_textFormat,
        rect,
        brush
    );
}

void TextRenderer::setShadowBrush(ID2D1SolidColorBrush* brush) {
    m_shadowBrush = brush;
}

} // namespace fps_monitor
