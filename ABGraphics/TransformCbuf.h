#pragma once

#include "ConstantBuffer.h"
#include "IDrawable.h"
#include "IBindable.h"

/*
* TransformCbuf�� ��ξ�� ������ ���� ���δ��� Ŭ�����̴�.
* Bind �Լ����� Drawable�� Ʈ������ ������ ���� �ͼ� �������� ������Ʈ�� �����Ѵ�.
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