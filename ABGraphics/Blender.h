#pragma once

#include "IBindable.h"
#include "BindableResourceManager.h"

namespace Bind
{
	class Blender : public IBindable
	{
	public:
		Blender(Graphics& graphics, bool blending, std::optional<float> factor = {});

		static shared_ptr<Blender> Create(Graphics& graphics, bool blending, std::optional<float> factor = {});
		static string GenerateUID(bool blending, optional<float> factor);

		inline void Bind(Graphics& graphics) override;

		inline void SetFactor(float factor);

		inline float GetFactor() const;
		inline string GetUID() const override;

	protected:
		Microsoft::WRL::ComPtr<ID3D11BlendState> mpBlender;
		bool mBlending;
		optional<array<float, 4>> mFactors;
	};

	inline void Blender::Bind(Graphics& graphics)
	{
		const float* data = mFactors ? mFactors->data() : nullptr;
		GetContext(graphics)->OMSetBlendState(mpBlender.Get(), data, 0xFFFFFFFFu);
	}

	inline void Blender::SetFactor(float factor)
	{
		assert(mFactors);
		mFactors->fill(factor);
	}

	inline float Blender::GetFactor() const
	{
		assert(mFactors);
		return mFactors->front();
	}
	inline string Blender::GetUID() const
	{
		return GenerateUID(mBlending, mFactors ? mFactors->front() : optional<float>{});
	}
}
