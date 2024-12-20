#pragma once
#include "GameObject.h"
#include "ComponentPoolManager.h"
#include "EventListener.h"

/// <summary>
/// 2024.01.39 ��â��
/// ���� ������Ʈ admin�̶�� �����ϴ� Ŭ����
/// ���ӿ�����Ʈ�� ���ư��� �����Ƿ� list�� �����Ѵ�. 
/// </summary>

class GameObjectManager : public EventListener
{
	using ComponentMap = std::map<uint32_t, Component*>;

public:
	GameObjectManager();
	~GameObjectManager() = default;

	GameObject* CreateGameObject(std::string_view name);

	GameObject* CreateGameObject(std::string_view name, uint32_t prefabNum);

	template <typename T> requires IsComponent<T>
	inline T* AddComponent(uint32_t gameObjectId);

	template <typename T> requires IsComponent<T>
	inline T* AddComponent(GameObject* gameObject) { return AddComponent<T>(gameObject->Id); }

	template <typename T> requires IsComponent<T>
	inline T* QueryComponent(uint32_t gameObjectId);

	template <typename T> requires IsComponent<T>
	inline T* QueryComponent(GameObject* gameObject) { return QueryComponent<T>(gameObject->Id); }

	size_t GetGameObjectCount() const { return m_GameObjects.size(); }

	GameObject* GetGameObject(uint32_t gameObjectId)
	{
		auto it = std::find_if(m_GameObjects.begin(), m_GameObjects.end(), [gameObjectId](const GameObject& go) { return go.Id == gameObjectId; });
		if (it == m_GameObjects.end())
			return nullptr;

		return &(*it);
	}

	std::string_view GetGameObjectName(uint32_t gameObjectId)
	{
		auto it = std::find_if(m_GameObjects.begin(), m_GameObjects.end(), [gameObjectId](const GameObject& go) { return go.Id == gameObjectId; });
		if (it == m_GameObjects.end())
			return "";

		return it->Name;
	}

	ComponentMap* GetAttachedComponents(uint32_t gameObjectId)
	{
		auto it = m_AttachedComponents.find(gameObjectId);

		if (it == m_AttachedComponents.end())
			return nullptr;

		return &it->second;
	}

	uint32_t FindLargestId() const
	{
		if (m_GameObjects.empty()) {
			return 0;
		}

		auto maxElement = std::max_element(
			m_GameObjects.begin(),
			m_GameObjects.end(),
			[](const GameObject& obj1, const GameObject& obj2) {
				return obj1.Id < obj2.Id;
			}
		);

		return maxElement->Id;
	}

	void Reset();

	uint32_t GetNextGameObjectIndex() const { return m_NextGameObjectIndex; }

	void OnReleaseGameObject(std::any objectNum);
	void OnRemoveComponent(std::any componentPtr);

private:
	void ReleaseGameObject(GameObject* gameObject);
	void ReleaseGameObject(uint32_t gameObjectId);

	template <typename T> requires IsComponent<T>
	inline void RemoveComponent(uint32_t gameObjectId);
	void RemoveComponent(uint32_t gameObjectId, Component* component);
	void RemoveComponent(Component* component);


private:
	std::list<GameObject> m_GameObjects;

	/// < ���ӿ�����Ʈ id, < ������Ʈ id, ������Ʈ > >
	std::map<uint32_t, ComponentMap> m_AttachedComponents;
	uint32_t m_NextGameObjectIndex = 1;

	friend class SerializeSystem;
	friend class TransformSystem;
};


template <typename T> requires IsComponent<T>
inline T* GameObjectManager::AddComponent(uint32_t gameObjectId)
{
	// ���� ������Ʈ nullptr üũ
	if (gameObjectId == 0)
		return nullptr;

	auto findGoIt = std::find_if(m_GameObjects.begin(), m_GameObjects.end(),
		[gameObjectId](GameObject& go)
		{
			return go.Id == gameObjectId;
		});

	assert(findGoIt != m_GameObjects.end());

	// �̹� ������Ʈ�� �����ϴ��� üũ
	auto& components = m_AttachedComponents[gameObjectId];
	auto it = components.find(T::GetStaticUid());
	if (it != components.end())
		return static_cast<T*>(it->second);

	// ������Ʈ ��������
	T* component = ComponentPoolManager::Get<T>();
	component->GameObjectId = gameObjectId;
	components[T::GetStaticUid()] = component;

	return component;
}

template <typename T> requires IsComponent<T>
inline T* GameObjectManager::QueryComponent(uint32_t gameObjectId)
{
	// ������Ʈ ��������
	auto& components = m_AttachedComponents[gameObjectId];
	auto it = components.find(T::GetStaticUid());
	if (it == components.end())
		return nullptr;

	return static_cast<T*>(it->second);
}

template <typename T> requires IsComponent<T>
void GameObjectManager::RemoveComponent(uint32_t gameObjectId)
{
	// ������Ʈ ��������
	auto& components = m_AttachedComponents[gameObjectId];
	auto it = components.find(T::GetStaticUid());
	if (it == components.end())
		return;

	// ������Ʈ ����
	auto ret = ComponentPoolManager::Release(it->second);

	auto& componentsToChange = m_AttachedComponents[ret];
	auto itToChange = componentsToChange.find(T::GetStaticUid());

	if (itToChange != componentsToChange.end())
	{
		itToChange->second = it->second;
	}
	components.erase(it);
}

