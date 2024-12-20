#pragma once

#include "IBindable.h"

namespace Bind
{
	class Rasterizer : public IBindable
	{
	public:
		Rasterizer(Graphics& graphics, bool twoSided);
		Rasterizer(Graphics& graphics, D3D11_CULL_MODE cullMode);
		~Rasterizer() = default;

		static shared_ptr<Rasterizer> Create(Graphics& graphics, bool twoSided);
		static shared_ptr<Rasterizer> Create(Graphics& graphics, D3D11_CULL_MODE cullMode);
		static string GenerateUID(bool twoSided);
		static string GenerateUID(D3D11_CULL_MODE cullMode);

		virtual void Bind(Graphics& graphics)  override;

		string GetUID() const  override;

	protected:
		Microsoft::WRL::ComPtr<ID3D11RasterizerState> mpRasterizer;
		D3D11_CULL_MODE mCullMode;
	};
}
