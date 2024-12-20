#pragma once
#include "SystemInterface.h"
#include "System.h"
#include "EventListener.h"

class System;
class GameObjectManager;
class Renderer;


class SystemManager : public EventListener
{
public:
	SystemManager()
	{
		EventManager::Subscribe("OnInitialize", CreateListenerInfo(&SystemManager::OnInitialize));
		EventManager::Subscribe("OnFinalize", CreateListenerInfo(&SystemManager::OnFinalize));
	}

	~SystemManager() = default;

	void OnInitialize(std::any data)
	{
		Initialize();
	}

	void OnFinalize(std::any data)
	{
		Finalize();
	}

	void Initialize(GameObjectManager* gom)
	{
		m_GameObjectManager = gom;
	}

	void Update(float deltaTime)
	{
		for (auto& updatable : m_Updatables)
		{
			updatable->Update(deltaTime);
		}
	}

	void FixedUpdate(float deltaTime)
	{
		m_ProgressedTime += deltaTime;

		while (m_ProgressedTime > 1.f / 60.f)
		{
			for (auto& fixedUpdatable : m_FixedUpdatables)
			{
				fixedUpdatable->FixedUpdate(1.f / 60.f);
			}

			m_ProgressedTime -= 1.f / 60.f;
		}
	}

	void Render()
	{
		for (auto& renderable : m_Renderables)
		{
			renderable->Render();
		}
	}

	void Initialize()
	{
		for (auto& startable : m_Startables)
		{
			startable->Initialize();
		}
	}

	void Start(uint32_t gameObjectId)
	{
		for (auto& startable : m_Startables)
		{
			startable->Start(gameObjectId);
		}
	}

	void Finish(uint32_t gameObjectId)
	{
		for (auto& startable : m_Startables)
		{
			startable->Finish(gameObjectId);
		}
	}

	void Finalize()
	{
		for (auto& startable : m_Startables)
		{
			startable->Finalize();
		}
	}


	template <typename T> requires std::is_base_of_v<System, T>
	T* AddSystem()
	{
		m_Systems.push_back(std::make_unique<T>(m_GameObjectManager));

		if constexpr (std::is_base_of_v<IUpdatable, T>)
		{
			m_Updatables.push_back(static_cast<T*>(m_Systems.back().get()));
		}

		if constexpr (std::is_base_of_v<IFixedUpdatable, T>)
		{
			m_FixedUpdatables.push_back(static_cast<T*>(m_Systems.back().get()));
		}

		if constexpr (std::is_base_of_v<IRenderable, T>)
		{
			m_Renderables.push_back(static_cast<T*>(m_Systems.back().get()));
		}

		if constexpr (std::is_base_of_v<IStartable, T>)
		{
			m_Startables.push_back(static_cast<T*>(m_Systems.back().get()));
		}

		return static_cast<T*>(m_Systems.back().get());
	}

	// 씬 Deserialize 용 시스템 추가 함수
	// std::move 로 인자 전달 필요
	void AddSystem(std::unique_ptr<System> system)
	{
		m_Systems.push_back(std::move(system));

		if (const auto ptr = dynamic_cast<IUpdatable*>(system.get()))
		{
			m_Updatables.push_back(ptr);
		}

		if (const auto ptr = dynamic_cast<IFixedUpdatable*>(system.get()))
		{
			m_FixedUpdatables.push_back(ptr);
		}

		if (const auto ptr = dynamic_cast<IRenderable*>(system.get()))
		{
			m_Renderables.push_back(ptr);
		}

		if (const auto ptr = dynamic_cast<IStartable*>(system.get()))
		{
			m_Startables.push_back(ptr);
		}
	}

private:
	GameObjectManager* m_GameObjectManager = nullptr;
	float m_ProgressedTime = 0.f;

	std::vector<std::unique_ptr<System>> m_Systems;
	std::vector<IUpdatable*> m_Updatables;
	std::vector<IFixedUpdatable*> m_FixedUpdatables;
	std::vector<IRenderable*> m_Renderables;

	std::vector<IStartable*> m_Startables;
};

