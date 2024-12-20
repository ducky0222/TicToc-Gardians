#pragma once
#include "CRC.h"
#include "GameObjectManager.h"
#include "../Reflection/TypeInfoMacro.h"
#include "../Reflection/PropertyMacro.h"

struct Component
{
	Component() = default;
	virtual ~Component() = default;

	virtual uint32_t GetUid() = 0;
	virtual uint32_t Release() = 0;
	virtual const TypeInfo& GetTypeInfo() = 0;
	virtual void AddThis(GameObjectManager* goManager, uint32_t objectId) const = 0;

	virtual void SerializePool(nlohmann::json& json) const {}
	virtual void DeserializePool(const nlohmann::json& json, GameObjectManager* goManager) const {};
	virtual void ResetPool() const = 0;

	virtual void SerializeThis(nlohmann::json& json) const {}
	virtual Component* DeserializeThis(const nlohmann::json& json, GameObjectManager* goManager, uint32_t objectId) const { return nullptr; }

	template <typename T> 
	T* Sibling(GameObjectManager* goManager)
	{
		return goManager->QueryComponent<T>(GameObjectId);
	}

	uint32_t GameObjectId = 0;
};