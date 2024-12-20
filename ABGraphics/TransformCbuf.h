#pragma once

#include "ConstantBuffer.h"
#include "IDrawable.h"
#include "IBindable.h"

/*
* TransformCbuf는 드로어블 참조를 갖는 바인더블 클래스이다.
* Bind 함수에서 Drawable의 트랜스폼 정보를 갖고 와서 매프레임 업데이트를 보장한다.
*/

namespace Bind
{
	class TransformCbuf : public IBindable
	{
	protected:
		struct Transforms
		{
			Matrix Model;
			Matrix View;
			Matrix Proj;
			Matrix ShadowView;
			Matrix ShadowProj;
			Matrix Tex;
			Matrix ModelInvTranspose;
		};

	public:
		TransformCbuf(Graphics& graphics, UINT slot = 0u);

		void Bind(Graphics& graphics) override;
		void InitReference(const IDrawable& parent) override;

	protected:
		void updateBindImpl(Graphics& graphics, const Transforms& transform);
		Transforms getTransforms(Graphics& graphics);

	private:
		std::shared_ptr<VertexConstantBuffer<Transforms>> mVertexCB;
		const IDrawable* mParent = nullptr;
	};
}