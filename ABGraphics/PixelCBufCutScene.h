#pragma once
class 
{
};

#pragma once

#include "ConstantBuffer.h"
#include "IDrawable.h"
#include "IBindable.h"
class Graphics;

namespace Bind
{
	class PixelCBufCutScene : public IBindable
	{
	public:
		struct Data
		{
			unsigned int TextureIndex;
			float unused[3];
		};

	public:
		PixelCBufCutScene(Graphics& graphics, UINT slot = 0u);

		void Bind(Graphics& graphics) override;
		void InitReference(const IDrawable& parent) override;

	private:
		std::shared_ptr<PixelConstantBuffer<Data>> mPixelsCB;
		const IDrawable* mParent = nullptr;
	};
}

