#include "pch.h"

#include "GeometryShader.h"
#include "Util.h"
#include "BindableResourceManager.h"

namespace Bind
{
	GeometryShader::GeometryShader(Graphics& graphics, const string& path)
	{
		Microsoft::WRL::ComPtr<ID3DBlob> pBlob;

		HRESULT hr = Util::CompileShaderFromFile(Util::ToWide(path).c_str(), "main", "gs_5_0", pBlob.GetAddressOf());

		if (FAILED(hr))
		{
			hr = D3DReadFileToBlob(Util::ToWide(path).c_str(), &pBlob);
		}

		assert(SUCCEEDED(hr));

		hr = GetDevice(graphics)->CreateGeometryShader(
			pBlob->GetBufferPointer(),
			pBlob->GetBufferSize(),
			nullptr,
			mGeometryShader.GetAddressOf()
		);

		assert(SUCCEEDED(hr));

	}

	shared_ptr<GeometryShader> GeometryShader::Create(Graphics& graphics, const string& path)
	{
		return ResourceManager::Create<GeometryShader>(graphics, path);
	}
	string GeometryShader::GenerateUID(const string& path)
	{
		return typeid(GeometryShader).name() + "#"s + path;
	}

	void GeometryShader::Bind(Graphics& graphics)
	{
		GetContext(graphics)->GSSetShader(mGeometryShader.Get(), nullptr, 0u);
	}

	string GeometryShader::GetUID() const
	{
		return GenerateUID(mPath);
	}
}