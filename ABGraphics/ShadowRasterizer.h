#pragma once

#include "IBindable.h"

namespace Bind
{
	class ShadowRasterizer : public IBindable
	{
	public:
		ShadowRasterizer(Graphics& gfx, int depthBias, float slopeBias, float clamp);
		~ShadowRasterizer() = default;

		void Bind(Graphics& gfx) override;

		int GetDepthBias() const { return mDepthBias; }
		float GetSlopeBias() const { return mSlopeBias; }
		float GetClamp() const { return mClamp; }

	private:
		Microsoft::WRL::ComPtr<ID3D11RasterizerState> mpRasterizer;
		int mDepthBias;
		float mSlopeBias;
		float mClamp;
	};
}