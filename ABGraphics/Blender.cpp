#include "pch.h"

#include "Blender.h"

namespace Bind
{
	Blender::Blender(Graphics& graphics, bool blending, std::optional<float> factor)
		: mBlending(blending)
	{
		if (factor.has_value())
		{
			mFactors.emplace();
			mFactors->fill(factor.value());
		}

		D3D11_BLEND_DESC blendDesc = CD3D11_BLEND_DESC{ CD3D11_DEFAULT{} };
		auto& brt = blendDesc.RenderTarget[0];

		if (blending)
		{
			brt.BlendEnable = true;

			if (factor)
			{
				brt.SrcBlend = D3D11_BLEND_BLEND_FACTOR;
				brt.DestBlend = D3D11_BLEND_INV_BLEND_FACTOR;
			}
			else
			{
				brt.SrcBlend = D3D11_BLEND_SRC_ALPHA;
				brt.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
			}
		}

		GetDevice(graphics)->CreateBlendState(&blendDesc, &mpBlender);
	}

	shared_ptr<Blender> Blender::Create(Graphics& graphics, bool blending, optional<float> factor)
	{
		return ResourceManager::Create<Blender>(graphics, blending, factor);
	}

	string Blender::GenerateUID(bool blending, optional<float> factor)
	{
		return typeid(Blender).name()
			+ "#"s + (blending ? "b"s : "n"s)
			+ (factor ? "#f"s + std::to_string(*factor) : "");
	}
}