#include "pch.h"
#include "Viewport.h"

#include "Graphics.h"

namespace Bind
{
	Viewport::Viewport(Graphics& graphics)
		: Viewport(graphics, graphics.GetWidth(), graphics.GetHeight())
	{}
	Viewport::Viewport(Graphics& graphics, float width, float height)
	{
		mViewport.Width = width;
		mViewport.Height = height;
		mViewport.MinDepth = 0.f;
		mViewport.MaxDepth = 1.f;
		mViewport.TopLeftX = 0.f;
		mViewport.TopLeftY = 0.f;
	}

	void Viewport::Bind(Graphics& graphics)
	{
		GetContext(graphics)->RSSetViewports(1, &mViewport);
	}
}