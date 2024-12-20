#include "pch.h"
#include "TransformCbufShadow.h"

namespace Bind
{
	TransformCbufShadow::TransformCbufShadow(Graphics& graphics, UINT slot)
	{
		pVcbuf = VertexConstantBuffer<ShadowTransform>::Create(graphics, slot);
	}

	void TransformCbufShadow::Bind(Graphics& graphics)
	{
		ShadowTransform shadowTransforms;
		shadowTransforms.ShadowWorld = pParent->GetTransform().Transpose();
		shadowTransforms.ShadowViewProj = graphics.GetShadowViewProj().Transpose();

		pVcbuf->Update(graphics, shadowTransforms);
		pVcbuf->Bind(graphics);
	}

	void TransformCbufShadow::InitReference(const IDrawable& parent)
	{
		pParent = &parent;
	}
}