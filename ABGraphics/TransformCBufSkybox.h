#pragma once

#include "ConstantBuffer.h"

namespace Bind
{
	/*
	Bind�� ������ ������ �ʿ��ϴٸ� �䷸�� �������ؼ� ����Ѵ�.
	*/

	class TransformCBufSkybox : public IBindable
	{
#pragma region Struct
	protected:
		struct Transforms
		{
			Matrix ViewProj;
		};
#pragma endregion

	public:
		TransformCBufSkybox(Graphics& graphics, UINT slot = 0u);
		~TransformCBufSkybox() = default;

		void Bind(Graphics& graphics) override;

	private:
		std::shared_ptr<VertexConstantBuffer<Transforms>> mpVcbuf;
	};
}