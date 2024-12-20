#include "TypeInfo.h"
#include "Property.h"

#include "../GameEngine/System.h"

const TypeInfo* TypeInfo::GetSuper() const
{
	return m_super;
}

bool TypeInfo::IsA(const TypeInfo& other) const
{
	if (this == &other)
	{
		return true;
	}

	return m_typeHash == other.m_typeHash;
}

bool TypeInfo::IsChildOf(const TypeInfo& other) const
{
	if (IsA(other))
	{
		return true;
	}

	for (const TypeInfo* super = m_super; super != nullptr; super = super->GetSuper())
	{
		if (super->IsA(other))
		{
			return true;
		}
	}

	return false;
}

const Property* TypeInfo::GetProperty(const char* name) const
{
	auto iter = m_propertyMap.find(name);
	return (iter != std::end(m_propertyMap)) ? iter->second : nullptr;
}

std::unique_ptr<System> TypeInfo::GenerateSystem(const std::string& systemName, GameObjectManager* goManager)
{
	auto it = SystemGenerators.find(systemName);
	if (it != SystemGenerators.end())
	{
		return it->second->Generate(goManager);
	}
	return nullptr;
}

void TypeInfo::CollectSuperProperties()
{
	const std::vector<const Property*>& properties = m_super->GetProperties();
	for (const Property* property : properties)
	{
		AddProperty(property);
	}
}

void TypeInfo::AddProperty(const Property* property)
{
	m_properties.emplace_back(property);
	m_propertyMap.emplace(property->GetName(), property);
}