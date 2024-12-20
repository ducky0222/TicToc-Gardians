#include "pch.h"
#include "SerializeSystem.h"

#include <fstream>

#include "GameObject.h"
#include "GameObjectManager.h"
#include "Scene.h"

#include "Component.h"
#include "Transform.h"
#include "TransformSystem.h"
#include "Transform.h"

bool SerializeSystem::SerializeScene(Scene* scene, const std::string& filePath)
{
	// 씬 저장용 json 객체
	nlohmann::json sceneJson;

	// todo : 시스템 시리얼라이징 (사용중인 유저 시스템의 이름을 저장한 std::vector)

	// 게임오브젝트 시리얼라이징
	sceneJson["GameObjects"] = m_GameObjectManager->m_GameObjects;

	// 컴포넌트 시리얼라이징
	for (auto& [name, cp] : TypeInfo::ComponentTypes)
	{
		nlohmann::json cpJson;
		cp->SerializePool(cpJson);

		// 씬에서 사용되지 않은 컴포넌트 제외
		if (cpJson.empty())
			continue;

		sceneJson["Components"][name] = cpJson;
	}

	// 결과 값 string 변환
	std::string result = sceneJson.dump(2);

	// 파일 저장
	std::ofstream outputFile(filePath + scene->Name + ".scene");
	if (outputFile.is_open())
	{
		outputFile << result;
		outputFile.close();
		return true;
	}

#ifdef EDITOR
	ImGui::OpenPopup("Error Popup");

	if (ImGui::BeginPopupModal("Error Popup", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("Error: Unable to open file %s", filePath.c_str());
		if (ImGui::Button("OK", ImVec2(120, 0))) // Adjust button size as needed
			ImGui::CloseCurrentPopup();
		ImGui::EndPopup();
	}
#endif

	return false;
}

bool SerializeSystem::DeserializeScene(Scene* scene, const std::string& filePath)
{
	// 파일 읽기
	std::ifstream inputFile(filePath);
	if (inputFile.is_open())
	{
		// todo : 유저 시스템 초기화

		// 게임 오브젝트 초기화
		m_GameObjectManager->Reset();

		// 컴포넌트 풀 초기화
		for (auto cp : TypeInfo::GetComponentMap() | std::views::values)
		{
			cp->ResetPool();
		}

		// 파일에서 JSON 데이터 읽기
		nlohmann::json sceneJson;
		inputFile >> sceneJson;

		// todo : 시스템 디시리얼라이징
		if (sceneJson.contains("Systems"))
		{
			std::vector<std::string> systems = sceneJson["Systems"].get<std::vector<std::string>>();

			for (const auto& name : systems) 
			{
				if(TypeInfo::SystemGenerators.contains(name))
				{
					auto systemPtr = TypeInfo::GenerateSystem(name, m_GameObjectManager);

					// todo : SystemManager::AddSystem
				}
			}
		}

		// 게임 오브젝트 디시리얼라이징
		if (sceneJson.contains("GameObjects"))
		{
			m_GameObjectManager->m_GameObjects = sceneJson["GameObjects"].get<std::list<GameObject>>();
			m_GameObjectManager->m_NextGameObjectIndex = m_GameObjectManager->FindLargestId() + 1;
		}

		// 컴포넌트 디시리얼라이징
		if (sceneJson.contains("Components"))
		{
			for (auto& [name, cpJson] : sceneJson["Components"].items())
			{
				const Component* cp = nullptr;

				if (TypeInfo::ComponentTypes.contains(name))
					cp = TypeInfo::ComponentTypes[name];
				else
					continue;

				for (auto& cpData : cpJson)
				{
					cp->DeserializePool(cpData, m_GameObjectManager);
				}
			}
		}

		inputFile.close();

		return true;
	}


#ifdef EDITOR
	ImGui::OpenPopup("Error Popup");

	if (ImGui::BeginPopupModal("Error Popup", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("Error: Unable to open file %s", filePath.c_str());
		if (ImGui::Button("OK", ImVec2(120, 0))) // Adjust button size as needed
			ImGui::CloseCurrentPopup();
		ImGui::EndPopup();
	}
#endif

	return false;
}

bool SerializeSystem::SerializePrefab(uint32_t gameObjectId, const std::string& filePath)
{
	// 프리팹 저장용 json 객체
	nlohmann::json prefabJson;

	// 재귀적 시리얼라이징
	SerializePrefabRecursive(gameObjectId, prefabJson, true);

	// 결과 값 string 변환
	std::string result = prefabJson.dump(2);

	// 파일 저장
	std::ofstream outputFile(filePath);
	if (outputFile.is_open())
	{
		outputFile << result;
		outputFile.close();
		return true;
	}

	return false;
}

void SerializeSystem::SerializePrefabRecursive(uint32_t gameObjectId, nlohmann::json& prefabJson, bool isTop)
{
	auto components = m_GameObjectManager->GetAttachedComponents(gameObjectId);

	// 게임 오브젝트 정보를 현재의 json에 추가
	nlohmann::json objectInfo;
	objectInfo["Name"] = m_GameObjectManager->GetGameObjectName(gameObjectId);
	objectInfo["GameObjectId"] = gameObjectId;

	// 각 컴포넌트 정보를 현재의 json에 추가
	for (auto cp : *components | std::views::values)
	{
		nlohmann::json attached;

		if (isTop && dynamic_cast<Parent*>(cp))
			continue;

		cp->SerializeThis(attached);

		if (!attached.empty())
		{
			objectInfo["Components"][cp->GetTypeInfo().GetName()].push_back(attached);
		}
	}

	prefabJson.push_back(objectInfo);

	if (const auto children = m_GameObjectManager->QueryComponent<Children>(gameObjectId))
	{
		for (auto child : children->ChildrenID)
		{
			// 재귀적으로 자식 게임 오브젝트를 처리
			SerializePrefabRecursive(child, prefabJson, false);
		}
	}
}

uint32_t SerializeSystem::GetHighestNumber(const nlohmann::json& jsonObj)
{
	uint32_t highestObjectId = 0;

	if (jsonObj.is_array() && !jsonObj.empty())
	{
		if (jsonObj[0].contains("GameObjectId"))
			highestObjectId = jsonObj[0]["GameObjectId"].get<uint32_t>();
		else
			return highestObjectId; // 예외 처리: 잘못된 또는 빈 JSON 형식

		for (const auto& object : jsonObj)
		{
			uint32_t currentObjectNumber = object["GameObjectId"].get<uint32_t>();
			if (currentObjectNumber > highestObjectId)
			{
				highestObjectId = currentObjectNumber;
			}
		}

		return highestObjectId;
	}

	return highestObjectId; // 예외 처리: 잘못된 또는 빈 JSON 형식
}


uint32_t SerializeSystem::DeserializePrefab(const std::string& filePath)
{
	// 파일 읽기
	std::ifstream inputFile(filePath);

	uint32_t rootNum = 0;

	if (inputFile.is_open())
	{
		// 파일에서 JSON 데이터 읽기
		nlohmann::json prefabJson;
		inputFile >> prefabJson;

		uint32_t alpha = GetHighestNumber(prefabJson);

		if (alpha == 0)
		{
			ImGui::OpenPopup("Error Popup");
			return false;
		}

		auto count = prefabJson.size();

		for (const auto& object : prefabJson)
		{
			const uint32_t objectNumber = object["GameObjectId"].get<uint32_t>();
			const uint32_t baseId = m_GameObjectManager->m_NextGameObjectIndex;
			std::string name = object["Name"].get<std::string>();

			GameObject* go = m_GameObjectManager->CreateGameObject(name, objectNumber);

			for (auto& [name, cpJson] : object["Components"].items())
			{
				const Component* cp = nullptr;

				if (TypeInfo::ComponentTypes.contains(name))
					cp = TypeInfo::ComponentTypes[name];
				else
					continue;

				for (auto& cpData : cpJson)
				{
					auto cpPtr = cp->DeserializeThis(cpData, m_GameObjectManager, objectNumber + baseId);

					if (cpPtr->GetUid() == Parent::GetStaticUid())
					{
						auto parentPtr = static_cast<Parent*>(cpPtr);
						parentPtr->ParentID += m_GameObjectManager->GetNextGameObjectIndex();
					}

					if (cpPtr->GetUid() == Children::GetStaticUid())
					{
						auto childrenPtr = static_cast<Children*>(cpPtr);
						for (auto& child : childrenPtr->ChildrenID)
						{
							child += m_GameObjectManager->GetNextGameObjectIndex();
						}
					}
				}

				if (!object["Components"].contains("Parent"))
				{
					rootNum = go->Id;
				}

			}

			inputFile.close();
		}

		m_GameObjectManager->m_NextGameObjectIndex += alpha + 1;
		return rootNum;
	}

	return 0;
}

uint32_t SerializeSystem::Instantiate(const std::string& filePath)
{
	uint32_t id = DeserializePrefab(filePath);

	assert(id != 0);
	// GameProcess
	EventManager::PublishImmediate("OnStart", id);

	return id;
}

uint32_t SerializeSystem::Instantiate(const std::string& filePath, uint32_t parentGameObjectId)
{
	uint32_t id = DeserializePrefab(filePath);

	assert(id != 0);

	ParentChildrenSystem(m_GameObjectManager).AddChild(parentGameObjectId, id);

	// GameProcess
	EventManager::PublishImmediate("OnStart", id);

	return id;
}

uint32_t SerializeSystem::Instantiate(const std::string& filePath, uint32_t parentGameObjectId, bool instantiateInWorldSpace)
{
	uint32_t id = DeserializePrefab(filePath);

	assert(id != 0);

	ParentChildrenSystem(m_GameObjectManager).AddChild(parentGameObjectId, id);

	if (instantiateInWorldSpace)
	{
		Transform* transformParent = m_GameObjectManager->QueryComponent<Transform>(parentGameObjectId);
		Transform* transformChild = m_GameObjectManager->QueryComponent<Transform>(id);
		VSM::Matrix localMatrix = VSM::Matrix::CreateScale(transformChild->LocalScale) * VSM::Matrix::CreateFromQuaternion(transformChild->LocalRotation) * VSM::Matrix::CreateTranslation(transformChild->LocalPosition);
		localMatrix = localMatrix * transformParent->WorldMatrix.Invert();
		VSM::Vector3 scale, translation;
		VSM::Quaternion rotation;
		localMatrix.Decompose(scale, rotation, translation);
		transformChild->LocalScale = scale;
		transformChild->LocalPosition = translation;
		transformChild->LocalRotation = rotation;
	}

	EventManager::PublishImmediate("OnStart", id);

	return id;
}

uint32_t SerializeSystem::Instantiate(const std::string& filePath, Vector3 position, Quaternion rotation)
{
	uint32_t id = DeserializePrefab(filePath);
	assert(id != 0);

	Transform* transform = m_GameObjectManager->QueryComponent<Transform>(id);

	transform->LocalPosition = position;
	transform->LocalRotation = rotation;


	EventManager::PublishImmediate("OnStart", id);

	return id;
}

uint32_t SerializeSystem::Instantiate(const std::string& filePath, Vector3 position, Quaternion rotation, uint32_t parentGameObjectId)
{
	uint32_t id = DeserializePrefab(filePath);
	assert(id != 0);

	Transform* transform = m_GameObjectManager->QueryComponent<Transform>(id);


	ParentChildrenSystem(m_GameObjectManager).AddChild(parentGameObjectId, id);

	transform->LocalPosition = position;
	transform->LocalRotation = rotation;

	EventManager::PublishImmediate("OnStart", id);

	return id;
}

