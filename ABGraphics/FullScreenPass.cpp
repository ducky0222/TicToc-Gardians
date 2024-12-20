#include "pch.h"
#include "FullScreenPass.h"

#include "Graphics.h"
#include "Vertex.h"
#include "BindableCommon.h"

namespace Rgph
{
	FullScreenPass::FullScreenPass(Graphics& graphics, const std::string name)
		: BindingPass(std::move(name))
	{
		Dvtx::VertexLayout lay;
		lay.Append(Dvtx::VertexLayout::Position2D);
		Dvtx::VertexBuffer bufFull{ lay };
		bufFull.EmplaceBack(Vector2{ -1, 1 });
		bufFull.EmplaceBack(Vector2{ 1, 1 });
		bufFull.EmplaceBack(Vector2{ -1, -1 });
		bufFull.EmplaceBack(Vector2{ 1, -1 });
		AddBind(Bind::VertexBuffer::Create(graphics, "$Full", std::move(bufFull)));

		std::vector<UINT> indices =
		{
			0,1,2,
			1,3,2
		};
		AddBind(Bind::IndexBuffer::Create(graphics, "$Full123", std::move(indices)));

		// setup other common fullscreen bindables
		auto vs = Bind::VertexShader::Create(graphics, "../Resources/Shader/Fullscreen_VS.hlsl");
		AddBind(Bind::InputLayout::Create(graphics, lay, vs->GetBytecode()));
		AddBind(std::move(vs));
		AddBind(Bind::Topology::Create(graphics));
		AddBind(Bind::Rasterizer::Create(graphics, false));
	}

	void FullScreenPass::Execute(Graphics& graphics) const
	{
		BindAll(graphics);

		graphics.DrawIndexed(6u);
	}
}