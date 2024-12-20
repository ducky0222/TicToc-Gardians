#include "pch.h"
#include "InGameManagerSystem.h"
#include "InGameManager.h"
#include "Input.h"
#include "EventManager.h"
#include "SerializeSystem.h"
#include "HysxEngine.h"
#include "SceneComponent.h"
#include "../SoundEngine/ISoundEngine.h"

InGameManagerSystem::InGameManagerSystem(GameObjectManager* gameObjManager)
	: System(gameObjManager)
{
	EventManager::Subscribe("OnPause", CreateListenerInfo(&InGameManagerSystem::OnPause));
	EventManager::Subscribe("OnResume", CreateListenerInfo(&InGameManagerSystem::OnResume));
	EventManager::Subscribe("OnLevelButtonClick", CreateListenerInfo(&InGameManagerSystem::OnLevelButtonClick));
	EventManager::Subscribe("OnClearAllMission", CreateListenerInfo(&InGameManagerSystem::OnClearAllMission));
	EventManager::Subscribe("OnResetStage", CreateListenerInfo(&InGameManagerSystem::OnResetStage));

	EventManager::Subscribe("OnQuit", CreateListenerInfo(&InGameManagerSystem::OnQuit)); // Ãß°¡: È«ÁöÈ¯
	EventManager::Subscribe("OnNextLevelButtonClick", CreateListenerInfo(&InGameManagerSystem::OnNextLevelButtonClick));
}

void InGameManagerSystem::Update(float deltaTime)
{
	for (InGameManager& inGameManager : ComponentItr<InGameManager>())
	{
		if (Input::GetKeyDown(KEY::NUM1))
		{
			Sound::GetInstance()->Play(0, "SFX_TimeRewinding_fix2");
			EventManager::ScheduleEvent("OnResetStage");
		}

		if (Input::GetKeyDown(KEY::ESC))
		{
			EventManager::ScheduleEvent("OnPause");
		}

	}
}

void InGameManagerSystem::Start(uint32_t gameObjectId)
{

}

void InGameManagerSystem::Initialize()
{
	for (InGameManager& inGameManager : ComponentItr<InGameManager>())
	{
		Physics::GetInstance()->SetGravity(inGameManager.GlobalGravity);
	}

}

void InGameManagerSystem::Finish(uint32_t gameObjectId)
{

}

void InGameManagerSystem::Finalize()
{

}

void InGameManagerSystem::OnPause(std::any data)
{
	for (InGameManager& inGameManager : ComponentItr<InGameManager>())
	{
		if (inGameManager.PausePopupId == 0)
		{
			// uint32_t id = SerializeSystem(m_GameObjectManager).Instantiate("../Data/prefab/UIPausePannel.prefab");
			uint32_t id = SerializeSystem(m_GameObjectManager).Instantiate("../Data/prefab/UIInGameConfig.prefab");
			inGameManager.PausePopupId = id;
		}
	}
}

void InGameManagerSystem::OnResume(std::any data)
{
	for (InGameManager& inGameManager : ComponentItr<InGameManager>())
	{
		if (inGameManager.PausePopupId != 0)
		{
			EventManager::OnDestroy(inGameManager.PausePopupId);
			inGameManager.PausePopupId = 0;
		}
	}
}


void InGameManagerSystem::OnLevelButtonClick(std::any data)
{
	EventManager::PublishImmediate("OnResume");

	Sound::GetInstance()->Stop(0, "BGM_Ingame");
	Sound::GetInstance()->Play(0, "BGM_Main");

	EventManager::ScheduleEvent("OnSceneTransition", data);
}

void InGameManagerSystem::OnNextLevelButtonClick(std::any data)
{
	for (InGameManager& inGameManager : ComponentItr<InGameManager>())
	{
		EventManager::ScheduleEvent("OnSceneTransition", inGameManager.LevelName);
	}
}

void InGameManagerSystem::OnClearAllMission(std::any daopo)
{
	for (InGameManager& inGameManager : ComponentItr<InGameManager>())
	{
		inGameManager.IsMissionClear = true;
	}
}

void InGameManagerSystem::OnNextLevelClick(std::any data)
{
	for (InGameManager& inGameManager : ComponentItr<InGameManager>())
	{
		EventManager::ScheduleEvent("OnSceneTransition", inGameManager.LevelName);

	}
}

void InGameManagerSystem::OnResetStage(std::any data)
{
	EventManager::PublishImmediate("OnResume");

	std::string sceneName;

	for (SceneComponent& scene : ComponentItr<SceneComponent>())
	{
		sceneName = scene.SceneName;
	}

	for (InGameManager& inGameManager : ComponentItr<InGameManager>())
	{


		if (inGameManager.IsResetAble)
		{
			EventManager::ScheduleEvent("OnEnterLevelSelect");
			EventManager::ScheduleEvent("OnMapSearchSceneTransition", sceneName);
		}
	}
}

void InGameManagerSystem::OnQuit(std::any data) // Ãß°¡: È«ÁöÈ¯
{
	exit(0);
}