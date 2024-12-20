#include "pch.h"

#include "Util.h"
#include "BindableResourceManager.h"
#include "CubeTexture.h"

namespace Bind
{
	CubeTexture::CubeTexture(Graphics& graphics, const std::string& path, UINT slot)
		: IBindable()
		, mSlot(slot)
	{
		HRESULT hr = DirectX::CreateDDSTextureFromFile(GetDevice(graphics), Util::ToWide(path).c_str(), NULL, mpTextureView.GetAddressOf());

		assert(SUCCEEDED(hr));
	}
	CubeTexture::CubeTexture(Graphics& graphics, const std::vector<std::string>& paths, UINT slot)
	{
		// 하나씩 데이터 읽어서 만드는 로직
		assert(false);
	}

	std::shared_ptr<CubeTexture> CubeTexture::Create(Graphics& graphics, const std::string& path, UINT slot)
	{
		return ResourceManager::Create<CubeTexture>(graphics, path, slot);
	}
	std::string CubeTexture::GenerateUID(const std::string& path, UINT slot)
	{
		using namespace std::string_literals;
		return typeid(CubeTexture).name() + "#"s + path + "#" + std::to_string(slot);
	}

	void CubeTexture::Bind(Graphics& graphics)
	{
		GetContext(graphics)->PSSetShaderResources(mSlot, 1u, mpTextureView.GetAddressOf());
	}

	std::string CubeTexture::GetUID() const
	{
		return GenerateUID(mPath, mSlot);
	}
}