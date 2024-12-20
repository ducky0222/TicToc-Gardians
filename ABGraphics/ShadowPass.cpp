#include "pch.h"
#include "ShadowPass.h"

#include "BindableCommon.h"
#include "ShadowRasterizer.h"

namespace Rgph
{
	ShadowPass::ShadowPass(Graphics& graphics, std::string name)
		: RenderQueuePass(name)
	{
		using namespace Bind;
		// AddBind(VertexShader::Create(graphics, "../SweetRedBean/Shadow_VS.hlsl"));
		AddBind(NullPixelShader::Create(graphics));
		AddBind(Stencil::Create(graphics, Stencil::Mode::Off));
		AddBind(Blender::Create(graphics, false));

		AddBind(std::make_shared<Viewport>(graphics, VIEWPORT_SIZE, VIEWPORT_SIZE));
		AddBind(std::make_shared<ShadowRasterizer>(graphics, 50, 2.f, 0.1f));

		mpDepthMap = std::make_shared<ShaderInputDepthStencil>(graphics, VIEWPORT_SIZE, VIEWPORT_SIZE, 9, DepthStencil::eUsage::ShadowDepth);
		mDepthStencil = mpDepthMap;

		registerSource(DirectBindableSource<ShaderInputDepthStencil>::Make("map", mpDepthMap));
	}

	void ShadowPass::Execute(Graphics& graphics) const
	{
		mDepthStencil->Clear(graphics);

		ID3D11ShaderResourceView* SRVs[12] = { nullptr, };
		graphics.GetContext()->PSSetShaderResources(0, 12, SRVs);

		RenderQueuePass::Execute(graphics);
	}
}