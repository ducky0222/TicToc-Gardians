#include "pch.h"
#include "PixelCBufAlpha.h"
#include "Model.h"

namespace Bind
{
	PixelCBufAlpha::PixelCBufAlpha(Graphics& graphics, UINT slot)
	{
		mPixelsCB = PixelConstantBuffer<AlphaData>::Create(graphics, slot);
	}

	void PixelCBufAlpha::Bind(Graphics& graphics)
	{
		AlphaData alphaData;
		alphaData.Alpha = mParent->GetAlphaData().Alpha;
		alphaData.bUseAlpha = mParent->GetAlphaData().bUsed;

		mPixelsCB->Update(graphics, alphaData);
		mPixelsCB->Bind(graphics);
	}

	void PixelCBufAlpha::InitReference(const IDrawable& parent)
	{
		mParent = &parent;
	}
}