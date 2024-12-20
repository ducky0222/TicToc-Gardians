#include "pch.h"
#include "PostEffectPass.h"

#include "BindableCommon.h"

namespace Rgph
{
	PostEffectPass::PostEffectPass(Graphics& graphics, std::string name)
		: FullScreenPass(graphics, std::move(name))
	{
		using namespace Bind;

		addBindSink<RenderTarget>("renderTargetSRV");

		registerSink(DirectBufferSink<RenderTarget>::Make("renderTarget", mRenderTarget));
		registerSink(DirectBufferSink<DepthStencil>::Make("depthStencil", mDepthStencil));

		registerSource(DirectBindableSource<RenderTarget>::Make("renderTargetSRV", mRenderTarget));

		AddBind(Bind::PixelShader::Create(graphics, "../Resources/Shader/PostEffect_PS.hlsl"));
		AddBind(Blender::Create(graphics, false));
		AddBind(Sampler::Create(graphics, Sampler::Type::Point, true));
	}

	void PostEffectPass::Execute(Graphics& graphics) const
	{
		ID3D11ShaderResourceView* SRVs[10] = { nullptr, };
		graphics.GetContext()->PSSetShaderResources(0, 10, SRVs);
		ID3D11RenderTargetView* rtv = nullptr;
		ID3D11DepthStencilView* dsv = nullptr;
		graphics.GetContext()->OMSetRenderTargets(1, &rtv, dsv);
		FullScreenPass::Execute(graphics);
	}
}