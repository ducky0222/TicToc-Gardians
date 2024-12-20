#include "pch.h"
#include "TransformCBufSkybox.h"
#include "ConstantBuffer.h"

namespace Bind
{
	TransformCBufSkybox::TransformCBufSkybox(Graphics& graphics, UINT slot)
	{
		mpVcbuf = VertexConstantBuffer<Transforms>::Create(graphics, slot);
	}

	void TransformCBufSkybox::Bind(Graphics& graphics)
	{
		TransformCBufSkybox::Transforms transforms;
		transforms.ViewProj = graphics.GetView();
		transforms.ViewProj._41 = 0.f;
		transforms.ViewProj._42 = 0.f;
		transforms.ViewProj._43 = 0.f;
		transforms.ViewProj._44 = 1.f;
		transforms.ViewProj *= graphics.GetProjection();

		transforms.ViewProj = transforms.ViewProj.Transpose();

		mpVcbuf->Update(graphics, transforms);
		mpVcbuf->Bind(graphics);
	}
}