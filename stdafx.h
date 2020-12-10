#pragma once
#include <iostream>
#include <sstream>
#include <ostream>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <ShellScalingApi.h>

#define DIRECTINPUT_VERSION 0x0800
#include <d3d11.h>
#include <dinput.h>
#include <tchar.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include <D3DX11.h>
#include <D3Dcompiler.h>


#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_win32.h"
#include "nodeEditor/imgui_node_editor.h"

#include "NodematException.h"

#define IM_MAX(A, B) (((A) >= (B)) ? (A) : (B))
#define IM_MIN(A, B) (((A) <= (B)) ? (A) : (B))
#define _RELEASE(p)		{ if(p){(p)->Release(); (p)=nullptr;} }