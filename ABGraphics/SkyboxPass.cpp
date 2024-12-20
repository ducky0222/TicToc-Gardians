#include "pch.h"

#include "SkyboxPass.h"

#include "BindableCommon.h"
#include "TransformCBufSkybox.h"
#include "Vertex.h"
#include "GeometryGenerator.h"

namespace Rgph
{
	SkyboxPass::SkyboxPass(Graphics& graphics, const std::string& name)
		: BindingPass(name)
	{
		using namespace Bind;

		registerSink(DirectBufferSink<RenderTarget>::Make("renderTarget", mRenderTarget));
		registerSink(DirectBufferSink<DepthStencil>::Make("depthStencil", mDepthStencil));
		registerSource(DirectBufferSource<RenderTarget>::Make("renderTarget", mRenderTarget));
		registerSource(DirectBufferSource<DepthStencil>::Make("depthStencil", mDepthStencil));

		// AddBind(std::make_shared<CubeTexture>(graphics, "../Resources/Texture/CubicFirstCubeMapEnvHDR.dds"));
		AddBind(Stencil::Create(graphics, Stencil::Mode::DepthFirst));
		AddBind(Sampler::Create(graphics, Sampler::Type::Bilinear));
		AddBind(Rasterizer::Create(graphics, true));
		AddBind(std::make_shared<TransformCBufSkybox>(graphics));

		auto VertexShader = VertexShader::Create(graphics, "../Resources/Shader/Skybox_VS.hlsl");
		AddBind(VertexShader);
		AddBind(PixelShader::Create(graphics, "../Resources/Shader/Skybox_PS.hlsl"));
		AddBind(Topology::Create(graphics, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

		GeometryGenerator::MeshData meshData;
		GeometryGenerator::CreateSphere(1, 10, 10, &meshData);

		Dvtx::VertexLayout vLayout;
		vLayout.Append(Dvtx::VertexLayout::Position3D);

		Dvtx::VertexBuffer vBuffer{ vLayout };

		for (auto& vertex : meshData.Vertices)
		{
			vBuffer.EmplaceBack(vertex.Position);
		}

		const std::string GEOMETRY_TAG = "$sphere_map";
		mpSphereVertices = VertexBuffer::Create(graphics, GEOMETRY_TAG, vBuffer);
		mpSphereIndices = IndexBuffer::Create(graphics, GEOMETRY_TAG, meshData.Indices);
		mSphereCount = (UINT)meshData.Indices.size();

		AddBind(InputLayout::Create(graphics, vLayout, VertexShader->GetBytecode()));
	}

	void SkyboxPass::Execute(Graphics& graphics) const
	{
		if (mSkybox != nullptr)
		{
			mSkybox->Bind(graphics);

			mpSphereVertices->Bind(graphics);
			mpSphereIndices->Bind(graphics);
			BindAll(graphics);
			graphics.DrawIndexed(mSphereCount);
		}
	}

	void SkyboxPass::SetSkymap(Graphics& graphics, const std::string& path)
	{
		mSkybox = Bind::CubeTexture::Create(graphics, path);
	}
}