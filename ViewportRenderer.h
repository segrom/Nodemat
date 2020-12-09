#pragma once
class ViewportRenderer
{
public:
	ViewportRenderer() = default;
	~ViewportRenderer() = default;
	virtual void Frame() =0;
};

