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
	class HasAlphaUIRenderPass : public RenderQueuePass
	{
	public:
		HasAlphaUIRenderPass(Graphics& graphics, std::string name)
			: RenderQueuePass(std::move(name), {}, true)
		{
			using namespace Bind;

			// 싱크 소스 매핑
			registerSink(DirectBufferSink<RenderTarget>::Make("renderTarget", mRenderTarget));
			registerSink(DirectBufferSink<DepthStencil>::Make("depthStencil", mDepthStencil));

			registerSource(DirectBindableSource<RenderTarget>::Make("renderTargetSRV", mRenderTarget));
			registerSource(DirectBufferSource<DepthStencil>::Make("depthStencil", mDepthStencil));

			AddBind(Sampler::Create(graphics, Sampler::Type::Point));
			AddBind(Stencil::Create(graphics, Stencil::Mode::DepthOff));

			// 이 설정에 따라 먹어지는 투명도가 다르다.
			AddBind(Blender::Create(graphics, true));
			AddBind(Rasterizer::Create(graphics, true));
		}
	};
}