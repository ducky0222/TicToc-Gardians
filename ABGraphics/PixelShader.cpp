#include "pch.h"

#include "PixelShader.h"
#include "Graphics.h"
#include "BindableResourceManager.h"
#include "Util.h"

namespace Bind
{
	PixelShader::PixelShader(Graphics& graphics, const std::string& path)
	{
		Microsoft::WRL::ComPtr<ID3DBlob> pBlob;

		HRESULT hr = Util::CompileShaderFromFile(Util::ToWide(path).c_str(), "main", "ps_5_0", pBlob.GetAddressOf());

		if (FAILED(hr))
		{
			hr = D3DReadFileToBlob(Util::ToWide(path).c_str(), &pBlob);
		}

		assert(SUCCEEDED(hr));

		hr = GetDevice(graphics)->CreatePixelShader(
			pBlob->GetBufferPointer(),
			pBlob->GetBufferSize(),
			nullptr,
			&mpPixelShader);

		assert(SUCCEEDED(hr));
	}

	shared_ptr<PixelShader> PixelShader::Create(Graphics& graphics, const std::string& path)
	{
		return ResourceManager::Create<PixelShader>(graphics, path);
	}
	string PixelShader::GenerateUID(const std::string& path)
	{
		return typeid(PixelShader).name() + "#"s + path;
	}

	void PixelShader::Bind(Graphics& graphics)
	{
		GetContext(graphics)->PSSetShader(mpPixelShader.Get(), nullptr, 0u);
	}

	string PixelShader::GetUID() const
	{
		return GenerateUID(mPath);
	}

}