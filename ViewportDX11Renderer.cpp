#include "ViewportDX11Renderer.h"

ViewportDX11Renderer::ViewportDX11Renderer(ImguiDX11WinRenderer* imguiRenderer, ImVec2 size) {

    _pVDevice = imguiRenderer->_pDevice;
    _pVSwapChain = imguiRenderer->_pSwapChain;
    _pVContext = imguiRenderer->_pContext;

    D3D11_TEXTURE2D_DESC textureDesc;
    ZeroMemory(&textureDesc, sizeof(textureDesc));

    // описание render target (цели рендера)
    textureDesc.Width = size.x;
    textureDesc.Height = size.y;
    textureDesc.MipLevels = 1;
    textureDesc.ArraySize = 1;
    textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    textureDesc.SampleDesc.Count = 1;
    textureDesc.Usage = D3D11_USAGE_DEFAULT;
    textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    textureDesc.CPUAccessFlags = 0;
    textureDesc.MiscFlags = 0;
    NMEX_HR(_pVDevice->CreateTexture2D(&textureDesc, NULL, &_pRenderTargetTexture));


    D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
    // Описание render target view.
    renderTargetViewDesc.Format = textureDesc.Format;
    renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    renderTargetViewDesc.Texture2D.MipSlice = 0;

    NMEX_HR(_pVDevice->CreateRenderTargetView(_pRenderTargetTexture, &renderTargetViewDesc, &_pVRenderTarget));


    D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
    // Описание shader resource view.
    shaderResourceViewDesc.Format = textureDesc.Format;
    shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
    shaderResourceViewDesc.Texture2D.MipLevels = 1;
    // Создание shader resource view.
    NMEX_HR(_pVDevice->CreateShaderResourceView(_pRenderTargetTexture, &shaderResourceViewDesc, &_shaderResourceView));

    D3D11_TEXTURE2D_DESC descDepth;
    ZeroMemory(&descDepth, sizeof(descDepth));
    descDepth.Width = size.x;
    descDepth.Height = size.y;
    descDepth.MipLevels = 1;
    descDepth.ArraySize = 1;
    descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    descDepth.SampleDesc.Count = 1;
    descDepth.SampleDesc.Quality = 0;
    descDepth.Usage = D3D11_USAGE_DEFAULT;
    descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    descDepth.CPUAccessFlags = 0;
    descDepth.MiscFlags = 0;
    NMEX_HR(_pVDevice->CreateTexture2D(&descDepth, NULL, &_pDepthStencil));

    D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
    ZeroMemory(&descDSV, sizeof(descDSV));
    descDSV.Format = descDepth.Format;
    descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    descDSV.Texture2D.MipSlice = 0;
    NMEX_HR( _pVDevice->CreateDepthStencilView(_pDepthStencil,&descDSV,&_pDepthStencilView));
    
    _pScene = new ViewportScene(this);
}

ViewportDX11Renderer::~ViewportDX11Renderer() {
    _pScene->~ViewportScene();
    delete _pScene;
    _RELEASE(_pVRenderTarget);
    _RELEASE(_pRenderTargetTexture);
    _RELEASE(_pDepthStencil);
    _RELEASE(_shaderResourceView);
}

void ViewportDX11Renderer::Frame() {
    _pVContext->OMSetRenderTargets(1, &_pVRenderTarget,_pDepthStencilView);
    float color[4] = { 0.0f, 0.2f, 0.0f, 1.0f };
    _pVContext->ClearRenderTargetView(_pVRenderTarget, color);
    _pVContext->ClearDepthStencilView(_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

    //_pVContext->PSSetShaderResources(0, 1, &_shaderResourceView);
    _pScene->Render(this);
    
    _pVSwapChain->Present(1, 0);
}

ID3D11ShaderResourceView* ViewportDX11Renderer::GetShaderResourceView() {
    return _shaderResourceView;
}

