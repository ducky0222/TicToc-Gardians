#include "pch.h"

#include "Topology.h"
#include "BindableResourceManager.h"

namespace Bind
{
	Topology::Topology(Graphics& graphics, D3D11_PRIMITIVE_TOPOLOGY type)
		: mType(type)
	{
	}

	std::shared_ptr<Topology> Topology::Create(Graphics& graphics, D3D11_PRIMITIVE_TOPOLOGY type)
	{
		return ResourceManager::Create<Topology>(graphics, type);
	}
	std::string Topology::GenerateUID(D3D11_PRIMITIVE_TOPOLOGY type)
	{
		using namespace std::string_literals;
		return typeid(Topology).name() + "#"s + std::to_string(type);
	}

	void Topology::Bind(Graphics& graphics)
	{
		GetContext(graphics)->IASetPrimitiveTopology(mType);
	}

	std::string Topology::GetUID() const 
	{
		return GenerateUID(mType);
	}
}
