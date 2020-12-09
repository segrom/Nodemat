#pragma once
#include "ImguiRenderer.h"
#include "ViewportDX11Renderer.h"
class ImguiDX11WinRenderer : public ImguiRenderer
{
public:	
	ImguiDX11WinRenderer(wchar_t* windowName, HINSTANCE hInstance);
	~ImguiDX11WinRenderer();
	void ImguiInit() override;
	void ImguiNewframe() override;
	void ImguiShutdown() override;
	void RenderDrawData(ImDrawData* data, float* clearColor, const ImGuiIO* io) override;
	HWND hwnd;
protected:
	friend class ViewportDX11Renderer;
	static ImguiDX11WinRenderer* _instance;
	ID3D11Device* _pDevice = nullptr;
	ID3D11DeviceContext* _pContext = nullptr;
	IDXGISwapChain* _pSwapChain = nullptr;
	ID3D11RenderTargetView* _pRenderTarget = nullptr;
	WNDCLASS wc = {};

	static LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	HRESULT CreateDevice(HWND hwnd);
	HRESULT CreateRenderTarget();
	void CleanupRenderTarget();
	void CleanupDevice();
};

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);


#ifndef WM_DPICHANGED
#define WM_DPICHANGED 0x02E0 // From Windows SDK 8.1+ headers
#endif
