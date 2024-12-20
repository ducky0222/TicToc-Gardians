#include "pch.h"

#include "MathUtil.h"
#include "FrameLight.h"
#include "ForwardRenderGraph.h"
#include "Source.h"
#include "RenderTarget.h"
#include "Graphics.h"
#include "DynamicConstant.h"
#include "PassCommon.h"

namespace Rgph
{
	ForwardRenderGraph::ForwardRenderGraph(Graphics& graphics, FrameLight* frameLight)
		: RenderGraph(graphics)
		, mFrameLight(frameLight)
	{
		{
			auto pass = std::make_unique<BufferClearPass>("clearRT");
			pass->SetSinkLinkage("buffer", "$.renderTarget");
			appendPass(std::move(pass));
		}
		{
			auto pass = std::make_unique<BufferClearPass>("clearDS");
			pass->SetSinkLinkage("buffer", "$.masterDepth");
			appendPass(std::move(pass));
		}
		{
			auto pass = std::make_unique<ShadowPass>(graphics, "shadowPass");
			appendPass(std::move(pass));
		}
		{
			auto pass = std::make_unique<SsaoDepthNormalPass>(graphics, "ssaoDepthNormal");
			pass->SetSinkLinkage("depthBuffer", "clearDS.buffer");

			appendPass(std::move(pass));
		}
		{
			auto pass = std::make_unique<SsaoPass>(graphics, "ssaoPass");
			pass->SetSinkLinkage("depthNormalBuffer", "ssaoDepthNormal.depthNormalBuffer");

			appendPass(std::move(pass));
		}
		{
			auto pass = std::make_unique<SsaoBlurPass>(graphics, "ssaoBlurPass");
			pass->SetSinkLinkage("ssaoMap", "ssaoPass.ssaoMap");

			appendPass(std::move(pass));
		}
		{
			auto pass = std::make_unique<BackgroundPass>(graphics, "backgroundRenderPass");
			pass->SetSinkLinkage("renderTarget", "clearRT.buffer");
			pass->SetSinkLinkage("depthStencil", "ssaoDepthNormal.depthBuffer");
			appendPass(std::move(pass));
		}
		{
			auto pass = std::make_unique<ForwardRenderPass>(graphics, "renderPass");
			pass->SetSinkLinkage("shadowMap", "shadowPass.map");
			pass->SetSinkLinkage("ssaoMap", "ssaoBlurPass.ssaoMap");
			pass->SetSinkLinkage("renderTarget", "backgroundRenderPass.renderTarget");
			pass->SetSinkLinkage("depthStencil", "backgroundRenderPass.depthStencil");

			appendPass(std::move(pass));
		}
		{
			auto pass = std::make_unique<SkyboxPass>(graphics, "skybox");
			pass->SetSinkLinkage("renderTarget", "renderPass.renderTarget");
			pass->SetSinkLinkage("depthStencil", "renderPass.depthStencil");
			appendPass(std::move(pass));
		}
		// 백그라운드 텍스처가 있으면 스카이맵보다 우선순위가 있다고 가정함?

		{
			auto pass = std::make_unique<CutScenePass>(graphics, "cutSceneRenderPass");
			pass->SetSinkLinkage("renderTarget", "skybox.renderTarget");
			pass->SetSinkLinkage("depthStencil", "skybox.depthStencil");
			appendPass(std::move(pass));
		}
		{
			auto pass = std::make_unique<UI3DRenderPass>(graphics, "UI3DRenderPass");
			pass->SetSinkLinkage("renderTarget", "cutSceneRenderPass.renderTarget");
			pass->SetSinkLinkage("depthStencil", "cutSceneRenderPass.depthStencil");
			appendPass(std::move(pass));
		}
		{
			auto pass = std::make_unique<HasAlphaForwardRenderPass>(graphics, "hasAlphaRenderPass");
			pass->SetSinkLinkage("shadowMap", "renderPass.shadowMap");
			pass->SetSinkLinkage("ssaoMap", "renderPass.ssaoMap");
			pass->SetSinkLinkage("renderTarget", "UI3DRenderPass.renderTarget");
			pass->SetSinkLinkage("depthStencil", "UI3DRenderPass.depthStencil");
			appendPass(std::move(pass));
		}
		{
			auto pass = std::make_unique<HasAlphaUI3DRenderPass>(graphics, "hasAlphaUI3DRenderPass");
			pass->SetSinkLinkage("renderTarget", "hasAlphaRenderPass.renderTarget");
			pass->SetSinkLinkage("depthStencil", "hasAlphaRenderPass.depthStencil");
			appendPass(std::move(pass));
		}
		{
			auto pass = std::make_unique<UIRenderPass>(graphics, "UIRenderPass");
			pass->SetSinkLinkage("renderTarget", "hasAlphaUI3DRenderPass.renderTarget");
			pass->SetSinkLinkage("depthStencil", "hasAlphaUI3DRenderPass.depthStencil");
			appendPass(std::move(pass));
		}
		{
			auto pass = std::make_unique<HasAlphaUIRenderPass>(graphics, "hasAlphaUIRenderPass");
			pass->SetSinkLinkage("renderTarget", "UIRenderPass.renderTarget");
			pass->SetSinkLinkage("depthStencil", "UIRenderPass.depthStencil");

			appendPass(std::move(pass));
		}
		{
			auto pass = std::make_unique<PostEffectPass>(graphics, "postEffect");
			pass->SetSinkLinkage("renderTargetSRV", "hasAlphaUIRenderPass.renderTargetSRV"); 
			pass->SetSinkLinkage("renderTarget", "$.postEffectRenderTarget");
			pass->SetSinkLinkage("depthStencil", "hasAlphaUIRenderPass.depthStencil");

			appendPass(std::move(pass));
		}
		{
			auto pass = std::make_unique<OffScreenMappingPass>(graphics, "fullScreen");
			pass->SetSinkLinkage("renderTargetSRV", "postEffect.renderTargetSRV");
			pass->SetSinkLinkage("renderTarget", "$.backbufferRenderTarget");

			appendPass(std::move(pass));
		}

		setSinkTarget("backbufferRenderTarget", "fullScreen.renderTarget");

		finalize();
	}

	void ForwardRenderGraph::Execute(Graphics& graphics)
	{
		assert(mbIsFinalized);

		for (auto& p : mPasses)
		{
			mFrameLight->Bind(graphics);
			p->Execute(graphics);
		}
	}

	void ForwardRenderGraph::OnResize(Graphics& graphics, UINT width, UINT height)
	{
		assert(mbIsFinalized);

		for (auto& pass : mPasses)
		{
			pass->OnResize(graphics, width, height);
		}
	}

	void ForwardRenderGraph::ResetSkymap()
	{
		assert(mbIsFinalized);

		for (auto& pass : mPasses)
		{
			pass->ResetSkymap();
		}
	}

	void ForwardRenderGraph::ResetIBL()
	{
		assert(mbIsFinalized);

		for (auto& pass : mPasses)
		{
			pass->ResetIBL();
		}
	}

	void ForwardRenderGraph::SetSkymap(Graphics& graphics, const std::string& path)
	{
		assert(mbIsFinalized);

		for (auto& pass : mPasses)
		{
			pass->SetSkymap(graphics, path);
		}
	}

	void ForwardRenderGraph::SetIBL(Graphics& graphics, const std::string& diffuse, const std::string& specular, const std::string& brdfLUT)
	{
		assert(mbIsFinalized);

		for (auto& pass : mPasses)
		{
			pass->SetIBL(graphics, diffuse, specular, brdfLUT);
		}
	}
}
