#include "pch.h"

#include "RenderTarget.h"
#include "DepthStencil.h"

namespace Bind
{
	RenderTarget::RenderTarget(Graphics& graphics, UINT width, UINT height, DXGI_FORMAT format)
		: mWidth(width)
		, mHeight(height)
		, mFormat(format)
	{
		// 텍스처 생성
		D3D11_TEXTURE2D_DESC textureDesc = {};
		textureDesc.Width = width;
		textureDesc.Height = height;
		textureDesc.MipLevels = 1;
		textureDesc.ArraySize = 1;
		textureDesc.Format = mFormat;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE; // RTV, SRV 둘 다 사용 가능함
		textureDesc.CPUAccessFlags = 0;
		textureDesc.MiscFlags = 0;

		ComPtr<ID3D11Texture2D> pTexture;
		GetDevice(graphics)->CreateTexture2D(&textureDesc, nullptr, &pTexture);

		// RTV 생성
		D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
		rtvDesc.Format = textureDesc.Format;
		rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		rtvDesc.Texture2D = D3D11_TEX2D_RTV{ 0 };
		GetDevice(graphics)->CreateRenderTargetView(pTexture.Get(), &rtvDesc, &mpTargetView);
	}

	RenderTarget::RenderTarget(Graphics& graphics, ID3D11Texture2D* pTexture)
	{
		// 텍스처로부터 RTV 생성
		D3D11_TEXTURE2D_DESC textureDesc;
		pTexture->GetDesc(&textureDesc);
		mWidth = textureDesc.Width;
		mHeight = textureDesc.Height;
		mFormat = textureDesc.Format;

		D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
		rtvDesc.Format = textureDesc.Format;
		rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		rtvDesc.Texture2D = D3D11_TEX2D_RTV{ 0 };
		GetDevice(graphics)->CreateRenderTargetView(pTexture, &rtvDesc, &mpTargetView);
	}

	void RenderTarget::BindBuffer(Graphics& graphics)
	{
		ID3D11DepthStencilView* const null = nullptr;
		bindBuffer(graphics, null);
	}

	void RenderTarget::BindBuffer(Graphics& graphics, BufferResource* depthStencil)
	{
		assert(dynamic_cast<DepthStencil*>(depthStencil) != nullptr);
		BindBuffer(graphics, static_cast<DepthStencil*>(depthStencil));
	}

	void RenderTarget::BindBuffer(Graphics& graphics, DepthStencil* depthStencil)
	{
		bindBuffer(graphics, depthStencil ? depthStencil->pDepthStencilView.Get() : nullptr);
	}

	void RenderTarget::bindBuffer(Graphics& graphics, ID3D11DepthStencilView* pDepthStencilView)
	{
		// 뷰포트와 랜더타겟 바인딩
		GetContext(graphics)->OMSetRenderTargets(1, mpTargetView.GetAddressOf(), pDepthStencilView);

		D3D11_VIEWPORT vp = {};
		vp.Width = (float)mWidth;
		vp.Height = (float)mHeight;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.TopLeftX = 0.0f;
		vp.TopLeftY = 0.0f;
		GetContext(graphics)->RSSetViewports(1u, &vp);
	}

	void RenderTarget::Clear(Graphics& graphics, const std::array<float, 4>& color)
	{
		GetContext(graphics)->ClearRenderTargetView(mpTargetView.Get(), color.data());
	}

	void RenderTarget::Clear(Graphics& graphics)
	{
		Clear(graphics, { 1,1,1,1 });
	}

	void RenderTarget::Release()
	{
		mpTargetView = nullptr;
	}

	void RenderTarget::OnResize(Graphics& graphics, ID3D11Texture2D* pTexture)
	{
		// 텍스처로부터 RTV 생성
		// mpTargetView->Release();

		D3D11_TEXTURE2D_DESC textureDesc;
		pTexture->GetDesc(&textureDesc);
		mWidth = textureDesc.Width;
		mHeight = textureDesc.Height;

		D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
		rtvDesc.Format = textureDesc.Format;
		rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		rtvDesc.Texture2D = D3D11_TEX2D_RTV{ 0 };

		GetDevice(graphics)->CreateRenderTargetView(pTexture, &rtvDesc, &mpTargetView);
	}

	void RenderTarget::OnResize(Graphics& graphics, UINT width, UINT height)
	{
		mWidth = width;
		mHeight = height;

		// 텍스처 생성
		D3D11_TEXTURE2D_DESC textureDesc = {};
		textureDesc.Width = width;
		textureDesc.Height = height;
		textureDesc.MipLevels = 1;
		textureDesc.ArraySize = 1;
		textureDesc.Format = mFormat;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE; // RTV, SRV 둘 다 사용 가능함
		textureDesc.CPUAccessFlags = 0;
		textureDesc.MiscFlags = 0;

		ComPtr<ID3D11Texture2D> pTexture;
		GetDevice(graphics)->CreateTexture2D(&textureDesc, nullptr, &pTexture);

		// RTV 생성
		D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
		rtvDesc.Format = textureDesc.Format;
		rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		rtvDesc.Texture2D = D3D11_TEX2D_RTV{ 0 };
		GetDevice(graphics)->CreateRenderTargetView(pTexture.Get(), &rtvDesc, &mpTargetView);
	}

	UINT RenderTarget::GetWidth() const
	{
		return mWidth;
	}

	UINT RenderTarget::GetHeight() const
	{
		return mHeight;
	}

	ShaderInputRenderTarget::ShaderInputRenderTarget(Graphics& graphics, UINT width, UINT height, UINT slot, DXGI_FORMAT format)
		: RenderTarget(graphics, width, height, format)
		, mSlot(slot)
	{
		ComPtr<ID3D11Resource> pRes;
		mpTargetView->GetResource(&pRes);

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = mFormat;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = 1;
		GetDevice(graphics)->CreateShaderResourceView(pRes.Get(), &srvDesc, &mpShaderResourceView);
	}

	void ShaderInputRenderTarget::Bind(Graphics& graphics)
	{
		GetContext(graphics)->PSSetShaderResources(mSlot, 1, mpShaderResourceView.GetAddressOf());
	}

	void ShaderInputRenderTarget::Release()
	{
		RenderTarget::Release();
		mpShaderResourceView = nullptr;
	}

	void ShaderInputRenderTarget::OnResize(Graphics& graphics, UINT width, UINT height)
	{
		RenderTarget::OnResize(graphics, width, height);

		ComPtr<ID3D11Resource> pRes;
		mpTargetView->GetResource(&pRes);

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = mFormat;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = 1;
		GetDevice(graphics)->CreateShaderResourceView(pRes.Get(), &srvDesc, &mpShaderResourceView);
	}

	void OutputOnlyRenderTarget::Bind(Graphics& graphics)
	{
		assert("출력 전용 RTV는 바인딩 불가능" && false);
	}

	OutputOnlyRenderTarget::OutputOnlyRenderTarget(Graphics& graphics, ID3D11Texture2D* pTexture)
		: RenderTarget(graphics, pTexture)
	{
	}
}
