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
	class UI3DRenderPass : public RenderQueuePass
	{
	public:
		UI3DRenderPass(Graphics& graphics, std::string name)
			: RenderQueuePass(std::move(name))
		{
			using namespace Bind;

			// 싱크로 랜더 타겟을 필요로 하고, 소스로 랜더 타겟을 만든다.
			registerSink(DirectBufferSink<RenderTarget>::Make("renderTarget", mRenderTarget));
			registerSink(DirectBufferSink<DepthStencil>::Make("depthStencil", mDepthStencil));

			registerSource(DirectBufferSource<RenderTarget>::Make("renderTarget", mRenderTarget));
			registerSource(DirectBufferSource<DepthStencil>::Make("depthStencil", mDepthStencil));

			AddBind(Sampler::Create(graphics, Sampler::Type::Point));
			AddBind(Stencil::Create(graphics, Stencil::Mode::DepthFirst));

			AddBind(Blender::Create(graphics, false));
			AddBind(Rasterizer::Create(graphics, true));
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

			RenderQueuePass::Execute(graphics);

			graphics.GetContext()->GSSetShader(NULL, NULL, 0);
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
		std::shared_ptr<Bind::CubeTexture> mDiffuseIBL;
		std::shared_ptr<Bind::CubeTexture> mSpecularIBL;
		std::shared_ptr<Bind::Texture> mBrdfLUT;
	};
}