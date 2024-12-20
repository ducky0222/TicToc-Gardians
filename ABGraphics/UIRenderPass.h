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
	class UIRenderPass : public RenderQueuePass
	{
	public:
		UIRenderPass(Graphics& graphics, std::string name)
			: RenderQueuePass(std::move(name), {})
		{
			using namespace Bind;

			// 싱크 소스 매핑
			registerSink(DirectBufferSink<RenderTarget>::Make("renderTarget", mRenderTarget));
			registerSink(DirectBufferSink<DepthStencil>::Make("depthStencil", mDepthStencil));

			registerSource(DirectBufferSource<RenderTarget>::Make("renderTarget", mRenderTarget));
			registerSource(DirectBufferSource<DepthStencil>::Make("depthStencil", mDepthStencil));

			AddBind(Sampler::Create(graphics, Sampler::Type::Point));
			AddBind(Stencil::Create(graphics, Stencil::Mode::DepthOff));

			AddBind(Blender::Create(graphics, false));
			AddBind(Rasterizer::Create(graphics, true));
		}
	};
}
