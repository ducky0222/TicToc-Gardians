#include "pch.h"

#include "NullPixelShader.h"
#include "BindableResourceManager.h"

namespace Bind
{
	NullPixelShader::NullPixelShader(Graphics& graphics)
	{
	}
	std::shared_ptr<NullPixelShader> NullPixelShader::Create(Graphics& graphics)
	{
		return ResourceManager::Create<NullPixelShader>(graphics);
	}
	std::string NullPixelShader::GenerateUID()
	{
		return typeid(NullPixelShader).name();
	}

	void NullPixelShader::Bind(Graphics& graphics)
	{
		GetContext(graphics)->PSSetShader(nullptr, nullptr, 0u);
	}

	std::string NullPixelShader::GetUID() const 
	{
		return GenerateUID();
	}
}
