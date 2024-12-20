#pragma once

#include "ConstantBuffer.h"
#include "IDrawable.h"

namespace Bind
{
	class TransformCBufBone : public IBindable
	{
	public:
		struct BonePallete
		{
			Matrix BoneMat[300];
		};

	public:
		TransformCBufBone(Graphics& graphics, UINT slot = 0u);

		void Bind(Graphics& graphics) override;
		void InitReference(const IDrawable& parent) override;

	private:
		std::shared_ptr<VertexConstantBuffer<BonePallete>> pVcbuf;
		const IDrawable* pParent = nullptr;
	};
}