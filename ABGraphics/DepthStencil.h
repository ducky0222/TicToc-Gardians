#pragma once

#include "IBindable.h"
#include "BufferResource.h"

class Graphics;

namespace Bind
{
	class RenderTarget;
	class MultiRenderTarget;

	class DepthStencil : public IBindable, public BufferResource
	{
		friend RenderTarget;
		friend MultiRenderTarget;

	public:
		enum class eUsage
		{
			DepthStencil,
			ShadowDepth
		};

	public:
		void BindBuffer(Graphics& graphics) override;
		void BindBuffer(Graphics& graphics, BufferResource* renderTarget)  override;
		void BindBuffer(Graphics& graphics, RenderTarget* renderTarget);
		void Clear(Graphics& graphics) override;

		virtual void OnResize(Graphics& graphics);

	protected:
		DepthStencil(Graphics& graphics, UINT width, UINT height, bool canBindShaderInput, eUsage usage);

	protected:
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> pDepthStencilView;
		eUsage mUsage;
	};

	// 입력(SRV)으로도 사용할 깊이 스텐실 버퍼
	class ShaderInputDepthStencil : public DepthStencil
	{
	public:
		ShaderInputDepthStencil(Graphics& graphics, UINT slot, eUsage usage = eUsage::DepthStencil);
		ShaderInputDepthStencil(Graphics& graphics, UINT width, UINT height, UINT slot, eUsage usage = eUsage::DepthStencil);

		virtual void OnResize(Graphics& graphics);

		void Bind(Graphics& graphics)  override;

	private:
		UINT slot;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pShaderResourceView;
	};

	// 출력 전용 깊이 스텐실 버퍼
	class OutputOnlyDepthStencil : public DepthStencil
	{
	public:
		OutputOnlyDepthStencil(Graphics& graphics);
		OutputOnlyDepthStencil(Graphics& graphics, UINT width, UINT height);

		using DepthStencil::OnResize;

		void Bind(Graphics& graphics) override;

	};
}
