#include "pch.h"
#include "ShadowSampler.h"

namespace Bind
{
	ShadowSampler::ShadowSampler(Graphics& graphics, UINT slot)
		: mSlot(slot)
	{
		D3D11_SAMPLER_DESC samplerDesc = {};
		samplerDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
		samplerDesc.MaxAnisotropy = 1u;
		samplerDesc.ComparisonFunc = D3D11_COMPARISON_LESS;
		samplerDesc.MinLOD = 0;
		samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

		GetDevice(graphics)->CreateSamplerState(&samplerDesc, mSampler.GetAddressOf());
	}

	void ShadowSampler::Bind(Graphics& graphics)
	{
		GetContext(graphics)->PSSetSamplers(mSlot, 1, mSampler.GetAddressOf());
	}
}