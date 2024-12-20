#pragma once

#include "IBindable.h"

namespace Bind
{
	class ShadowSampler : public IBindable
	{
	public:
		ShadowSampler(Graphics& graphics, UINT slot = 0);
		~ShadowSampler() = default;

		void Bind(Graphics& graphics) override;

	protected:
		Microsoft::WRL::ComPtr<ID3D11SamplerState> mSampler;
		UINT mSlot;
	};
}