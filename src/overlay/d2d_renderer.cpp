#include "d2d_renderer.h"

namespace fps_monitor {

D2DRenderer::D2DRenderer()
    : m_factory(nullptr)
    , m_renderTarget(nullptr)
    , m_hwnd(nullptr)
    , m_initialized(false)
{
}

D2DRenderer::~D2DRenderer() {
    shutdown();
}

bool D2DRenderer::initialize(HWND hwnd) {
    if (m_initialized) {
        return true;
    }

    m_hwnd = hwnd;

    // Create D2D factory
    HRESULT hr = D2D1CreateFactory(
        D2D1_FACTORY_TYPE_SINGLE_THREADED,
        &m_factory
    );

    if (FAILED(hr)) {
        return false;
    }

    // Create render target
    if (!createRenderTarget(hwnd)) {
        shutdown();
        return false;
    }

    m_initialized = true;
    return true;
}

void D2DRenderer::shutdown() {
    releaseRenderTarget();

    if (m_factory) {
        m_factory->Release();
        m_factory = nullptr;
    }

    m_initialized = false;
}

void D2DRenderer::beginDraw() {
    if (m_renderTarget) {
        m_renderTarget->BeginDraw();
    }
}

bool D2DRenderer::endDraw() {
    if (!m_renderTarget) {
        return false;
    }

    HRESULT hr = m_renderTarget->EndDraw();

    // Check for device lost
    if (hr == D2DERR_RECREATE_TARGET) {
        releaseRenderTarget();
        return createRenderTarget(m_hwnd);
    }

    return SUCCEEDED(hr);
}

void D2DRenderer::clear(float r, float g, float b, float a) {
    if (m_renderTarget) {
        m_renderTarget->Clear(D2D1::ColorF(r, g, b, a));
    }
}

ID2D1SolidColorBrush* D2DRenderer::createSolidBrush(float r, float g, float b, float a) {
    if (!m_renderTarget) {
        return nullptr;
    }

    ID2D1SolidColorBrush* brush = nullptr;
    m_renderTarget->CreateSolidColorBrush(
        D2D1::ColorF(r, g, b, a),
        &brush
    );

    return brush;
}

ID2D1HwndRenderTarget* D2DRenderer::getRenderTarget() const {
    return m_renderTarget;
}

ID2D1Factory* D2DRenderer::getFactory() const {
    return m_factory;
}

bool D2DRenderer::resize(UINT width, UINT height) {
    if (m_renderTarget) {
        D2D1_SIZE_U size = D2D1::SizeU(width, height);
        HRESULT hr = m_renderTarget->Resize(size);
        return SUCCEEDED(hr);
    }
    return false;
}

bool D2DRenderer::isInitialized() const {
    return m_initialized && m_renderTarget != nullptr;
}

bool D2DRenderer::createRenderTarget(HWND hwnd) {
    if (!m_factory || !hwnd) {
        return false;
    }

    RECT rc;
    GetClientRect(hwnd, &rc);

    D2D1_SIZE_U size = D2D1::SizeU(
        rc.right - rc.left,
        rc.bottom - rc.top
    );

    D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(
        D2D1_RENDER_TARGET_TYPE_DEFAULT,
        D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED),
        0, 0,
        D2D1_RENDER_TARGET_USAGE_NONE,
        D2D1_FEATURE_LEVEL_DEFAULT
    );

    HRESULT hr = m_factory->CreateHwndRenderTarget(
        props,
        D2D1::HwndRenderTargetProperties(hwnd, size),
        &m_renderTarget
    );

    if (FAILED(hr)) {
        return false;
    }

    // Enable anti-aliasing
    m_renderTarget->SetAntialiasMode(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);

    return true;
}

void D2DRenderer::releaseRenderTarget() {
    if (m_renderTarget) {
        m_renderTarget->Release();
        m_renderTarget = nullptr;
    }
}

} // namespace fps_monitor
