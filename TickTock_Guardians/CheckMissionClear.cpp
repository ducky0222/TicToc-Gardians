#include "pch.h"
#include "CheckMissionClear.h"

#include "Character.h"
#include "GameObjectManager.h"
#include "System.h"
#include "TickTockComponentCommon.h"
#include "ComponentCommon.h"
#include "DimensionCrack.h"
#include "../SoundEngine/ISoundEngine.h"

CheckMissionClear::CheckMissionClear(GameObjectManager* gameObjectManager)
	: TickTockScene(eSceneState::CheckMissionClear, gameObjectManager)
{
	EventManager::Subscribe("OnFinishReplay", CreateListenerInfo(&CheckMissionClear::OnFinishReplay));
	EventManager::Subscribe("OnClearAllMission", CreateListenerInfo(&CheckMissionClear::OnClearAllMission));
	EventManager::Subscribe("OnFailMission", CreateListenerInfo(&CheckMissionClear::OnFailMission));

	EventManager::Subscribe("OnReplayBtnClick", CreateListenerInfo(&CheckMissionClear::OnReplayBtnClick));
	EventManager::Subscribe("OnNextStageBtnClick", CreateListenerInfo(&CheckMissionClear::OnNextBtnClick));
	EventManager::Subscribe("OnLobbyBtnClick", CreateListenerInfo(&CheckMissionClear::OnLobbyBtnClick));

	EventManager::Subscribe("OnTransitionStageOver", CreateListenerInfo(&CheckMissionClear::OnTransitionStageOver));
	EventManager::Subscribe("SetCharacterIndex", CreateListenerInfo(&CheckMissionClear::SetCharacterIndex));
}

void CheckMissionClear::Enter()
{
	mbIsActivated = true;
	mbIsChecked = false;
	mbIsStageOver = false;

	FindTimerID(&mDigitCounterId10, &mDigitCounterId01, &mDigitCounterId001, &mDigitCounterId0001, &mDigitCounterColonId);

	for (EntityIdentity& entityIdentity : System::ComponentItr<EntityIdentity>())
	{
		// �ʻ�ȭ ã��
		if (entityIdentity.EntityUUID == "Portrait01")
		{
			mPortraitId01 = entityIdentity.GameObjectId;
		}
		else if (entityIdentity.EntityUUID == "Portrait02")
		{
			mPortraitId02 = entityIdentity.GameObjectId;
		}
		else if (entityIdentity.EntityUUID == "Portrait03")
		{
			mPortraitId03 = entityIdentity.GameObjectId;
		}
	}

	TagType firstSelectType = GetSelectTagMappingPlayerTag(firstType);
	TagType secondSelectType = GetSelectTagMappingPlayerTag(secondType);
	TagType thirdSelectType = GetSelectTagMappingPlayerTag(thirdType);

	// exit �� �ݵ�� ��Ȱ��ȭ ��������� ��
	SetTagTypeOnOff(mGameObjectManager, TagType::Cat, true);
	SetTagTypeOnOff(mGameObjectManager, TagType::Beaver, true);
	SetTagTypeOnOff(mGameObjectManager, TagType::Rabbit, true);

	for (DimensionCrack& dimension : System::ComponentItr<DimensionCrack>())
	{
		dimension.CharactersNearCrack = 0;
	}

	// ù��° ĳ����, �ι�°, ������ ĳ���� ã��
	for (Character& character : System::ComponentItr<Character>())
	{
		Tag* tag = character.Sibling<Tag>(mGameObjectManager);
		assert(tag != nullptr);

		if (tag->tag == firstSelectType)
		{
			mCharacterId1 = character.GameObjectId;
			EventManager::ScheduleEvent("OnStartReplay", mCharacterId1);

		}
		else if (tag->tag == secondSelectType)
		{
			mCharacterId2 = character.GameObjectId;
			EventManager::ScheduleEvent("OnStartReplay", mCharacterId2);
		}
		else // thirdType
		{
			assert(tag->tag == thirdSelectType);
			mCharacterId3 = character.GameObjectId;
			EventManager::ScheduleEvent("OnStartReplay", mCharacterId3);
		}
	}

	Replayer* replayer = mGameObjectManager->QueryComponent<Replayer>(mCharacterId3);

	if (mDigitCounterId10 != 0 && mDigitCounterId01 != 0 && mDigitCounterId001 != 0 && mDigitCounterId0001 != 0 && mDigitCounterColonId != 0 && replayer != nullptr)
	{
		ShowTimer(replayer, mDigitCounterId10, mDigitCounterId01, mDigitCounterId001, mDigitCounterId0001, mDigitCounterColonId);
	}

	/// TODO : �̰� ���߿� Ÿ�̸��� ���� ���� �ʿ���
	if (mPortraitId01 != 0 && mPortraitId02 != 0 && mPortraitId03 != 0)
	{
		// ������ ī���Ϳ� �ʻ�ȭ�� Ȱ��ȭ�ϰ�, �ʻ�ȭ�� �ʻ�ȭ�� �����Ѵ�.
		mGameObjectManager->QueryComponent<OnOff>(mPortraitId01)->IsOn = true;
		mGameObjectManager->QueryComponent<OnOff>(mPortraitId02)->IsOn = true;
		mGameObjectManager->QueryComponent<OnOff>(mPortraitId03)->IsOn = true;
		mGameObjectManager->QueryComponent<UI2DRenderer>(mPortraitId01)->FileName = MatchPortraitPlayer(firstSelectType);
		mGameObjectManager->QueryComponent<UI2DRenderer>(mPortraitId02)->FileName = MatchPortraitPlayer(secondSelectType);
		mGameObjectManager->QueryComponent<UI2DRenderer>(mPortraitId03)->FileName = MatchPortraitPlayer(thirdSelectType);
	}

	SetEntityIdentityNameOnOff(mGameObjectManager, "PleasePressAnyKey", false);

}
eSceneState CheckMissionClear::HandleTransition()
{
	if (mbIsRetryBtnClicked)
		return eSceneState::CharacterSelect;

	if (mbIsStageOver)
		return eSceneState::StageOver;


	return GetGameState();
}
void CheckMissionClear::Update(float deltaTime)
{
	// TODO : ���⼭ �� �������� �˻��ؼ� ������ ƴ�̶� �˻��ϸ� �ɵ�/
	if (!mbIsChecked && mbIsFinished1 && mbIsFinished2 && mbIsFinished3)
	{
		mbIsChecked = true;
		EventManager::ScheduleEvent("OnCheckMissionClear");
	}

	ActiveTimer(mDigitCounterId10, mDigitCounterId01, mDigitCounterId001, mDigitCounterId0001);
}
void CheckMissionClear::Exit()
{
	mbIsActivated = false;

	mbIsFinished1 = false;
	mbIsFinished2 = false;
	mbIsFinished3 = false;

	mbIsChecked = false;
	mbIsRetryBtnClicked = false;
	mbIsStageOver = false;

	SetTagTypeOnOff(mGameObjectManager, TagType::Cat, false);
	SetTagTypeOnOff(mGameObjectManager, TagType::Beaver, false);
	SetTagTypeOnOff(mGameObjectManager, TagType::Rabbit, false);

	// �ּ�ó�� : ȫ��ȯ, �ش� ������ ��û���� ���� ��� ����
	//std::set<std::string> inactiveUUIDs;
	//
	//inactiveUUIDs.insert({ "MissionClearUI" });
	//
	//SetEntityIdentityNameOnOffs(mGameObjectManager, inactiveUUIDs, false);

	if (mDigitCounterId10 != 0 && mDigitCounterId01 != 0 && mDigitCounterId001 != 0 && mDigitCounterId0001 != 0 && mDigitCounterColonId != 0)
	{
		InActiveTimer(mDigitCounterId10, mDigitCounterId01, mDigitCounterId001, mDigitCounterId0001, mDigitCounterColonId);
	}

	if (mPortraitId01 != 0 && mPortraitId02 != 0 && mPortraitId03 != 0)
	{
		mGameObjectManager->QueryComponent<OnOff>(mPortraitId01)->IsOn = false;
		mGameObjectManager->QueryComponent<OnOff>(mPortraitId02)->IsOn = false;
		mGameObjectManager->QueryComponent<OnOff>(mPortraitId03)->IsOn = false;
	}
}

void CheckMissionClear::OnFinishReplay(std::any data)
{
	if (!mbIsActivated)
		return;

	uint32_t gameObjectId = std::any_cast<uint32_t>(data);

	if (gameObjectId == mCharacterId1)
	{
		mbIsFinished1 = true;
	}
	else if (gameObjectId == mCharacterId2)
	{
		mbIsFinished2 = true;
	}
	else if (gameObjectId == mCharacterId3)
	{
		mbIsFinished3 = true;
	}
}

void CheckMissionClear::OnClearAllMission(std::any data)
{
	if (!mbIsActivated)
		return;

	// UI Ȱ��ȭ // �ּ�ó�� : ȫ��ȯ, �ش� ������ ��û���� ���� ��� ����
	//std::set<std::string> activeUUIDs;
	//
	//activeUUIDs.insert({ "MissionClearUI" });
	//
	//SetEntityIdentityNameOnOffs(mGameObjectManager, activeUUIDs, true);

	// why?
	for (Character& character : System::ComponentItr<Character>())
	{
		character.IsFixable = true;
	}

	EventManager::ScheduleEvent("OnTransitionStageOver", {}, SCENE_TRANSITION_DELAY);
	Sound::GetInstance()->Play(0, "SFX_ClearNotice");
}

void CheckMissionClear::OnFailMission(std::any data)
{
	if (!mbIsActivated)
		return;

	// UI Ȱ��ȭ
	//std::set<std::string> activeUUIDs;

	// activeUUIDs.insert({ "MissionFailUI" }); // �ּ�ó�� : ȫ��ȯ, �ش� ������ ��û���� ���� ��� ����

	//SetEntityIdentityNameOnOffs(mGameObjectManager, activeUUIDs, true);

	EventManager::ScheduleEvent("OnTransitionStageOver", {}, SCENE_TRANSITION_DELAY);
	Sound::GetInstance()->Play(0, "SFX_FailNotice");
}

void CheckMissionClear::OnReplayBtnClick(std::any data)
{
	if (!mbIsActivated)
		return;

	mbIsRetryBtnClicked = true;
}

void CheckMissionClear::OnNextBtnClick(std::any data)
{
	if (!mbIsActivated)
		return;

	std::string nextSceneName = std::any_cast<std::string>(data);
	EventManager::ScheduleEvent("OnMapSearchSceneTransition", nextSceneName);
}

void CheckMissionClear::OnLobbyBtnClick(std::any data)
{
	if (!mbIsActivated)
		return;

	std::string lobbySceneName = std::any_cast<std::string>(data);
	EventManager::ScheduleEvent("OnSceneTransition", lobbySceneName);
}

void CheckMissionClear::OnTransitionStageOver(std::any data)
{
	if (!mbIsActivated)
		return;

	mbIsStageOver = true;
}

void CheckMissionClear::SetCharacterIndex(std::any data)
{
	SelectTurn turn = std::any_cast<SelectTurn>(data);
	firstType = turn.first.first;
	secondType = turn.second.first;
	thirdType = turn.third.first;
}
