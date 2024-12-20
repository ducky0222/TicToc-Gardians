#pragma once

#include "RenderQueuePass.h"
#include "BindableCommon.h"
#include "BindableResourceManager.h"


class Graphics;

namespace Rgph
{
	class SsaoDepthNormalPass : public RenderQueuePass
	{
	public:
		SsaoDepthNormalPass(Graphics& graphics, std::string name)
			: RenderQueuePass(std::move(name))
		{
			using namespace Bind;

			// ���� �븻 RTV�� ����� ���� ����Ÿ������ �����ϰ� �ҽ��� ����Ѵ�.
			mDepthNormalRTV = std::make_shared<ShaderInputRenderTarget>(graphics, graphics.GetWidth(), graphics.GetHeight(), 0, DXGI_FORMAT_R16G16B16A16_FLOAT);

			mRenderTarget = mDepthNormalRTV;
			registerSink(DirectBufferSink<DepthStencil>::Make("depthBuffer", mDepthStencil));

			registerSource(DirectBindableSource<ShaderInputRenderTarget>::Make("depthNormalBuffer", mDepthNormalRTV));
			registerSource(DirectBufferSource<DepthStencil>::Make("depthBuffer", mDepthStencil));

			// AddBind(VertexShader::Create(graphics, "SsaoDepthNormalBasic_VS.hlsl"));
			// AddBind(VertexShader::Create(graphics, "SsaoDepthNormalSkinned_VS.hlsl"));
			AddBind(PixelShader::Create(graphics, "../Resources/Shader/SsaoDepthNormal_PS.hlsl"));
			AddBind(Stencil::Create(graphics, Stencil::Mode::Off));
		}

		void OnResize(Graphics& graphics, UINT width, UINT height) override
		{
			mDepthNormalRTV->OnResize(graphics, width, height);
		}

		void Execute(Graphics& graphics) const override
		{
			mDepthNormalRTV->Clear(graphics, { 0.0f, 0.0f, -1.0f, 1e5f });

			RenderQueuePass::Execute(graphics);
		}

	private:
		std::shared_ptr<Bind::ShaderInputRenderTarget> mDepthNormalRTV;
	};
}