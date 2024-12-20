#include "pch.h"

#include "SolidPlane.h"
#include "Plane.h"
#include "BindableCommon.h"
#include "TransformCbuf.h"

SolidPlane::SolidPlane(Graphics& graphics, float size, DirectX::SimpleMath::Color color)
{
	using namespace Bind;
	namespace dx = DirectX;

	auto model = Plane::Make();
	model.Transform(dx::XMMatrixScaling(size, size, 1.0f));
	const auto geometryTag = "$plane." + std::to_string(size);

	{
		Technique solid;
		Step renderStep("renderPass");

		pVertices = (VertexBuffer::Create(graphics, geometryTag, model.vertices));
		pIndices = (IndexBuffer::Create(graphics, geometryTag, model.indices));

		pTopology = (Topology::Create(graphics, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

		auto pvs = VertexShader::Create(graphics, "../SweetRedBean/Solid_VS.hlsl");
		auto pvsbc = pvs->GetBytecode();
		renderStep.AddBindable(std::move(pvs));

		renderStep.AddBindable(InputLayout::Create(graphics, model.vertices.GetLayout(), pvsbc));

		renderStep.AddBindable(PixelShader::Create(graphics, "../SweetRedBean/Solid_PS.hlsl"));

		renderStep.AddBindable(std::make_shared<TransformCbuf>(graphics));

		struct PSColorConstant
		{
			DirectX::SimpleMath::Color Color;
		} colorConst;

		colorConst.Color = color;
		renderStep.AddBindable(PixelConstantBuffer<PSColorConstant>::Create(graphics, colorConst));

		renderStep.AddBindable(Blender::Create(graphics, true, 0.5f));

		renderStep.AddBindable(Rasterizer::Create(graphics, true));

		solid.AddStep(renderStep);
		AddTechnique(solid);
	}
}

void SolidPlane::SpawnControlWindow(Graphics& graphics, const std::string& name)
{
	//if( ImGui::Begin( name.c_str() ) )
	//{
	//	ImGui::Text( "Position" );
	//	ImGui::SliderFloat( "X",&pos.x,-80.0f,80.0f,"%.1f" );
	//	ImGui::SliderFloat( "Y",&pos.y,-80.0f,80.0f,"%.1f" );
	//	ImGui::SliderFloat( "Z",&pos.z,-80.0f,80.0f,"%.1f" );
	//	ImGui::Text( "Orientation" );
	//	ImGui::SliderAngle( "Roll",&roll,-180.0f,180.0f );
	//	ImGui::SliderAngle( "Pitch",&pitch,-180.0f,180.0f );
	//	ImGui::SliderAngle( "Yaw",&yaw,-180.0f,180.0f );
	//	ImGui::Text( "Shading" );
	//	auto pBlender = QueryBindable<Bind::Blender>();
	//	float factor = pBlender->GetFactor();
	//	ImGui::SliderFloat( "Translucency",&factor,0.0f,1.0f );
	//	pBlender->SetFactor( factor );
	//}
	//ImGui::End();
}
