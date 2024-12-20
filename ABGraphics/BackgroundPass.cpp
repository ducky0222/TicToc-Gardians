#include "pch.h"
#include "BackgroundPass.h"

#include "BindableCommon.h"

namespace Rgph
{
	BackgroundPass::BackgroundPass(Graphics& graphics, std::string name)
		: RenderQueuePass(std::move(name))
	{
		using namespace Bind;

		registerSink(DirectBufferSink<RenderTarget>::Make("renderTarget", mRenderTarget));
		registerSink(DirectBufferSink<DepthStencil>::Make("depthStencil", mDepthStencil));

		registerSource(DirectBufferSource<RenderTarget>::Make("renderTarget", mRenderTarget));
		registerSource(DirectBufferSource<DepthStencil>::Make("depthStencil", mDepthStencil));

		AddBind(Bind::PixelShader::Create(graphics, "../Resources/Shader/CutScene_PS.hlsl"));
		AddBind(Blender::Create(graphics, false));
		AddBind(Sampler::Create(graphics, Sampler::Type::Point, true));
		AddBind(Stencil::Create(graphics, Stencil::Mode::DepthOff));
	}
}