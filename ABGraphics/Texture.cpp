#include "pch.h"

#include "Texture.h"
#include "BindableResourceManager.h"
#include "Surface.h"

namespace Bind
{
	Texture::Texture(Graphics& graphics, const std::string& path, UINT slot)
		: mPath(path)
		, mSlot(slot)
	{
		// 파일 로딩
		const auto surface = Surface::FromFile(path);
		mbHasAlpha = surface.AlphaLoaded();

		// 텍스처 생성
		D3D11_TEXTURE2D_DESC textureDesc = {};
		textureDesc.Width = surface.GetWidth();
		textureDesc.Height = surface.GetHeight();
		textureDesc.MipLevels = 0;
		textureDesc.ArraySize = 1;
		textureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
		textureDesc.CPUAccessFlags = 0;
		textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
		ComPtr<ID3D11Texture2D> pTexture;
		GetDevice(graphics)->CreateTexture2D(&textureDesc, nullptr, &pTexture);

		// surface로 텍스처 초기 정보 업데이트
		GetContext(graphics)->UpdateSubresource(pTexture.Get(), 0u, nullptr, surface.GetBufferPtrConst(), surface.GetWidth() * sizeof(Surface::Color), 0u);

		// SRV 생성
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = textureDesc.Format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = -1;
		GetDevice(graphics)->CreateShaderResourceView(pTexture.Get(), &srvDesc, &mpTextureView);

		// 밉맵 수준 생성
		GetContext(graphics)->GenerateMips(mpTextureView.Get());
	}

	std::shared_ptr<Texture> Texture::Create(Graphics& graphics, const std::string& path, UINT slot)
	{
		return ResourceManager::Create<Texture>(graphics, path, slot);
	}
	std::string Texture::GenerateUID(const std::string& path, UINT slot)
	{
		using namespace std::string_literals;
		return typeid(Texture).name() + "#"s + path + "#" + std::to_string(slot);
	}

	void Texture::Bind(Graphics& graphics)
	{
		GetContext(graphics)->PSSetShaderResources(mSlot, 1u, mpTextureView.GetAddressOf());
	}

	std::string Texture::GetUID() const
	{
		return GenerateUID(mPath, mSlot);
	}
	bool Texture::HasAlpha() const
	{
		return mbHasAlpha;
	}

	UINT Texture::calculateNumberOfMipLevels(UINT width, UINT height)
	{
		const float xSteps = std::ceil(log2((float)width));
		const float ySteps = std::ceil(log2((float)height));

		return (UINT)std::fmax(xSteps, ySteps);
	}

	TextureArray::TextureArray(Graphics& graphics, const std::vector<std::string>& paths, UINT slot)
		: mPaths(paths)
		, mSlot(slot)
	{
		// 파일 로딩

		std::vector<Surface> surfaces;
		surfaces.reserve(mPaths.size());

		for (const std::string& path : mPaths)
		{
			surfaces.push_back(Surface::FromFile(path));
		}

		const size_t MIP_LEVEL = 2;

		// 텍스처 생성
		D3D11_TEXTURE2D_DESC textureDesc = {};
		textureDesc.Width = surfaces[0].GetWidth();
		textureDesc.Height = surfaces[0].GetHeight();
		textureDesc.MipLevels = MIP_LEVEL;
		textureDesc.ArraySize = paths.size();
		textureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
		textureDesc.CPUAccessFlags = 0;
		textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
		ComPtr<ID3D11Texture2D> pTexture;
		GetDevice(graphics)->CreateTexture2D(&textureDesc, nullptr, &pTexture);

		// surface로 텍스처 초기 정보 업데이트
		for (size_t i = 0; i < surfaces.size(); ++i)
		{
			GetContext(graphics)->UpdateSubresource(
				pTexture.Get(),

				D3D11CalcSubresource(0, i, MIP_LEVEL),
				nullptr,
				surfaces[i].GetBufferPtrConst(),
				surfaces[i].GetWidth() * sizeof(Surface::Color),
				0u);
		}
		// SRV 생성
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = textureDesc.Format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
		srvDesc.Texture2DArray.MostDetailedMip = 0;
		srvDesc.Texture2DArray.MipLevels = MIP_LEVEL;
		srvDesc.Texture2DArray.FirstArraySlice = 0;
		srvDesc.Texture2DArray.ArraySize = mPaths.size();
		HRESULT hr = GetDevice(graphics)->CreateShaderResourceView(pTexture.Get(), &srvDesc, &mTextureView);

		// 밉맵 수준 생성
		GetContext(graphics)->GenerateMips(mTextureView.Get());
	}

	void TextureArray::Bind(Graphics& graphics)
	{
		GetContext(graphics)->PSSetShaderResources(mSlot, 1u, mTextureView.GetAddressOf());
	}
}
