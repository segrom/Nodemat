#pragma once
#include "stdafx.h"
#include "ViewportDX11Renderer.h"

using namespace DirectX;

class ViewportDX11Renderer;
class ViewportScene
{
public:
	ViewportScene(ViewportDX11Renderer* vpRenderer);
	~ViewportScene();
	void Render(ViewportDX11Renderer* vpRenderer);
private:
	ViewportDX11Renderer* _myViewportRenderer = nullptr;
	ID3D11Buffer* _pVertexBuffer = nullptr;
	ID3D11Buffer* _pIndexBuffer = nullptr;
	ID3D11Buffer* _pConstantBuffer = nullptr;
	ID3D11InputLayout* _pVertexLayout = nullptr;
	ID3D11VertexShader* _pVertexShader = nullptr;
	ID3D11PixelShader* _pPixelShader = nullptr;

	XMMATRIX _worldMat;
	XMMATRIX _viewMat;
	XMMATRIX _projectionMat;

	HRESULT CompileShaderFromFile(LPCSTR file, LPCSTR entryPoint, LPCSTR shaderModel, ID3DBlob** ppBlobOut);
};

