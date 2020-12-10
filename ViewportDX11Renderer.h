#pragma once
#include "stdafx.h"
#include "ImguiDX11WinRenderer.h"
#include "ViewportRenderer.h"
#include "ViewportScene.h"


class ImguiDX11WinRenderer;
class ViewportScene;
class ViewportDX11Renderer :public ViewportRenderer
{
public:
    ViewportDX11Renderer(ImguiDX11WinRenderer* imguiRenderer, ImVec2 size);
    ~ViewportDX11Renderer();
    void Frame() override;
    ID3D11ShaderResourceView* GetShaderResourceView();
protected:
    friend class ViewportScene;

    ViewportScene* _pScene = nullptr;

    ID3D11Device* _pVDevice = nullptr;
    ID3D11DeviceContext* _pVContext = nullptr;
    IDXGISwapChain* _pVSwapChain = nullptr;
    ID3D11RenderTargetView* _pVRenderTarget = nullptr;
    ID3D11Texture2D* _pRenderTargetTexture = nullptr;
    ID3D11Texture2D* _pDepthStencil = nullptr;
    ID3D11ShaderResourceView* _shaderResourceView = nullptr;
    ID3D11DepthStencilView* _pDepthStencilView = nullptr;
};


