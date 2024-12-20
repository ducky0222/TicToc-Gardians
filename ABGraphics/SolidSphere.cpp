#include "pch.h"

#include "SolidSphere.h"
#include "BindableCommon.h"
#include "Vertex.h"
#include "Sphere.h"
#include "Stencil.h"
#include "TransformCbuf.h"

SolidSphere::SolidSphere(Graphics& graphics, float radius)
{
	using namespace Bind;
	namespace dx = DirectX;

	auto model = Sphere::Make();
	model.Transform(dx::XMMatrixScaling(radius, radius, radius));

	const auto geometryTag = "$sphere." + std::to_string(radius);
	pVertices = VertexBuffer::Create(graphics, geometryTag, model.vertices);
	pIndices = IndexBuffer::Create(graphics, geometryTag, model.indices);
	pTopology = Topology::Create(graphics, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// NonPBR
	{
		Technique solid;
		Step only("renderPass");

		auto pvs = VertexShader::Create(graphics, "../Resources/Shader/Solid_VS.hlsl");
		auto pvsbc = pvs->GetBytecode();
		only.AddBindable(std::move(pvs));

		only.AddBindable(PixelShader::Create(graphics, "../Resources/Shader/Solid_PS.hlsl"));

		struct PSColorConstant
		{
			dx::SimpleMath::Vector3 color = { 1.0f,1.0f,1.0f };
			float padding;
		} colorConst;
		only.AddBindable(PixelConstantBuffer<PSColorConstant>::Create(graphics, colorConst, 1u));

		only.AddBindable(InputLayout::Create(graphics, model.vertices.GetLayout(), pvsbc));

		only.AddBindable(std::make_shared<TransformCbuf>(graphics));

		only.AddBindable(Rasterizer::Create(graphics, false));

		solid.AddStep(std::move(only));
		AddTechnique(std::move(solid));
	}

	// // Deferred
	// {
	// 	Technique solid;
	// 	Step only("Deferred");
	// 
	// 	only.AddBindable(Sampler::Create(graphics, Sampler::Type::Bilinear, false));
	// 	only.AddBindable(Rasterizer::Create(graphics, false));
	// 
	// 	only.AddBindable(PixelShader::Create(graphics, "../SweetRedBean/FirstPassPS.hlsl"));
	// 	auto pvs = VertexShader::Create(graphics, "../SweetRedBean/FirstPassVS.hlsl");
	// 	auto pvsbc = pvs->GetBytecode();
	// 	only.AddBindable(std::move(pvs));
	// 
	// 	// 동적으로 추가
	// 	Dvtx::VertexLayout vertexLayout = model.vertices.GetLayout();
	// 	vertexLayout.Append(Dvtx::VertexLayout::Normal);
	// 	only.AddBindable(InputLayout::Create(graphics, model.vertices.GetLayout(), pvsbc));
	// 
	// 	// vs constant buffer
	// 	struct DirectionLight
	// 	{
	// 		Vector4 color = { 1.0f,1.0f,0.0f,1.0f };
	// 		Vector3 direction = { 1.0f,1.0f,1.0f };
	// 		float padding = 0.0f;
	// 	} dirLight;	
	// 	
	// 	only.AddBindable(VertexConstantBuffer<DirectionLight>::Create(graphics, dirLight, 1u));
	// 	only.AddBindable(std::make_shared<DefTransformCBuf>(graphics));		
	// 
	// 	// ps constant buffer
	// 
	// 	solid.AddStep(std::move(only));
	// 	AddTechnique(std::move(solid));
	// }
}
