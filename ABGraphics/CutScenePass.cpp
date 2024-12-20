#include "pch.h"
#include "CutScenePass.h"

#include "BindableCommon.h"

namespace Rgph
{
	CutScenePass::CutScenePass(Graphics& graphics, std::string name)
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
		AddBind(Stencil::Create(graphics, Stencil::Mode::Off)); // 이상하게 작동시 깊이 판정 끄고 쓰기는 하는 걸로 재적용
	}

}