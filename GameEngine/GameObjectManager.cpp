#include "pch.h"
#include "GameObjectManager.h"
#include "GameObject.h"
#include "Component.h"
#include "Transform.h"
#include "EventManager.h"
#include "Input.h"
#include "OnOff.h"
#include "TransformSystem.h"

GameObjectManager::GameObjectManager()
{
	EventManager::Subscribe("OnDeleteGameObject", CreateListenerInfo(&GameObjectManager::OnReleaseGameObject));
	EventManager::Subscribe("OnDeleteComponent", CreateListenerInfo(&GameObjectManager::OnRemoveComponent));
}

[[nodiscard]]
GameObject* GameObjectManager::CreateGameObject(std::string_view name)
{
	CreateGameObject(name, 0);
	m_NextGameObjectIndex++;

	return &m_GameObjects.back();
}

GameObject* GameObjectManager::CreateGameObject(std::string_view name, uint32_t prefabNum)
{
	m_GameObjects.emplace_back(name, m_NextGameObjectIndex + prefabNum);

	m_AttachedComponents.emplace(m_GameObjects.back().Id, ComponentMap{});

	AddComponent<Transform>(m_GameObjects.back().Id);
	AddComponent<OnOff>(m_GameObjects.back().Id);

	return &m_GameObjects.back();
}



void GameObjectManager::ReleaseGameObject(GameObject* gameObject)
{
	ReleaseGameObject(gameObject->Id);
}

void GameObjectManager::ReleaseGameObject(uint32_t gameObjectId)
{
	if (!m_AttachedComponents.contains(gameObjectId))
		return;


	if (auto childrenCp = QueryComponent<Children>(gameObjectId))
	{
		for (auto child : childrenCp->ChildrenID)
		{
			ReleaseGameObject(child);
		}
	}

	ComponentMap& components = m_AttachedComponents.find(gameObjectId)->second;

	for (auto& component : components)
	{
		// 교체작업 진행
		Component* componentPtr = component.second;
		auto uid = componentPtr->GetUid();
		uint32_t originId = ComponentPoolManager::Release(componentPtr);
		if (originId == 0 ) continue;
		auto iter = m_AttachedComponents.find(originId);
		if (iter == m_AttachedComponents.end()) continue;
		auto originComponentIter = iter->second.find(uid);
		originComponentIter->second = componentPtr;
	}
	m_AttachedComponents.erase(gameObjectId);


	m_GameObjects.erase(std::remove_if(m_GameObjects.begin(), m_GameObjects.end(),
		[gameObjectId](const GameObject& go)
		{
			return go.Id == gameObjectId;
		})
		, m_GameObjects.end());
}

void GameObjectManager::RemoveComponent(uint32_t gameObjectId, Component* component)
{
	if (!m_AttachedComponents.contains(gameObjectId))
		return;

	ComponentMap& components = m_AttachedComponents.find(gameObjectId)->second;

	auto uid = component->GetUid();
	auto it = components.find(uid);
	if (it == components.end())
		return;

	auto ret = ComponentPoolManager::Release(it->second);

	auto& componentsToChange = m_AttachedComponents[ret];
	auto itToChange = componentsToChange.find(uid);

	if (itToChange != componentsToChange.end())
	{
		itToChange->second = it->second;
	}
	components.erase(it);
}

void GameObjectManager::RemoveComponent(Component* component)
{
	RemoveComponent(component->GameObjectId, component);
}

void GameObjectManager::Reset()
{
	m_GameObjects.clear();
	m_AttachedComponents.clear();
	m_NextGameObjectIndex = 1;
}

void GameObjectManager::OnReleaseGameObject(std::any objectNum)
{
	uint32_t goNum = std::any_cast<uint32_t>(objectNum);

	auto parent = QueryComponent<Parent>(goNum);
	if (parent)
	{
		auto children = QueryComponent<Children>(parent->ParentID);
		children->ChildrenID.remove_if(
			[goNum](uint32_t id)
			{
				return id == goNum;
			}
		);

		if (children->ChildrenID.empty())
		{
			RemoveComponent(children);
		}
	}
	ReleaseGameObject(goNum);
}

void GameObjectManager::OnRemoveComponent(std::any componentPtr)
{
	Component* cpPtr = std::any_cast<Component*>(componentPtr);
	RemoveComponent(cpPtr);
}
