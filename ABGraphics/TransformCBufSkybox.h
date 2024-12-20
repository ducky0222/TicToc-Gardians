#pragma once

#include "ConstantBuffer.h"

namespace Bind
{
	/*
	Bind시 별도의 로직이 필요하다면 요렇게 재정의해서 사용한다.
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