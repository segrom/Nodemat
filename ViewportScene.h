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
	void Render();
private:
	ViewportDX11Renderer* _myViewportRenderer;
	ID3D11Buffer* _pVertexBuffer;
	ID3D11Buffer* _pIndexBuffer;
	ID3D11Buffer* _pConstantBuffer;
	ID3D11InputLayout* _pVertexLayout;
	ID3D11VertexShader* _pVertexShader;
	ID3D11PixelShader* _pPixelShader;

	XMMATRIX _worldMat;
	XMMATRIX _viewMat;
	XMMATRIX _projectionMat;

	HRESULT CompileShaderFromFile(LPCSTR file, LPCSTR entryPoint, LPCSTR shaderModel, ID3DBlob** ppBlobOut);
};
