#include "pch.h"

#include "DepthStencil.h"
#include "RenderTarget.h"
#include "Graphics.h"

namespace Bind
{
	DXGI_FORMAT MapUsageTypeless(DepthStencil::eUsage usage)
	{
		switch (usage)
		{
		case DepthStencil::eUsage::DepthStencil:
			return DXGI_FORMAT::DXGI_FORMAT_R24G8_TYPELESS;

		case DepthStencil::eUsage::ShadowDepth:
			return DXGI_FORMAT::DXGI_FORMAT_R32_TYPELESS;
		default:
			assert(false);
			throw std::runtime_error{ "Base usage for Typeless format map in DepthStencil." };
		}
	}

	DXGI_FORMAT MapUsageTyped(DepthStencil::eUsage usage)
	{
		switch (usage)
		{
		case DepthStencil::eUsage::DepthStencil:
			return DXGI_FORMAT::DXGI_FORMAT_D24_UNORM_S8_UINT;
		case DepthStencil::eUsage::ShadowDepth:
			return DXGI_FORMAT::DXGI_FORMAT_D32_FLOAT;
		default:
			assert(false);
			throw std::runtime_error{ "Base usage for Typed format map in DepthStencil." };
		}
	}

	DXGI_FORMAT MapUsageColored(DepthStencil::eUsage usage)
	{
		switch (usage)
		{
		case DepthStencil::eUsage::DepthStencil:
			return DXGI_FORMAT::DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
		case DepthStencil::eUsage::ShadowDepth:
			return DXGI_FORMAT::DXGI_FORMAT_R32_FLOAT;
		default:
			assert(false);
			throw std::runtime_error{ "Base usage for Colored format map in DepthStencil." };
		}
	}

	DepthStencil::DepthStencil(Graphics& graphics, UINT width, UINT height, bool canBindShaderInput, eUsage usage)
		: mUsage(usage)
	{
		// 깊이 스텐실 텍스처 생성
		ComPtr<ID3D11Texture2D> pDepthStencil;
		D3D11_TEXTURE2D_DESC descDepth = {};
		descDepth.Width = width;
		descDepth.Height = height;
		descDepth.MipLevels = 1u;
		descDepth.ArraySize = 1u;
		descDepth.Format = MapUsageTypeless(usage);
		descDepth.SampleDesc.Count = 1u;
		descDepth.SampleDesc.Quality = 0u;
		descDepth.Usage = D3D11_USAGE_DEFAULT;
		descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL | (canBindShaderInput ? D3D11_BIND_SHADER_RESOURCE : 0);
		HRESULT hr = (GetDevice(graphics)->CreateTexture2D(&descDepth, nullptr, &pDepthStencil));

		// 깊이 스텐실 뷰 생성
		D3D11_DEPTH_STENCIL_VIEW_DESC descView = {};
		descView.Format = MapUsageTyped(usage);
		descView.Flags = 0;
		descView.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		descView.Texture2D.MipSlice = 0;

		hr = GetDevice(graphics)->CreateDepthStencilView(
			pDepthStencil.Get(),
			&descView,
			&pDepthStencilView
		);
	}

	void DepthStencil::BindBuffer(Graphics& graphics)
	{
		GetContext(graphics)->OMSetRenderTargets(0, nullptr, pDepthStencilView.Get());
	}

	void DepthStencil::BindBuffer(Graphics& graphics, BufferResource* renderTarget)
	{
		assert(dynamic_cast<RenderTarget*>(renderTarget) != nullptr);
		BindBuffer(graphics, static_cast<RenderTarget*>(renderTarget));
	}

	void DepthStencil::BindBuffer(Graphics& graphics, RenderTarget* renderTarget)
	{
		renderTarget->BindBuffer(graphics, this);
	}

	void DepthStencil::Clear(Graphics& graphics)
	{
		GetContext(graphics)->ClearDepthStencilView(pDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0u);
	}

	void DepthStencil::OnResize(Graphics& graphics)
	{
		// pDepthStencilView->Release();

		D3D11_DEPTH_STENCIL_VIEW_DESC descView = {};
		pDepthStencilView->GetDesc(&descView);

		ComPtr<ID3D11Resource> buffer;
		pDepthStencilView->GetResource(buffer.GetAddressOf());

		D3D11_TEXTURE2D_DESC depthStencilDesc;
		static_cast<ID3D11Texture2D*>(buffer.Get())->GetDesc(&depthStencilDesc);

		depthStencilDesc.Width = graphics.GetWidth();
		depthStencilDesc.Height = graphics.GetHeight();

		ComPtr<ID3D11Texture2D> pDepthStencil;
		HRESULT hr = (GetDevice(graphics)->CreateTexture2D(&depthStencilDesc, nullptr, &pDepthStencil));

		GetDevice(graphics)->CreateDepthStencilView(
			pDepthStencil.Get(),
			&descView,
			&pDepthStencilView
		);
	}

	ShaderInputDepthStencil::ShaderInputDepthStencil(Graphics& graphics, UINT slot, eUsage usage)
		: ShaderInputDepthStencil(graphics, graphics.GetWidth(), graphics.GetHeight(), slot, usage)
	{
	}

	ShaderInputDepthStencil::ShaderInputDepthStencil(Graphics& graphics, UINT width, UINT height, UINT slot, eUsage usage)
		: DepthStencil(graphics, width, height, true, usage)
		, slot(slot)
	{
		ComPtr<ID3D11Resource> pRes;
		pDepthStencilView->GetResource(&pRes);

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = MapUsageColored(usage);
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = 1;

		HRESULT hr = GetDevice(graphics)->CreateShaderResourceView(
			pRes.Get(),
			&srvDesc,
			&pShaderResourceView);
		assert(SUCCEEDED(hr));
	}

	void ShaderInputDepthStencil::Bind(Graphics& graphics)
	{
		GetContext(graphics)->PSSetShaderResources(slot, 1u, pShaderResourceView.GetAddressOf());
	}

	void ShaderInputDepthStencil::OnResize(Graphics& graphics)
	{
		DepthStencil::OnResize(graphics);

		pShaderResourceView->Release();

		ComPtr<ID3D11Resource> pRes;
		pDepthStencilView->GetResource(&pRes);

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = MapUsageColored(mUsage);
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = 1;

		HRESULT hr = GetDevice(graphics)->CreateShaderResourceView(
			pRes.Get(),
			&srvDesc,
			&pShaderResourceView);
		assert(SUCCEEDED(hr));
	}

	OutputOnlyDepthStencil::OutputOnlyDepthStencil(Graphics& graphics)
		: OutputOnlyDepthStencil(graphics, graphics.GetWidth(), graphics.GetHeight())
	{
	}

	OutputOnlyDepthStencil::OutputOnlyDepthStencil(Graphics& graphics, UINT width, UINT height)
		: DepthStencil(graphics, width, height, false, eUsage::DepthStencil)
	{
	}

	void OutputOnlyDepthStencil::Bind(Graphics& graphics)
	{
		assert("출력 전용은 입력으로 바인딩되면 안됩니다." && false);
	}
}
