#pragma once
#include "RenderQueuePass.h"
#include "Job.h"
#include "Sink.h"
#include "Source.h"
#include "Stencil.h"
#include "BindableCommon.h"
#include "ShadowSampler.h"
#include "DepthStencil.h"

class Graphics;

namespace Rgph
{
	class ForwardRenderPass : public RenderQueuePass
	{
	public:
		ForwardRenderPass(Graphics& graphics, std::string name)
			: RenderQueuePass(std::move(name))
		{
			using namespace Bind;

			// 싱크로 랜더 타겟을 필요로 하고, 소스로 랜더 타겟을 만든다.
			registerSink(DirectBufferSink<RenderTarget>::Make("renderTarget", mRenderTarget));
			registerSink(DirectBufferSink<DepthStencil>::Make("depthStencil", mDepthStencil));
			registerSink(DirectBindableSink< ShaderInputDepthStencil>::Make("shadowMap", mShadowMap));
			registerSink(DirectBindableSink< ShaderInputRenderTarget>::Make("ssaoMap", mSsaoMap));

			registerSource(DirectBufferSource<RenderTarget>::Make("renderTarget", mRenderTarget));
			registerSource(DirectBufferSource<DepthStencil>::Make("depthStencil", mDepthStencil));
			registerSource(DirectBindableSource<ShaderInputDepthStencil>::Make("shadowMap", mShadowMap));
			registerSource(DirectBindableSource<ShaderInputRenderTarget>::Make("ssaoMap", mSsaoMap));

			AddBind(std::make_shared<Bind::ShadowSampler>(graphics));
			AddBind(Sampler::Create(graphics, Sampler::Type::Anisotropic, false, 1));
			AddBind(Sampler::Create(graphics, Sampler::Type::Bilinear, false, 2));
			AddBind(Stencil::Create(graphics, Stencil::Mode::Off));
		}

		void Execute(Graphics& graphics) const override
		{
			if (mDiffuseIBL)
			{
				mDiffuseIBL->Bind(graphics);
			}
			if (mSpecularIBL)
			{
				mSpecularIBL->Bind(graphics);
			}
			if (mBrdfLUT)
			{
				mBrdfLUT->Bind(graphics);
			}

			mShadowMap->Bind(graphics);
			mSsaoMap->Bind(graphics);
			mDepthStencil->Clear(graphics);

			RenderQueuePass::Execute(graphics);
		}

		void ResetIBL() override
		{
			mDiffuseIBL = nullptr;
			mSpecularIBL = nullptr;
			mBrdfLUT = nullptr;
		}
		virtual void SetIBL(Graphics& graphics, const std::string& diffuse, const std::string& specular, const std::string& brdfLUT) override
		{
			mDiffuseIBL = Bind::CubeTexture::Create(graphics, diffuse, 6);
			mSpecularIBL = Bind::CubeTexture::Create(graphics, specular, 7);
			mBrdfLUT = Bind::Texture::Create(graphics, brdfLUT, 8);
		}

	private:
		std::shared_ptr<Bind::ShaderInputDepthStencil> mShadowMap;
		std::shared_ptr<Bind::ShaderInputRenderTarget> mSsaoMap;
		std::shared_ptr<Bind::CubeTexture> mDiffuseIBL;
		std::shared_ptr<Bind::CubeTexture> mSpecularIBL;
		std::shared_ptr<Bind::Texture> mBrdfLUT;
	};
}