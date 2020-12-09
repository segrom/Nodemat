#include "ViewportScene.h"


using namespace DirectX;
struct SimpleVertex {
	XMFLOAT3 Pos;
};

struct ConstantBuffer {
	XMMATRIX MVP;
};

HRESULT ViewportScene::CompileShaderFromFile(LPCSTR file, LPCSTR entryPoint, LPCSTR shaderModel, ID3DBlob** ppBlobOut) {
	HRESULT hr = S_OK;
	DWORD shaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
	shaderFlags |= D3DCOMPILE_DEBUG;
#endif // _DEBUG
	ID3DBlob* pErrorBlob = nullptr;
	hr = D3DX11CompileFromFileA(file, NULL, NULL, entryPoint, shaderModel, shaderFlags, 0, NULL, ppBlobOut, &pErrorBlob, NULL);
	if (FAILED(hr) && pErrorBlob != NULL) {
		OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());
		NMEX(1,(char*)pErrorBlob->GetBufferPointer());
	}
	pErrorBlob->Release();
	return hr;
}

ViewportScene::ViewportScene(ViewportDX11Renderer* vpRenderer) :_myViewportRenderer(vpRenderer) {
	HRESULT hr = S_OK;
	ID3D10Blob* pVSBlob = nullptr;
	NMEX_HR( CompileShaderFromFile("shader.fx", "vert", "vs_4_0", &pVSBlob));
	NMEX_HR( _myViewportRenderer->_pVDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), NULL, &_pVertexShader) );

	D3D11_INPUT_ELEMENT_DESC layout[] = {
		{"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0},
	};

	UINT layoutNum = ARRAYSIZE(layout);

	NMEX_HR(_myViewportRenderer->_pVDevice->CreateInputLayout(layout, layoutNum, pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), &_pVertexLayout));
	pVSBlob->Release();
	_myViewportRenderer->_pVContext->IASetInputLayout(_pVertexLayout);

	ID3D10Blob* pPSBlob = nullptr;
	NMEX_HR(CompileShaderFromFile("shader.fx", "frag", "ps_4_0", &pPSBlob));
	NMEX_HR(_myViewportRenderer->_pVDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), NULL, &_pPixelShader));

	SimpleVertex vertices[] =
	{
		{ XMFLOAT3(-1.0f, 1.0f, -1.0f), },
		{ XMFLOAT3(1.0f, 1.0f, -1.0f), 	},
		{ XMFLOAT3(1.0f, 1.0f, 1.0f), 	},
		{ XMFLOAT3(-1.0f, 1.0f, 1.0f), 	},
		{ XMFLOAT3(-1.0f, -1.0f, -1.0f),},
		{ XMFLOAT3(1.0f, -1.0f, -1.0f), },
		{ XMFLOAT3(1.0f, -1.0f, 1.0f), 	},
		{ XMFLOAT3(-1.0f, -1.0f, 1.0f), },
	};


	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(SimpleVertex) * ARRAYSIZE(vertices);
	bd.BindFlags = 0;
	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = vertices;
	NMEX_HR( _myViewportRenderer->_pVDevice->CreateBuffer(&bd, &InitData, &_pVertexBuffer));

	UINT stride = sizeof(SimpleVertex);
	UINT offset = 0;
	_myViewportRenderer->_pVContext->IASetVertexBuffers(0, 1, &_pVertexBuffer, &stride, &offset);

	WORD indices[] =
	{
		3,1,0,
		2,1,3,

		0,5,4,
		1,5,0,

		3,4,7,
		0,4,3,

		1,6,5,
		2,6,1,

		2,7,6,
		3,7,2,

		6,4,5,
		7,4,6,
	};

	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(WORD) * ARRAYSIZE(indices);
	bd.BindFlags = D3D10_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	InitData.pSysMem = indices;
	NMEX_HR( _myViewportRenderer->_pVDevice->CreateBuffer(&bd, &InitData, &_pIndexBuffer));
	_myViewportRenderer->_pVContext->IASetIndexBuffer(_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
	_myViewportRenderer->_pVContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(ConstantBuffer);
	bd.CPUAccessFlags = 0;
	NMEX_HR( _myViewportRenderer->_pVDevice->CreateBuffer(&bd, NULL, &_pConstantBuffer));

	_worldMat = XMMatrixIdentity();

	XMVECTOR Eye = XMVectorSet(0.0f, 1.0f, -5.0f, 0.0f);
	XMVECTOR At = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	_viewMat = XMMatrixLookAtLH(Eye, At, Up);

	DXGI_SWAP_CHAIN_DESC scDesc;
	_myViewportRenderer->_pVSwapChain->GetDesc(&scDesc);
	float width = scDesc.BufferDesc.Width;
	float height = scDesc.BufferDesc.Height;
	_projectionMat = XMMatrixPerspectiveFovLH(XM_PIDIV2, width / height, 0.01f, 100.0f);
}

ViewportScene::~ViewportScene() {
	_RELEASE(_pVertexBuffer);
	_RELEASE(_pIndexBuffer);
	_RELEASE(_pConstantBuffer);
	_RELEASE(_pVertexLayout);
	_RELEASE(_pVertexShader);
	_RELEASE(_pPixelShader);
}

void ViewportScene::Render() {
	static float t = 0.01;
	static DWORD dwTimeStart = 0;
	DWORD dwTimeCur = GetTickCount();
	if (dwTimeStart == 0) dwTimeStart = dwTimeCur;
	t = (dwTimeCur - dwTimeStart) / 1000.0f;

	_worldMat = XMMatrixRotationY(t);
	ConstantBuffer cb;
	cb.MVP = XMMatrixTranspose(_worldMat) * XMMatrixTranspose(_viewMat) * XMMatrixTranspose(_projectionMat);
	_myViewportRenderer->_pVContext->UpdateSubresource(_pConstantBuffer, 0, NULL, &cb, 0, 0);

	_myViewportRenderer->_pVContext->VSSetShader(_pVertexShader, NULL, 0);
	_myViewportRenderer->_pVContext->VSSetConstantBuffers(0, 1, &_pConstantBuffer);
	_myViewportRenderer->_pVContext->PSSetShader(_pPixelShader, NULL, 0);
	_myViewportRenderer->_pVContext->DrawIndexed(36, 0, 0);
}