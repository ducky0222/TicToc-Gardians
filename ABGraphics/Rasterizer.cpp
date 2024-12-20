#include "pch.h"

#include "Rasterizer.h"

#include "BindableResourceManager.h"

namespace Bind
{
	Rasterizer::Rasterizer(Graphics& graphics, bool twoSided)
		: Rasterizer(graphics, twoSided ? D3D11_CULL_NONE : D3D11_CULL_BACK)
	{
	}

	Rasterizer::Rasterizer(Graphics& graphics, D3D11_CULL_MODE cullMode)
		: mCullMode(cullMode)
	{
		D3D11_RASTERIZER_DESC rasterDesc = CD3D11_RASTERIZER_DESC(CD3D11_DEFAULT{});
		rasterDesc.CullMode = cullMode;

		GetDevice(graphics)->CreateRasterizerState(&rasterDesc, &mpRasterizer);
	}

	void Rasterizer::Bind(Graphics& graphics)
	{
		GetContext(graphics)->RSSetState(mpRasterizer.Get());
	}

	std::shared_ptr<Rasterizer> Rasterizer::Create(Graphics& graphics, bool twoSided)
	{
		return ResourceManager::Create<Rasterizer>(graphics, twoSided);
	}
	std::shared_ptr<Rasterizer> Rasterizer::Create(Graphics& graphics, D3D11_CULL_MODE cullMode)
	{
		return ResourceManager::Create<Rasterizer>(graphics, cullMode);
	}

	string Rasterizer::GenerateUID(bool twoSided)
	{
		return GenerateUID(twoSided ? D3D11_CULL_NONE : D3D11_CULL_BACK);
	}
	std::string Rasterizer::GenerateUID(D3D11_CULL_MODE cullMode)
	{
		using namespace std::string_literals;

		return typeid(Rasterizer).name() + "#"s + std::to_string((int)cullMode);
	}
	std::string Rasterizer::GetUID() const
	{
		return GenerateUID(mCullMode);
	}
}