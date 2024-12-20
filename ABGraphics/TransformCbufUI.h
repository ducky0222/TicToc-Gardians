#pragma once

#include "ConstantBuffer.h"
#include "IDrawable.h"
#include "IBindable.h"

namespace Bind
{
	class TransformCbufUI : public IBindable
	{
	protected:
		struct Transforms
		{
			Matrix World;
			Matrix Proj;
			Matrix TexTransform;
		};

	public:
		TransformCbufUI(Graphics& graphics, UINT slot = 0u);

		void Bind(Graphics& graphics) override;
		void InitReference(const IDrawable& parent) override;

	private:
		std::shared_ptr<VertexConstantBuffer<Transforms>> mVertexCB;
		const IDrawable* mParent = nullptr;
	};
}