#include "pch.h"
#include "OffScreenMappingPass.h"

#include "DebugRenderManager.h"
#include "BindableCommon.h"

namespace Rgph
{
	OffScreenMappingPass::OffScreenMappingPass(Graphics& graphics, std::string name)
		: FullScreenPass(graphics, std::move(name))
	{
		using namespace Bind;

		addBindSink<RenderTarget>("renderTargetSRV");
		registerSink(DirectBufferSink<RenderTarget>::Make("renderTarget", mRenderTarget));

		registerSource(DirectBufferSource<RenderTarget>::Make("renderTarget", mRenderTarget));

		AddBind(Bind::PixelShader::Create(graphics, "../Resources/Shader/Fullscreen_PS.hlsl"));
		AddBind(Blender::Create(graphics, false));
		AddBind(Sampler::Create(graphics, Sampler::Type::Point, true));
	}

	void OffScreenMappingPass::Execute(Graphics& graphics) const
	{
		ID3D11ShaderResourceView* SRVs[10] = { nullptr, };
		graphics.GetContext()->PSSetShaderResources(0, 10, SRVs);
		ID3D11RenderTargetView* rtv = nullptr;
		ID3D11DepthStencilView* dsv = nullptr;
		graphics.GetContext()->OMSetRenderTargets(1, &rtv, dsv);
		FullScreenPass::Execute(graphics);

	}
}