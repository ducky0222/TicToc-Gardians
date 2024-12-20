#pragma once

#include "FullScreenPass.h"
#include "BindableCommon.h"
#include "BindableResourceManager.h"
#include "Vertex.h"
#include "Sink.h"
#include "MathUtil.h"
#include "Util.h"

namespace Rgph
{
	class SsaoPass : public BindingPass
	{
	public:
		struct Vertex
		{
			Vector3 Pos;
			Vector3 Normal;
			Vector2 UV;
		};

		// Cbuffer
		struct VSCBuffer0
		{
			Vector4 FrustumCorners[4];
		};

		struct PSCBuffer0
		{
			Matrix ViewToTex;
			Vector4 OffsetVector[14];
		};

	public:
		SsaoPass(Graphics& graphics, std::string name)
			: BindingPass(std::move(name))
		{
			using namespace Bind;

			UINT textureWidth = graphics.GetWidth() / 2;
			UINT textureHeight = graphics.GetHeight() / 2;

			//(DirectBindableSink<ShaderInputRenderTarget>::Make("depthNormalBuffer", mDepthNormalSRV));
			addBindSink<IBindable>("depthNormalBuffer");

			mSsaoMap = std::make_shared<ShaderInputRenderTarget>(graphics, textureWidth, textureHeight, 10, DXGI_FORMAT_R16_FLOAT);
			registerSource(DirectBindableSource<ShaderInputRenderTarget>::Make("ssaoMap", mSsaoMap));
			mRenderTarget = mSsaoMap;

			// AddBind(mDepthNormalSRV);
			AddBind(Bind::VertexShader::Create(graphics, "../Resources/Shader/Ssao_VS.hlsl"));
			AddBind(PixelShader::Create(graphics, "../Resources/Shader/Ssao_PS.hlsl"));

			// vertexLayout
			Vertex vertices[4];
			vertices[0].Pos = XMFLOAT3(-1.0f, -1.0f, 0.0f);
			vertices[1].Pos = XMFLOAT3(-1.0f, +1.0f, 0.0f);
			vertices[2].Pos = XMFLOAT3(+1.0f, +1.0f, 0.0f);
			vertices[3].Pos = XMFLOAT3(+1.0f, -1.0f, 0.0f);

			vertices[0].Normal = XMFLOAT3(0.0f, 0.0f, 0.0f);
			vertices[1].Normal = XMFLOAT3(1.0f, 0.0f, 0.0f);
			vertices[2].Normal = XMFLOAT3(2.0f, 0.0f, 0.0f);
			vertices[3].Normal = XMFLOAT3(3.0f, 0.0f, 0.0f);

			vertices[0].UV = XMFLOAT2(0.0f, 1.0f);
			vertices[1].UV = XMFLOAT2(0.0f, 0.0f);
			vertices[2].UV = XMFLOAT2(1.0f, 0.0f);
			vertices[3].UV = XMFLOAT2(1.0f, 1.0f);

			Dvtx::VertexLayout vLayout;
			vLayout.Append(Dvtx::VertexLayout::ElementType::Position3D)
				.Append(Dvtx::VertexLayout::ElementType::Normal)
				.Append(Dvtx::VertexLayout::ElementType::Texture2D);

			Dvtx::VertexBuffer vBuffer{ vLayout };

			for (int i = 0; i < 4; ++i)
			{
				vBuffer.EmplaceBack(vertices[i].Pos, vertices[i].Normal, vertices[i].UV);
			}

			AddBind(std::make_shared<VertexBuffer>(graphics, std::move(vBuffer)));

			std::vector<UINT> indices =
			{
				0,1,2,
				0,2,3
			};
			AddBind(Bind::IndexBuffer::Create(graphics, "AnotherFull", std::move(indices)));

			// setup other common fullscreen bindables
			auto vs = Bind::VertexShader::Create(graphics, "../Resources/Shader/Ssao_VS.hlsl");
			AddBind(Bind::PixelShader::Create(graphics, "../Resources/Shader/Ssao_PS.hlsl"));
			AddBind(Bind::InputLayout::Create(graphics, vLayout, vs->GetBytecode()));
			AddBind(std::move(vs));
			AddBind(Bind::Topology::Create(graphics));
			AddBind(Bind::Rasterizer::Create(graphics, false));
			AddBind(Bind::Sampler::Create(graphics, Sampler::Type::SSAOSampler, D3D11_TEXTURE_ADDRESS_BORDER, 0));
			AddBind(Bind::Sampler::Create(graphics, Sampler::Type::Bilinear, false, 1));

			static const Matrix T(
				0.5f, 0.0f, 0.0f, 0.0f,
				0.0f, -0.5f, 0.0f, 0.0f,
				0.0f, 0.0f, 1.0f, 0.0f,
				0.5f, 0.5f, 0.0f, 1.0f);

			mPSCBuffer0.ViewToTex = (graphics.GetProjection() * T).Transpose();

			buildOffsetVector();
			buildRandomVectorTexture(graphics);

			mVCB = std::make_unique<VertexConstantBuffer<VSCBuffer0>>(graphics, mVSCBuffer0, 0);
			mPCB = std::make_unique<PixelConstantBuffer<PSCBuffer0>>(graphics, mPSCBuffer0, 0);
		}

		void OnResize(Graphics& graphics, UINT width, UINT height) override
		{
			mSsaoMap->OnResize(graphics, width * 0.5f, height * 0.5f);
		}

		void Execute(Graphics& graphics) const override
		{
			ID3D11ShaderResourceView* SRVs[4] = { nullptr, };
			graphics.GetContext()->PSSetShaderResources(0, 4, SRVs);

			BindAll(graphics);

			mSsaoMap->Clear(graphics, { 0,0,0,1 });
			graphics.GetContext()->OMSetBlendState(NULL, NULL, 0xFFFFFFFF);
			graphics.GetContext()->OMSetDepthStencilState(NULL, 0);
			static const Matrix T(
				0.5f, 0.0f, 0.0f, 0.0f,
				0.0f, -0.5f, 0.0f, 0.0f,
				0.0f, 0.0f, 1.0f, 0.0f,
				0.5f, 0.5f, 0.0f, 1.0f);
			mPSCBuffer0.ViewToTex = (graphics.GetProjection() * T).Transpose();

			buildFrustumFarCorners(graphics.GetWidth(), graphics.GetHeight(), graphics.GetFov(), graphics.GetFarZ());

			mPCB->Update(graphics, mPSCBuffer0);
			mVCB->Update(graphics, mVSCBuffer0);

			mVCB->Bind(graphics);
			mPCB->Bind(graphics);
			graphics.GetContext()->PSSetShaderResources(1, 1, mRandomVectorSRV.GetAddressOf());

			graphics.DrawIndexed(6u);
		}

	private:
		void buildOffsetVector()
		{
			mPSCBuffer0.OffsetVector[0] = Vector4(+1.0f, +1.0f, +1.0f, 0.0f);
			mPSCBuffer0.OffsetVector[1] = Vector4(-1.0f, -1.0f, -1.0f, 0.0f);

			mPSCBuffer0.OffsetVector[2] = Vector4(-1.0f, +1.0f, +1.0f, 0.0f);
			mPSCBuffer0.OffsetVector[3] = Vector4(+1.0f, -1.0f, -1.0f, 0.0f);

			mPSCBuffer0.OffsetVector[4] = Vector4(+1.0f, +1.0f, -1.0f, 0.0f);
			mPSCBuffer0.OffsetVector[5] = Vector4(-1.0f, -1.0f, +1.0f, 0.0f);

			mPSCBuffer0.OffsetVector[6] = Vector4(-1.0f, +1.0f, -1.0f, 0.0f);
			mPSCBuffer0.OffsetVector[7] = Vector4(+1.0f, -1.0f, +1.0f, 0.0f);

			// 6 centers of cube faces
			mPSCBuffer0.OffsetVector[8] = Vector4(-1.0f, 0.0f, 0.0f, 0.0f);
			mPSCBuffer0.OffsetVector[9] = Vector4(+1.0f, 0.0f, 0.0f, 0.0f);

			mPSCBuffer0.OffsetVector[10] = Vector4(0.0f, -1.0f, 0.0f, 0.0f);
			mPSCBuffer0.OffsetVector[11] = Vector4(0.0f, +1.0f, 0.0f, 0.0f);

			mPSCBuffer0.OffsetVector[12] = Vector4(0.0f, 0.0f, -1.0f, 0.0f);
			mPSCBuffer0.OffsetVector[13] = Vector4(0.0f, 0.0f, +1.0f, 0.0f);

			for (int i = 0; i < 14; ++i)
			{
				// Create random lengths in [0.25, 1.0].
				float s = MathHelper::RandF(0.25f, 1.0f);

				XMVECTOR v = s * XMVector4Normalize(XMLoadFloat4(&mPSCBuffer0.OffsetVector[i]));

				XMStoreFloat4(&mPSCBuffer0.OffsetVector[i], v);
			}
		}

		void buildRandomVectorTexture(Graphics& graphics)
		{
			D3D11_TEXTURE2D_DESC texDesc;
			texDesc.Width = 256;
			texDesc.Height = 256;
			texDesc.MipLevels = 1;
			texDesc.ArraySize = 1;
			texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			texDesc.SampleDesc.Count = 1;
			texDesc.SampleDesc.Quality = 0;
			texDesc.Usage = D3D11_USAGE_IMMUTABLE;
			texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
			texDesc.CPUAccessFlags = 0;
			texDesc.MiscFlags = 0;

			D3D11_SUBRESOURCE_DATA initData = { 0 };
			initData.SysMemPitch = 256 * sizeof(DirectX::SimpleMath::Color);

			std::vector<DirectX::SimpleMath::Color> color(256 * 256);
			for (int i = 0; i < 256; ++i)
			{
				for (int j = 0; j < 256; ++j)
				{
					XMFLOAT3 v(MathHelper::RandF(), MathHelper::RandF(), MathHelper::RandF());

					color[i * 256 + j] = DirectX::SimpleMath::Color(v.x, v.y, v.z, 0.0f);
				}
			}

			initData.pSysMem = color[0];
			ID3D11Texture2D* tex = 0;
			HR(graphics.GetDevice()->CreateTexture2D(&texDesc, &initData, &tex));

			HR(graphics.GetDevice()->CreateShaderResourceView(tex, 0, &mRandomVectorSRV));

			// view saves a reference.
			ReleaseCOM(tex);
		}
		void buildFrustumFarCorners(int width, int height, float fovy, float farZ) const
		{
			float halfHeight;
			float halfWidth;

			if (fovy != 0.f)
			{
				float aspect = (float)width / (float)height;
				halfHeight = farZ * tanf(0.5f * fovy);
				halfWidth = aspect * halfHeight;
			}
			else
			{
				halfHeight = height * 0.5f;
				halfWidth = width * 0.5f;
			}


			mVSCBuffer0.FrustumCorners[0] = Vector4(-halfWidth, -halfHeight, farZ, 0.0f);
			mVSCBuffer0.FrustumCorners[1] = Vector4(-halfWidth, +halfHeight, farZ, 0.0f);
			mVSCBuffer0.FrustumCorners[2] = Vector4(+halfWidth, +halfHeight, farZ, 0.0f);
			mVSCBuffer0.FrustumCorners[3] = Vector4(+halfWidth, -halfHeight, farZ, 0.0f);
		}

	private:
		mutable VSCBuffer0 mVSCBuffer0;
		mutable PSCBuffer0 mPSCBuffer0;

		std::shared_ptr<Bind::ShaderInputRenderTarget> mSsaoMap;
		std::unique_ptr<Bind::VertexConstantBuffer<VSCBuffer0>> mVCB;
		std::unique_ptr<Bind::PixelConstantBuffer<PSCBuffer0>> mPCB;
		ComPtr<ID3D11ShaderResourceView> mRandomVectorSRV;
	};
}
