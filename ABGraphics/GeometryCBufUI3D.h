#pragma once

#include "ConstantBuffer.h"
#include "IDrawable.h"
#include "IBindable.h"

namespace Bind
{
	class GeometryCBufUI3D : public IBindable
	{
	protected:
		struct Transforms
		{
			Matrix ViewProj;
			Matrix TexTransform;
			Vector3 LookVector;
			float padding0;
			Vector3 Postion;
			float padding1;
			Vector2 Size;
			float unused[2];
		};

	public:
		GeometryCBufUI3D(Graphics& graphics, UINT slot = 0u);

		void Bind(Graphics& graphics) override;
		void InitReference(const IDrawable& parent) override;

	private:
		std::shared_ptr<GeometryConstantBuffer<Transforms>> mGeometryCB;
		const IDrawable* mParent = nullptr;
	};
}