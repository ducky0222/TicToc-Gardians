#include "pch.h"

#include "Sampler.h"
#include "BindableResourceManager.h"

namespace Bind
{
	Sampler::Sampler(Graphics& graphics, Type type, bool reflect, UINT slot)
		: Sampler(graphics, type, reflect ? D3D11_TEXTURE_ADDRESS_MIRROR : D3D11_TEXTURE_ADDRESS_WRAP, slot)
	{
	}

	Sampler::Sampler(Graphics& graphics, Type type, D3D11_TEXTURE_ADDRESS_MODE addressMode, UINT slot)
		: mType(type)
		, mAddressMode(addressMode)
		, mSlot(slot)
	{
		D3D11_SAMPLER_DESC samplerDesc = CD3D11_SAMPLER_DESC{ CD3D11_DEFAULT{} };

		samplerDesc.BorderColor[0] = { 0.f };
		samplerDesc.BorderColor[1] = { 0.f };
		samplerDesc.BorderColor[2] = { 0.f };
		samplerDesc.BorderColor[3] = { 1e5f };

		samplerDesc.Filter = [type]()
			{
				switch (type)
				{
				case Type::Anisotropic:
					return D3D11_FILTER_ANISOTROPIC;
				case Type::Point:
					return D3D11_FILTER_MIN_MAG_MIP_POINT;
				case Type::Bilinear:
					return D3D11_FILTER_MIN_MAG_MIP_LINEAR;
				case Type::SSAOSampler:
					return D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
				default:
					assert(false);
				}
			}();

			if (type == Type::SSAOSampler)
			{
				samplerDesc.BorderColor[0] = { 0.f };
				samplerDesc.BorderColor[1] = { 0.f };
				samplerDesc.BorderColor[2] = { 0.f };
				samplerDesc.BorderColor[3] = { 1e5f };
				samplerDesc.MinLOD = 0;
				samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
			}

			samplerDesc.AddressU = addressMode;
			samplerDesc.AddressV = addressMode;
			samplerDesc.AddressW = addressMode;
			samplerDesc.MaxAnisotropy = D3D11_REQ_MAXANISOTROPY;

			GetDevice(graphics)->CreateSamplerState(&samplerDesc, &mpSampler);
	}

	void Sampler::Bind(Graphics& graphics)
	{
		GetContext(graphics)->PSSetSamplers(mSlot, 1, mpSampler.GetAddressOf());
	}

	std::shared_ptr<Sampler> Sampler::Create(Graphics& graphics, Type type, bool reflect, UINT slot)
	{
		return ResourceManager::Create<Sampler>(graphics, type, reflect, slot);
	}
	std::shared_ptr<Sampler> Sampler::Create(Graphics& graphics, Type type, D3D11_TEXTURE_ADDRESS_MODE addressMode, UINT slot)
	{
		return ResourceManager::Create<Sampler>(graphics, type, addressMode, slot);
	}
	std::string Sampler::GenerateUID(Type type, bool reflect, UINT slot)
	{
		return GenerateUID(type, reflect ? D3D11_TEXTURE_ADDRESS_MIRROR : D3D11_TEXTURE_ADDRESS_WRAP, slot);
	}
	std::string Sampler::GenerateUID(Type type, D3D11_TEXTURE_ADDRESS_MODE addressMode, UINT slot)
	{
		using namespace std::string_literals;
		return typeid(Sampler).name() + "#"s + std::to_string((int)type) + std::to_string(addressMode) + std::to_string(slot);
	}

	std::string Sampler::GetUID() const
	{
		return GenerateUID(mType, mbIsReflect);
	}
}