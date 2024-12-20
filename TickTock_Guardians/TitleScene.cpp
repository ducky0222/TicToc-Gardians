#include "pch.h"
#include "TitleScene.h"
#include "Input.h"
#include "GameObjectManager.h"
#include "ComponentCommon.h"
#include "TickTockComponentCommon.h"

#include "../SoundEngine/ISoundEngine.h"

TitleScene::TitleScene(GameObjectManager* gameObjectManager)
	: TickTockScene(eSceneState::Title, gameObjectManager)
{
}

void TitleScene::Enter()
{
	minputlimitDuration = INIT_INPUT_LIMIT_DURATION;
	Sound::GetInstance()->Play(0, "BGM_Main");

	uint32_t entityID;

	if (GetEntityIdentityNameID(mGameObjectManager, "UIAnyKeyInput", &entityID))
	{
		UIController* controller = mGameObjectManager->QueryComponent<UIController>(entityID);

		if (controller != nullptr)
		{
			controller->bIsReset = true;
			controller->bIsActive = false;
		}
	}
}

eSceneState TitleScene::HandleTransition()
{
	if (minputlimitDuration < 0.f && Input::IsAnyKeyDown())
	{
		EventManager::ScheduleEvent("OnSceneTransition", std::string("LevelSelectHong.scene"));
		Sound::GetInstance()->Play(0, "SFX_UI_Select_Click");
	}

	return GetGameState();
}

void TitleScene::Update(float deltaTime)
{
	minputlimitDuration -= deltaTime;

	if (minputlimitDuration < 0)
	{
		uint32_t entityID;

		if (GetEntityIdentityNameID(mGameObjectManager, "UIAnyKeyInput", &entityID))
		{
			UIController* controller = mGameObjectManager->QueryComponent<UIController>(entityID);

			if (controller != nullptr)
			{
				controller->bIsActive = true;
			}
		}
	}
}

void TitleScene::Exit()
{
}
