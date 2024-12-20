#pragma once

#include "FullScreenPass.h"
#include "BindableCommon.h"
#include "BindableResourceManager.h"

namespace Rgph
{
	class SsaoBlurPass : public FullScreenPass
	{
	public:
		struct PSCBuffer0
		{
			float TexelWidth;
			float TexelHeight;
			int bIsHorizotalBlur;
			int unused[1];
		};

	public:
		SsaoBlurPass(Graphics& graphics, std::string name)
			: FullScreenPass(graphics, std::move(name))
		{
			using namespace Bind;

			UINT textureWidth = graphics.GetWidth() / 2;
			UINT textureHeight = graphics.GetHeight() / 2;

			// ½ÌÅ©, ¼Ò½º
			registerSink(DirectBindableSink<ShaderInputRenderTarget>::Make("ssaoMap", mShaderInputRTV0));
			mShaderInputRTV1 = std::make_shared<ShaderInputRenderTarget>(graphics, textureWidth, textureHeight, 10, DXGI_FORMAT_R16_FLOAT);
			registerSource(DirectBindableSource<ShaderInputRenderTarget>::Make("ssaoMap", mShaderInputRTV1));
			mRenderTarget = mShaderInputRTV1;

			// ½¦ÀÌ´õ
			AddBind(PixelShader::Create(graphics, "../Resources/Shader/SsaoBlur_PS.hlsl"));

			// ºäÆ÷Æ®
			AddBind(std::make_shared<Bind::Viewport>(graphics, textureWidth, textureHeight));

			// »ó¼ö¹öÆÛ
			mPSCBuffer0.TexelWidth = 1.f / textureWidth;
			mPSCBuffer0.TexelHeight = 1.f / textureHeight;
			mPSCBuffer0.bIsHorizotalBlur = true;
			mPSCB = std::make_shared<PixelConstantBuffer<PSCBuffer0>>(graphics, mPSCBuffer0, 0);
			AddBind(mPSCB);

			// »ùÇÃ·¯
			AddBind(Sampler::Create(graphics, Sampler::Type::Bilinear, false, 0));
		}

		// todo
		void OnResize() {}

		void Execute(Graphics& graphics) const override
		{
			BindAll(graphics);

			for (int i = 0; i < 4; ++i)
			{
				blurAmbientMap(graphics, mShaderInputRTV0.get(), mShaderInputRTV1.get(), true);
				blurAmbientMap(graphics, mShaderInputRTV1.get(), mShaderInputRTV0.get(), false);
			}
		}

	private:
		void blurAmbientMap(Graphics& graphics, Bind::ShaderInputRenderTarget* inputSRV, Bind::ShaderInputRenderTarget* outputRTV, bool bIsHorizotalBlur) const
		{
			mPSCBuffer0.bIsHorizotalBlur = bIsHorizotalBlur;
			mPSCB->Update(graphics, mPSCBuffer0);

			outputRTV->BindBuffer(graphics);
			outputRTV->Clear(graphics, { 0, 0,0,0 });

			inputSRV->Bind(graphics);

			graphics.DrawIndexed(6u);

			ID3D11ShaderResourceView* nullSRV = { nullptr };
			graphics.GetContext()->PSSetShaderResources(10, 1, &nullSRV);
		}

	private:
		// blurCB
		// srv rtv pingpong
		std::shared_ptr<Bind::ShaderInputRenderTarget> mShaderInputRTV1;
		std::shared_ptr<Bind::ShaderInputRenderTarget> mShaderInputRTV0;

		mutable PSCBuffer0 mPSCBuffer0;
		std::shared_ptr<Bind::PixelConstantBuffer<PSCBuffer0>> mPSCB;
	};
}