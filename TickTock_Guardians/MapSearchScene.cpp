#include "pch.h"
#include "MapSearchScene.h"
#include "Input.h"
#include "SerializeSystem.h"
#include "GameObjectManager.h"
#include "ComponentCommon.h"
#include "TickTockComponentCommon.h"
#include "InGameManager.h"
#include "../SoundEngine/ISoundEngine.h"

#include <random>
#include <ranges>




MapSearchScene::MapSearchScene(GameObjectManager* gameObjectManager)
	: TickTockScene(eSceneState::MapSearchScene, gameObjectManager)
{
	EventManager::Subscribe("OnCutSceneOut", CreateListenerInfo(&MapSearchScene::OnCutSceneOut));
	EventManager::Subscribe("OnEnterUIAppear", CreateListenerInfo(&MapSearchScene::OnEnterUIAppear));

	// 이 내부는 로직만을 위한 씬이므로 씬 이름을 알 수 없는데 요걸로 중간에 가로채서 씬 이름을 저장한다.
	EventManager::Subscribe("OnSceneTransition", CreateListenerInfo(&MapSearchScene::OnSceneTransition));
	EventManager::Subscribe("OnEnterLevelSelect", CreateListenerInfo(&MapSearchScene::OnEnterLevelSelect));


	// 랜덤한 아이캐처 씬을 디시리얼라이즈한다.

	// 가로챈 씬으로 씬트랜지션 이벤트를 5초 정도 딜레이 줘서 건다.

	// 여기서 ui를 어떻게 처리한다.



}

void MapSearchScene::Enter()
{
	Sound::GetInstance()->Stop(0, "BGM_Main");
	Sound::GetInstance()->Play(0, "BGM_Ingame");

	struct InGameCutScene
	{
		std::string CharacterPath;
		std::string BackgroundPath;
	};

	EventManager::OnLoadScene(curSceneName);
	EventManager::ScheduleEvent("OnCutSceneOut", {}, mCutSceneDuration);
	mbIsActiveEvent = true;
	SetEntityIdentityNameOnOff(mGameObjectManager, "PleasePressAnyKey", true);

	for (EntityIdentity& entityIdentity : System::ComponentItr<EntityIdentity>())
	{
		// 카운터 찾기
		if (entityIdentity.EntityUUID == "ResetBtn")
		{
			entityIdentity.Sibling<OnOff>(mGameObjectManager)->IsOn = false;
		}
		else if (entityIdentity.EntityUUID == "SkipBtn")
		{
			entityIdentity.Sibling<OnOff>(mGameObjectManager)->IsOn = false;
		}
		else if (entityIdentity.EntityUUID == "UndoBtn")
		{
			entityIdentity.Sibling<OnOff>(mGameObjectManager)->IsOn = false;
		}
	}


	for(InGameManager& manager : System::ComponentItr<InGameManager>())
	{
		manager.IsResetAble = false;
	}

	// 탐색 완료 시 Enter UI Push 띄우기
}

eSceneState MapSearchScene::HandleTransition()
{
	// 아무 키나 입력했을 때
	if (Input::IsAnyKeyDown())
	{
		std::string sceneName = "CharacterSelect.scene";
		EventManager::ScheduleEvent("OnSceneTransition", sceneName);
		return eSceneState::CharacterSelect;
	}

	return GetGameState();
}

void MapSearchScene::Update(float deltaTime)
{

}

void MapSearchScene::Exit()
{

	for (EntityIdentity& entityIdentity : System::ComponentItr<EntityIdentity>())
	{
		// 카운터 찾기
		if (entityIdentity.EntityUUID == "ResetBtn")
		{
			entityIdentity.Sibling<OnOff>(mGameObjectManager)->IsOn = true;
		}
		else if (entityIdentity.EntityUUID == "SkipBtn")
		{
			entityIdentity.Sibling<OnOff>(mGameObjectManager)->IsOn = true;
		}
		else if (entityIdentity.EntityUUID == "UndoBtn")
		{
			entityIdentity.Sibling<OnOff>(mGameObjectManager)->IsOn = true;
		}
	}
	mGameObjectManager->OnReleaseGameObject(mCutSceneCharacterId);
	mGameObjectManager->OnReleaseGameObject(mCutSceneBackgroundId);
	SetEntityIdentityNameOnOff(mGameObjectManager, "UIEnter", false);
	mbIsActiveEvent = false;

}

void MapSearchScene::OnCutSceneOut(std::any data)
{
	//auto* charEntity = mGameObjectManager->GetGameObject(mCutSceneCharacterId);
	//auto* backEntity = mGameObjectManager->GetGameObject(mCutSceneBackgroundId);
	//
	//mGameObjectManager->QueryComponent<NeoTransformAnim3D>(charEntity)->bIsActive = true;
	//mGameObjectManager->QueryComponent<NeoTransformAnim2D>(backEntity)->bIsActive = true;
	//
	//EventManager::ScheduleEvent("OnEnterUIAppear", {}, mCutSceneDuration);
}

void MapSearchScene::OnEnterUIAppear(std::any data)
{
	SetEntityIdentityNameOnOff(mGameObjectManager, "UIEnter", true);
}

void MapSearchScene::OnSceneTransition(std::any data)
{
	// 한 번만 발생해야 하네 

	// currentScene은 아니지만 인스턴스가 살아있기 때문에 이벤트를 언제나 받을 수가 있어

	if (!mbIsActiveEvent)
	{
		return;
	}

	if (!mbEyeCatchUsed)
	{
		mbEyeCatchUsed = true;
		std::string sceneName = std::any_cast<std::string>(data);
		EventManager::ScheduleEvent("OnMapSearchSceneTransition", sceneName, 3.0f);


		static std::random_device rd;
		static std::mt19937 gen(rd());
		static std::uniform_int_distribution<int> distrib(0, 2);
		auto randomNum = std::views::iota(0, 3) | std::views::transform([&](auto) {return distrib(gen); });
		std::vector<std::pair<std::string, std::string>> eyeCatches = { {"EyeCatchBeaver.scene","EyeCatchBeaver"},{"EyeCatchCat.scene","EyeCatchCat"},{"EyeCatchRabbit.scene","EyeCatchRabbit"}};
		EventManager::PublishImmediate("OnSceneTransition", eyeCatches[randomNum.front()].first);
		Sound::GetInstance()->Play(0, eyeCatches[randomNum.front()].second);
	}

	/// 일정시간 딜레이 후 원래 씬 연결
}

void MapSearchScene::OnEnterLevelSelect(std::any data)
{
	mbEyeCatchUsed = false;
}

// 여긴 딜레이 넣을것
void MapSearchScene::OnDelayedFinalize(std::any data)
{
	mbEyeCatchUsed = false;
}