#include "pch.h"

#include "VertexShader.h"
#include "Util.h"
#include "BindableResourceManager.h"

namespace Bind
{
	VertexShader::VertexShader(Graphics& graphics, const string& path)
	{
		HRESULT hr = Util::CompileShaderFromFile(Util::ToWide(path).c_str(), "main", "vs_5_0", mpBytecodeBlob.GetAddressOf());
		
		if (FAILED(hr))
		{
			hr = D3DReadFileToBlob(Util::ToWide(path).c_str(), &mpBytecodeBlob);
		}

		assert(SUCCEEDED(hr));

		hr = GetDevice(graphics)->CreateVertexShader(
			mpBytecodeBlob->GetBufferPointer(),
			mpBytecodeBlob->GetBufferSize(),
			nullptr,
			mpVertexShader.GetAddressOf()
		);
		
		assert(SUCCEEDED(hr));
	}

	std::shared_ptr<VertexShader> VertexShader::Create(Graphics& graphics, const std::string& path)
	{
		return ResourceManager::Create<VertexShader>(graphics, path);
	}
	std::string VertexShader::GenerateUID(const std::string& path)
	{
		using namespace std::string_literals;
		return typeid(VertexShader).name() + "#"s + path;
	}

	void VertexShader::Bind(Graphics& graphics)
	{
		GetContext(graphics)->VSSetShader(mpVertexShader.Get(), nullptr, 0u);
	}

	ID3D10Blob* VertexShader::GetBytecode() const
	{
		return mpBytecodeBlob.Get();
	}
	std::string VertexShader::GetUID() const
	{
		return GenerateUID(mPath);
	}
}