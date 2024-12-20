#include "pch.h"

#include "TransformCbuf.h"
#include "Camera.h"
#include "BindableResourceManager.h"
#include "MathUtil.h"
#include "Util.h"

namespace Bind
{
	TransformCbuf::TransformCbuf(Graphics& graphics, UINT slot)
	{
		mVertexCB = VertexConstantBuffer<Transforms>::Create(graphics, slot);
	}

	void TransformCbuf::Bind(Graphics& graphics)
	{
		updateBindImpl(graphics, getTransforms(graphics));
	}

	void TransformCbuf::InitReference(const IDrawable& parent)
	{
		mParent = &parent;
	}

	void TransformCbuf::updateBindImpl(Graphics& graphics, const Transforms& transform)
	{
		assert(mParent != nullptr);
		mVertexCB->Update(graphics, transform);
		mVertexCB->Bind(graphics);
	}

	TransformCbuf::Transforms TransformCbuf::getTransforms(Graphics& graphics)
	{
		assert(mParent != nullptr);
		const auto modelView = mParent->GetTransform() * graphics.GetView();

		// 텍스처 변환 행렬
		TransformCbuf::Transforms result =
		{
			mParent->GetTransform().Transpose(),
			graphics.GetView().Transpose(),
			graphics.GetProjection().Transpose(),
			graphics.GetShadowView().Transpose(),
			graphics.GetShadowProj().Transpose(),
			mParent->GetTexTransform().Transpose(),
			MathHelper::InverseTranspose(mParent->GetTransform()).Transpose()
		};
		return result;
	}
}