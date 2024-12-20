#pragma once
#include "System.h"

class Scene;

class SerializeSystem : public System
{
public:
	using System::System;

	bool SerializeScene(Scene* scene, const std::string& filePath);
	bool DeserializeScene(Scene* scene, const std::string& filePath);

	bool SerializePrefab(uint32_t gameObjectId, const std::string& filePath);
	uint32_t DeserializePrefab(const std::string& filePath); // 게임오브젝트의 루트 부모를 리턴한다. 

	uint32_t Instantiate(const std::string& filePath);
	uint32_t Instantiate(const std::string& filePath, uint32_t parentGameObjectId);
	uint32_t Instantiate(const std::string& filePath, uint32_t parentGameObjectId, bool instantiateInWorldSpace);
	uint32_t Instantiate(const std::string& filePath, Vector3 position, Quaternion rotation);
	uint32_t Instantiate(const std::string& filePath, Vector3 position, Quaternion rotation, uint32_t parentGameObjectId);

private:
	void SerializePrefabRecursive(uint32_t gameObjectId, nlohmann::json& prefabJson, bool isTop);
	uint32_t GetHighestNumber(const nlohmann::json& jsonObj);
};