#pragma once

#include "ConstantBuffer.h"
#include "IDrawable.h"
#include "IBindable.h"
class Graphics;

namespace Bind
{
	class PixelCBufAlpha : public IBindable
	{
	public:
		struct AlphaData
		{
			int bUseAlpha;
			float Alpha;
			float unused[2];
		};

	public:
		PixelCBufAlpha(Graphics& graphics, UINT slot = 0u);

		void Bind(Graphics& graphics) override;
		void InitReference(const IDrawable& parent) override;

	private:
		std::shared_ptr<PixelConstantBuffer<AlphaData>> mPixelsCB;
		const IDrawable* mParent = nullptr;
	};
}

