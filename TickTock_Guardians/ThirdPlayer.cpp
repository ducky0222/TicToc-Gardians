#include "pch.h"
#include "ThirdPlayer.h"

#include "Character.h"
#include "GameObjectManager.h"
#include "Input.h"
#include "System.h"
#include "TickTockComponentCommon.h"
#include "EntityIdentity.h"
#include "UI2DRenderer.h"
#include "UIController.h"

ThirdPlayer::ThirdPlayer(GameObjectManager* gameObjectManager)
	: TickTockScene(eSceneState::ThirdPlayer, gameObjectManager)
{
	EventManager::Subscribe("OnFinishTracing", CreateListenerInfo(&ThirdPlayer::OnFinishTracing));
	EventManager::Subscribe("OnFinishReplay", CreateListenerInfo(&ThirdPlayer::OnFinishReplay));
	EventManager::Subscribe("SetCharacterIndex", CreateListenerInfo(&ThirdPlayer::SetCharacterIndex));
	EventManager::Subscribe("OnSkipTracing", CreateListenerInfo(&ThirdPlayer::OnSkipTracing));
}
void ThirdPlayer::Enter()
{
	mbIsActivated = true;

	FindTimerID(&mDigitCounterId10, &mDigitCounterId01, &mDigitCounterId001, &mDigitCounterId0001, &mDigitCounterColonId);

	for (EntityIdentity& entityIdentity : System::ComponentItr<EntityIdentity>())
	{
		// 초상화 찾기
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

	// 느리지만 관리 가능한 형태로 활성화 여부를 다룸
	// exit 시 반드시 비활성화 해야 함
	SetTagTypeOnOff(mGameObjectManager, TagType::Cat, true);
	SetTagTypeOnOff(mGameObjectManager, TagType::Beaver, true);
	SetTagTypeOnOff(mGameObjectManager, TagType::Rabbit, true);

	// 첫번째 캐릭터, 두번째, 세번쨰 캐릭터 찾기
	for (Character& character : System::ComponentItr<Character>())
	{
		Tag* tag = character.Sibling<Tag>(mGameObjectManager);
		assert(tag != nullptr);

		if (tag->tag == firstSelectType)
		{
			mCharacterId1 = character.GameObjectId;
			//EventManager::ScheduleEvent("OnStartReplay", mCharacterId1);

		}
		else if (tag->tag == secondSelectType)
		{
			mCharacterId2 = character.GameObjectId;
			//EventManager::ScheduleEvent("OnStartReplay", mCharacterId2);
		}
		else // thirdType
		{
			assert(tag->tag == thirdSelectType);
			mCharacterId3 = character.GameObjectId;
			//EventManager::ScheduleEvent("OnStartMoving", mCharacterId3);
		}
	}

	Replayer* replayer = mGameObjectManager->QueryComponent<Replayer>(mCharacterId3);

	if (mDigitCounterId10 != 0 && mDigitCounterId01 != 0 && mDigitCounterId001 != 0 && mDigitCounterId0001 != 0 && mDigitCounterColonId != 0 && replayer != nullptr)
	{
		ShowTimer(replayer, mDigitCounterId10, mDigitCounterId01, mDigitCounterId001, mDigitCounterId0001, mDigitCounterColonId);
	}

	/// TODO : 이거 나중에 타이머쪽 리셋 수정 필요함
	if (mPortraitId01 != 0 && mPortraitId02 != 0 && mPortraitId03 != 0)
	{
		// 디지털 카운터와 초상화를 활성화하고, 초상화에 초상화를 설정한다.
		mGameObjectManager->QueryComponent<OnOff>(mPortraitId01)->IsOn = true;
		mGameObjectManager->QueryComponent<OnOff>(mPortraitId02)->IsOn = true;
		mGameObjectManager->QueryComponent<OnOff>(mPortraitId03)->IsOn = true; mGameObjectManager->QueryComponent<OnOff>(mDigitCounterId0001)->IsOn = true;
		mGameObjectManager->QueryComponent<UI2DRenderer>(mPortraitId01)->FileName = MatchPortraitDim(firstSelectType);
		mGameObjectManager->QueryComponent<UI2DRenderer>(mPortraitId02)->FileName = MatchPortraitDim(secondSelectType);
		mGameObjectManager->QueryComponent<UI2DRenderer>(mPortraitId03)->FileName = MatchPortraitPlayer(thirdSelectType);
	}

	EventManager::ScheduleEvent("OnReady", mCharacterId3);
	EventManager::ScheduleEvent("OnSetCurrentSceneState", GetGameState());

	SetEntityIdentityNameOnOff(mGameObjectManager, "PleasePressAnyKey", false);

}
eSceneState ThirdPlayer::HandleTransition()
{
	if (mbIsUndo)
	{
		return eSceneState::SecondPlayer;
	}

	if (mbIsFinished1 && mbIsFinished2 && mbIsFinished3)
	{
		return eSceneState::CheckMissionClear;
	}

	if (mbIsResetStage)
	{
		return eSceneState::CharacterSelect;
	}

	return GetGameState();
}
void ThirdPlayer::Update(float deltaTime)
{
	if (!mbIsStart && Input::IsAnyKeyDown())
	{
		mbIsStart = true;
		EventManager::ScheduleEvent("OnStartReplay", mCharacterId1);
		EventManager::ScheduleEvent("OnStartReplay", mCharacterId2);
		EventManager::ScheduleEvent("OnStartMoving", mCharacterId3);
		ActiveTimer(mDigitCounterId10, mDigitCounterId01, mDigitCounterId001, mDigitCounterId0001);
	}

	if (Input::GetKeyDown(KEY::R))
	{
		// 이동 중 언두
		if (mbIsStart)
		{
			EventManager::PublishImmediate("OnUndoLandingPoint", GetGameState());
			EventManager::ScheduleEvent("OnStopReplay", mCharacterId1);
			EventManager::ScheduleEvent("OnStopReplay", mCharacterId2);
			EventManager::PublishImmediate("OnUndo", mCharacterId3);

			Exit();
			Enter();
		}
		else // 정지 중 언두
		{
			mbIsUndo = true;

			EventManager::PublishImmediate("OnUndoLandingPoint", eSceneState::SecondPlayer);
			EventManager::PublishImmediate("OnUndo", mCharacterId3);
		}
	}
}
void ThirdPlayer::Exit()
{
	mbIsActivated = false;

	for (EntityIdentity& entityIdentity : System::ComponentItr<EntityIdentity>())
	{
		if (entityIdentity.EntityUUID == "TimerUI")
		{
			entityIdentity.Sibling<OnOff>(mGameObjectManager)->IsOn = true;
		}
	}

	mCharacterId1 = 0;
	mCharacterId2 = 0;
	mCharacterId3 = 0;
	mbIsFinished1 = false;
	mbIsFinished2 = false;
	mbIsFinished3 = false;
	mbIsStart = false;
	mbIsUndo = false;

	SetTagTypeOnOff(mGameObjectManager, TagType::Cat, false);
	SetTagTypeOnOff(mGameObjectManager, TagType::Beaver, false);
	SetTagTypeOnOff(mGameObjectManager, TagType::Rabbit, false);

	for (Timer& timer : System::ComponentItr<Timer>())
	{
		timer.Sibling<OnOff>(mGameObjectManager)->IsOn = false;
	}

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

	EventManager::ScheduleEvent("OnResetScene");
}

void ThirdPlayer::OnFinishTracing(std::any data)
{
	if (!mbIsActivated)
		return;

	if (std::any_cast<uint32_t>(data) == mCharacterId3)
	{
		mbIsFinished3 = true;
	}
}

void ThirdPlayer::OnFinishReplay(std::any data)
{
	if (!mbIsActivated)
		return;

	auto id = std::any_cast<uint32_t>(data);

	if (mCharacterId1 == id)
	{
		mbIsFinished1 = true;
	}
	else if (mCharacterId2 == id)
	{
		mbIsFinished2 = true;
	}
}


void ThirdPlayer::SetCharacterIndex(std::any data)
{
	SelectTurn turn = std::any_cast<SelectTurn>(data);
	firstType = turn.first.first;
	secondType = turn.second.first;
	thirdType = turn.third.first;
}

void ThirdPlayer::OnSkipTracing(std::any data)
{
	if (!mbIsActivated)
		return;

	mbIsFinished1 = true;
	mbIsFinished2 = true;
	mbIsFinished3 = true;
}
