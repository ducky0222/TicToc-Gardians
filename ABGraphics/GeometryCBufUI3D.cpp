#include "pch.h"
#include "GeometryCBufUI3D.h"
#include "Model.h"
#include "Camera.h"

namespace Bind
{
	GeometryCBufUI3D::GeometryCBufUI3D(Graphics& graphics, UINT slot)
	{
		mGeometryCB = GeometryConstantBuffer<Transforms>::Create(graphics, slot);
	}

	void GeometryCBufUI3D::Bind(Graphics& graphics)
	{
		assert(mParent != nullptr);

		const UI3DModel* model = dynamic_cast<const UI3DModel*>(mParent);

		Transforms transform;
		transform.ViewProj = graphics.GetViewProj().Transpose();
		transform.TexTransform = model->GetTexTransform().Transpose();
		transform.LookVector = model->GetLookVector();
		transform.Postion = model->GetPosition();
		transform.Size = model->GetSize();

		mGeometryCB->Update(graphics, transform);
		mGeometryCB->Bind(graphics);
	}

	void GeometryCBufUI3D::InitReference(const IDrawable& parent)
	{
		mParent = &parent;
	}
}