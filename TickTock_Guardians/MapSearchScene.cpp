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

	// �� ���δ� �������� ���� ���̹Ƿ� �� �̸��� �� �� ���µ� ��ɷ� �߰��� ����ä�� �� �̸��� �����Ѵ�.
	EventManager::Subscribe("OnSceneTransition", CreateListenerInfo(&MapSearchScene::OnSceneTransition));
	EventManager::Subscribe("OnEnterLevelSelect", CreateListenerInfo(&MapSearchScene::OnEnterLevelSelect));


	// ������ ����ĳó ���� ��ø���������Ѵ�.

	// ����æ ������ ��Ʈ������ �̺�Ʈ�� 5�� ���� ������ �༭ �Ǵ�.

	// ���⼭ ui�� ��� ó���Ѵ�.



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
		// ī���� ã��
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

	// Ž�� �Ϸ� �� Enter UI Push ����
}

eSceneState MapSearchScene::HandleTransition()
{
	// �ƹ� Ű�� �Է����� ��
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
		// ī���� ã��
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
	// �� ���� �߻��ؾ� �ϳ� 

	// currentScene�� �ƴ����� �ν��Ͻ��� ����ֱ� ������ �̺�Ʈ�� ������ ���� ���� �־�

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

	/// �����ð� ������ �� ���� �� ����
}

void MapSearchScene::OnEnterLevelSelect(std::any data)
{
	mbEyeCatchUsed = false;
}

// ���� ������ ������
void MapSearchScene::OnDelayedFinalize(std::any data)
{
	mbEyeCatchUsed = false;
}