#include "pch.h"
#include "StageOver.h"
#include "InGameManager.h"
#include "SerializeSystem.h"
#include "EventManager.h"
#include "OnOff.h"
#include "SceneComponent.h"
#include "SkyBoxRenderer.h"
#include "../SoundEngine/ISoundEngine.h"

StageOver::StageOver(GameObjectManager* gameObjectManager)
	: TickTockScene(eSceneState::StageOver, gameObjectManager)
{
	// 이벤트 구독
}

void StageOver::Enter()
{
	mbIsActivated = true;

	for (SceneComponent& scene : System::ComponentItr<SceneComponent>())
	{
		CurrentSceneName = scene.SceneName;
	}

	for (InGameManager& inGameManager : System::ComponentItr<InGameManager>())
	{
		mbIsCleared = inGameManager.Sibling<InGameManager>(mGameObjectManager)->IsMissionClear;
	}

	if (mbIsCleared)
	{
		Sound::GetInstance()->Play(0, "SFX_FailNotice");
		SerializeSystem(mGameObjectManager).Instantiate("../Data/Prefab/StageOverClear.prefab");
	}
	else
	{
		Sound::GetInstance()->Play(0, "SFX_ClearNotice");
		SerializeSystem(mGameObjectManager).Instantiate("../Data/Prefab/StageOverFail.prefab");
	}
}

eSceneState StageOver::HandleTransition()
{
	return GetGameState();
}

void StageOver::Update(float deltaTime)
{

}

void StageOver::Exit()
{
	mbIsActivated = false;
}

