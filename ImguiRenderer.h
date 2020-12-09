#pragma once
#include "stdafx.h"
class ImguiRenderer
{
public:
	virtual void ImguiInit() =0;
	virtual void ImguiNewframe() =0;
	virtual void ImguiShutdown() =0;
	virtual void RenderDrawData(ImDrawData* data, float* clearColor, const ImGuiIO* io) =0;
};

