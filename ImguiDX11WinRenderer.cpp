#include "ImguiDX11WinRenderer.h"

ImguiDX11WinRenderer* ImguiDX11WinRenderer::_instance = nullptr;

ImguiDX11WinRenderer::ImguiDX11WinRenderer(wchar_t* windowName, HINSTANCE hInstance){
    _instance = this;
	ImGui_ImplWin32_EnableDpiAwareness();
	const wchar_t CLASS_NAME[] = L"Nodemat Window Class";

    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    RegisterClass(&wc);

    hwnd = CreateWindowEx(
        0,                              // Optional window styles.
        CLASS_NAME,                     // Window class
        windowName,    // Window text
        WS_OVERLAPPEDWINDOW,            // Window style
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        NULL,       // Parent window    
        NULL,       // Menu
        hInstance,  // Instance handle
        NULL        // Additional application data
    );

    NMEX(hwnd == NULL,"Window creation failed");

    if (FAILED(CreateDevice(hwnd))) {
        CleanupDevice();
        UnregisterClass(wc.lpszClassName, wc.hInstance);
        NMEX(1, "Device creation failed");
    }

    ShowWindow(hwnd, SW_SHOWMAXIMIZED);
    UpdateWindow(hwnd);
}

void ImguiDX11WinRenderer::ImguiInit() {
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init(_pDevice, _pContext);
}

void ImguiDX11WinRenderer::ImguiNewframe() {
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
}

void ImguiDX11WinRenderer::RenderDrawData(ImDrawData* data, float* clearColor,const ImGuiIO* io) {
    _pContext->OMSetRenderTargets(1, &_pRenderTarget, NULL);
    _pContext->ClearRenderTargetView(_pRenderTarget, clearColor);
    ImGui_ImplDX11_RenderDrawData(data);
    if (io->ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
    }
    _pSwapChain->Present(1, 0);
}

ImguiDX11WinRenderer::~ImguiDX11WinRenderer() { 
    CleanupDevice();
    DestroyWindow(hwnd);
    UnregisterClass(wc.lpszClassName, wc.hInstance);
}

void ImguiDX11WinRenderer::ImguiShutdown() {
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
}

HRESULT ImguiDX11WinRenderer::CreateDevice(HWND hwnd) {
    DXGI_SWAP_CHAIN_DESC scDesc;
    ZeroMemory(&scDesc, sizeof(scDesc));
    scDesc.BufferCount = 2;
    scDesc.BufferDesc.Width = 0;
    scDesc.BufferDesc.Height = 0;
    scDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    scDesc.BufferDesc.RefreshRate.Numerator = 60;
    scDesc.BufferDesc.RefreshRate.Denominator = 1;
    scDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    scDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    scDesc.OutputWindow = hwnd;
    scDesc.SampleDesc.Count = 1;
    scDesc.SampleDesc.Quality = 0;
    scDesc.Windowed = TRUE;
    scDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    UINT deviceFlags = 0;
#ifdef _DEBUG
    deviceFlags != D3D11_CREATE_DEVICE_DEBUG;
#endif // DEBUG

    const D3D_FEATURE_LEVEL featureLevelArray[4] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0 };
    for (D3D_FEATURE_LEVEL fl : featureLevelArray) {
        if (D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, deviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &scDesc, &_pSwapChain, &_pDevice, &fl, &_pContext) == S_OK)
            break;
    }
    CreateRenderTarget();
    return S_OK;
}

HRESULT ImguiDX11WinRenderer::CreateRenderTarget()
{
    ID3D11Texture2D* pBackBuffer = nullptr;
    _pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    HRESULT hr = _pDevice->CreateRenderTargetView(pBackBuffer, NULL, &_pRenderTarget);
    _RELEASE(pBackBuffer);
    return hr;
}
void ImguiDX11WinRenderer::CleanupRenderTarget()
{
    _RELEASE(_pRenderTarget);
}

void ImguiDX11WinRenderer::CleanupDevice()
{
    CleanupRenderTarget();
    _RELEASE(_pSwapChain);
    _RELEASE(_pContext);
    _RELEASE(_pDevice);
}


LRESULT CALLBACK ImguiDX11WinRenderer::WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hwnd, uMsg, wParam, lParam))
        return true;

    switch (uMsg)
    {
    case WM_SIZE:
        if (_instance->_pDevice != NULL && wParam != SIZE_MINIMIZED)
        {
            _instance->CleanupRenderTarget();
            _instance->_pSwapChain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
            _instance->CreateRenderTarget();
        }
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            return 0;
        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    case WM_DPICHANGED:
        if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_DpiEnableScaleViewports)
        {
            //const int dpi = HIWORD(wParam);
            //printf("WM_DPICHANGED to %d (%.0f%%)\n", dpi, (float)dpi / 96.0f * 100.0f);
            const RECT* suggested_rect = (RECT*)lParam;
            ::SetWindowPos(hwnd, NULL, suggested_rect->left, suggested_rect->top, suggested_rect->right - suggested_rect->left, suggested_rect->bottom - suggested_rect->top, SWP_NOZORDER | SWP_NOACTIVATE);
        }
        break;
    }
    return ::DefWindowProc(hwnd, uMsg, wParam, lParam);
}
