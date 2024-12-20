#pragma once

#include "ConstantBuffer.h"
#include "IDrawable.h"

namespace Bind
{
	class TransformCbufShadow : public IBindable
	{
	public:
		struct ShadowTransform
		{
			Matrix ShadowWorld;
			Matrix ShadowViewProj;
		};

	public:
		TransformCbufShadow(Graphics& graphics, UINT slot = 0u);

		void Bind(Graphics& graphics) override;
		void InitReference(const IDrawable& parent) override;

	private:
		std::shared_ptr<VertexConstantBuffer<ShadowTransform>> pVcbuf;
		const IDrawable* pParent = nullptr;
	};
}