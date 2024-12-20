#include "pch.h"
#include "CheckTutorialClear.h"

#include "Character.h"
#include "Tag.h"
#include "OnOff.h"
#include "../SoundEngine/ISoundEngine.h"

CheckTutorialClear::CheckTutorialClear(GameObjectManager* goManager)
	: TickTockScene(eSceneState::CheckTutorialClear, goManager)
{
	EventManager::Subscribe("OnFinishReplay", CreateListenerInfo(&CheckTutorialClear::OnFinishReplay));
	EventManager::Subscribe("OnClearAllMission", CreateListenerInfo(&CheckTutorialClear::OnClearAllMission));
	EventManager::Subscribe("OnFailMission", CreateListenerInfo(&CheckTutorialClear::OnFailMission));

	EventManager::Subscribe("OnReplayBtnClick", CreateListenerInfo(&CheckTutorialClear::OnReplayBtnClick));
	EventManager::Subscribe("OnNextStageBtnClick", CreateListenerInfo(&CheckTutorialClear::OnNextBtnClick));
	EventManager::Subscribe("OnLobbyBtnClick", CreateListenerInfo(&CheckTutorialClear::OnLobbyBtnClick));
	EventManager::Subscribe("OnResetStage", CreateListenerInfo(&CheckTutorialClear::OnResetStage));
	EventManager::Subscribe("OnTransitionStageOver", CreateListenerInfo(&CheckTutorialClear::OnTransitionStageOver));
}

void CheckTutorialClear::Enter()
{
	mCharacterId = 0;
	mbIsActivated = true;
	mbIsFinished = false;
	mbIsChecked = false;

	uint32_t count = 0;

	for (Character& character : System::ComponentItr<Character>())
	{
		mCharacterId = character.GameObjectId;
		count++;
	}

	mGameObjectManager->QueryComponent<OnOff>(mCharacterId)->IsOn = true;

	FindTimerID(&mDigitCounterId10, &mDigitCounterId01, &mDigitCounterId001, &mDigitCounterId0001, &mDigitCounterColonId);

	Replayer* replayer = mGameObjectManager->QueryComponent<Replayer>(mCharacterId);

	if (mDigitCounterId10 != 0 && mDigitCounterId01 != 0 && mDigitCounterId001 != 0 && mDigitCounterId0001 != 0 && mDigitCounterColonId != 0 && replayer != nullptr)
	{
		ShowTimer(replayer, mDigitCounterId10, mDigitCounterId01, mDigitCounterId001, mDigitCounterId0001, mDigitCounterColonId);
	}

	// 씬에 캐릭터가 둘 이상이거나 존재하지 않을 경우
	assert(count == 1);
	EventManager::ScheduleEvent("OnStartReplay", mCharacterId);
}

eSceneState CheckTutorialClear::HandleTransition()
{
	if (mbIsRetryBtnClicked)
	{
		return eSceneState::TutorialPlayer;
	}
	if (mbIsResetStage)
	{
		return eSceneState::TutorialPlayer;
	}
	if (mbIsStageOver)
	{
		return eSceneState::StageOver;
	}

	return GetGameState();
}

void CheckTutorialClear::Update(float deltaTime)
{
	if (!mbIsChecked && mbIsFinished)
	{
		mbIsChecked = true;
		EventManager::ScheduleEvent("OnCheckMissionClear");
	}

	ActiveTimer(mDigitCounterId10, mDigitCounterId01, mDigitCounterId001, mDigitCounterId0001);
}

void CheckTutorialClear::Exit()
{
	mCharacterId = 0;
	mbIsActivated = false;
	mbIsFinished = false;
	mbIsRetryBtnClicked = false;
	mbIsResetStage = false;
	mbIsStageOver = false;
	mbIsChecked = false;

	if (mDigitCounterId10 != 0 && mDigitCounterId01 != 0 && mDigitCounterId001 != 0 && mDigitCounterId0001 != 0 && mDigitCounterColonId != 0)
	{
		InActiveTimer(mDigitCounterId10, mDigitCounterId01, mDigitCounterId001, mDigitCounterId0001, mDigitCounterColonId);
	}

	std::set<std::string> inactiveUUIDs;
	inactiveUUIDs.insert({ "MissionClearUI" });
	inactiveUUIDs.insert({ "NextStageBtn" });
	inactiveUUIDs.insert({ "ReplayBtn" });
	inactiveUUIDs.insert({ "LobbyBtn" });
	SetEntityIdentityNameOnOffs(mGameObjectManager, inactiveUUIDs, false);

}

void CheckTutorialClear::OnFinishReplay(std::any data)
{
	uint32_t id = std::any_cast<uint32_t>(data);

	if (id == mCharacterId)
		mbIsFinished = true;
}

void CheckTutorialClear::OnClearAllMission(std::any data)
{
	if (!mbIsActivated)
		return;

	// UI 활성화
	std::set<std::string> activeUUIDs;
	activeUUIDs.insert({ "MissionClearUI" });
	activeUUIDs.insert({ "NextStageBtn" });
	activeUUIDs.insert({ "LobbyBtn" });
	SetEntityIdentityNameOnOffs(mGameObjectManager, activeUUIDs, true);

	// why?
	for (Character& character : System::ComponentItr<Character>())
	{
		character.IsFixable = true;
	}


	EventManager::ScheduleEvent("OnTransitionStageOver", {}, 3.f);
	Sound::GetInstance()->Play(0, "SFX_ClearNotice");


}

void CheckTutorialClear::OnFailMission(std::any data)
{
	if (!mbIsActivated)
		return;

	// UI 활성화
	std::set<std::string> activeUUIDs;
	activeUUIDs.insert({ "MissionClearUI" });
	activeUUIDs.insert({ "ReplayBtn" });
	activeUUIDs.insert({ "LobbyBtn" });
	SetEntityIdentityNameOnOffs(mGameObjectManager, activeUUIDs, true);

	EventManager::ScheduleEvent("OnTransitionStageOver", {}, 3.f);
	Sound::GetInstance()->Play(0, "SFX_FailNotice");

}

void CheckTutorialClear::OnReplayBtnClick(std::any data)
{
	if (!mbIsActivated)
		return;

	mbIsRetryBtnClicked = true;
}

void CheckTutorialClear::OnNextBtnClick(std::any data)
{
	if (!mbIsActivated)
		return;

	std::string nextSceneName = std::any_cast<std::string>(data);
	EventManager::ScheduleEvent("OnSceneTransition", nextSceneName);
}

void CheckTutorialClear::OnLobbyBtnClick(std::any data)
{
	if (!mbIsActivated)
		return;

	std::string lobbySceneName = std::any_cast<std::string>(data);
	EventManager::ScheduleEvent("OnSceneTransition", lobbySceneName);
}

void CheckTutorialClear::OnResetStage(std::any data)
{
	if (!mbIsActivated)
		return;

	mbIsResetStage = true;
}

void CheckTutorialClear::OnTransitionStageOver(std::any data)
{
	if (!mbIsActivated)
		return;

	mbIsStageOver = true;
}