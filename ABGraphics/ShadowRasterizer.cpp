#include "pch.h"
#include "ShadowRasterizer.h"

namespace Bind
{
	ShadowRasterizer::ShadowRasterizer(Graphics& gfx, int depthBias, float slopeBias, float clamp)
		: mDepthBias(depthBias)
		, mSlopeBias(slopeBias)
		, mClamp(clamp)
	{
		D3D11_RASTERIZER_DESC rasterDesc = CD3D11_RASTERIZER_DESC(CD3D11_DEFAULT{});
		rasterDesc.DepthBias = depthBias;
		rasterDesc.SlopeScaledDepthBias = slopeBias;
		rasterDesc.DepthBiasClamp = clamp;

		GetDevice(gfx)->CreateRasterizerState(&rasterDesc, mpRasterizer.GetAddressOf());
	}

	void ShadowRasterizer::Bind(Graphics& gfx)
	{
		GetContext(gfx)->RSSetState(mpRasterizer.Get());
	}
}