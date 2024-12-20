#include "pch.h"
#include "TransformCbufUI.h"

namespace Bind
{
	TransformCbufUI::TransformCbufUI(Graphics& graphics, UINT slot)
	{
		mVertexCB = VertexConstantBuffer<Transforms>::Create(graphics, slot);
	}

	void TransformCbufUI::Bind(Graphics& graphics)
	{
		Transforms trasforms;
		trasforms.World = mParent->GetTransform().Transpose();
		trasforms.TexTransform = mParent->GetTexTransform().Transpose(); // mParent->GetTexTransform();
		trasforms.Proj = graphics.GetProjection().Transpose();

		mVertexCB->Update(graphics, trasforms);
		mVertexCB->Bind(graphics);
	}

	void TransformCbufUI::InitReference(const IDrawable& parent)
	{
		mParent = &parent;
	}
}