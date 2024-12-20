#include "pch.h"
#include "SecondPlayer.h"

#include "Character.h"
#include "GameObjectManager.h"
#include "Input.h"
#include "System.h"
#include "TickTockComponentCommon.h"
#include "EntityIdentity.h"
#include "UI2DRenderer.h"
#include "UIController.h"

SecondPlayer::SecondPlayer(GameObjectManager* gameObjectManager)
	: TickTockScene(eSceneState::SecondPlayer, gameObjectManager)
{
	EventManager::Subscribe("OnFinishTracing", CreateListenerInfo(&SecondPlayer::OnFinishTracing));
	EventManager::Subscribe("OnFinishReplay", CreateListenerInfo(&SecondPlayer::OnFinishReplay));
	EventManager::Subscribe("SetCharacterIndex", CreateListenerInfo(&SecondPlayer::SetCharacterIndex));
	EventManager::Subscribe("OnSkipTracing", CreateListenerInfo(&SecondPlayer::OnSkipTracing));
}

void SecondPlayer::Enter()
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


	// ente 시 활성화한 오브젝트는 반드시 exit에서 비활성화 해줘야 한다.
	SetTagTypeOnOff(mGameObjectManager, TagType::Cat, false);
	SetTagTypeOnOff(mGameObjectManager, TagType::Beaver, false);
	SetTagTypeOnOff(mGameObjectManager, TagType::Rabbit, false);
	SetTagTypeOnOff(mGameObjectManager, firstSelectType, true);
	SetTagTypeOnOff(mGameObjectManager, secondSelectType, true);

	// 첫번째 캐릭터, 두번째 캐릭터 id 찾기
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
			//EventManager::ScheduleEvent("OnStartMoving", mCharacterId2);
		}
	}

	Replayer* replayer = mGameObjectManager->QueryComponent<Replayer>(mCharacterId2);

	if (mDigitCounterId10 != 0 && mDigitCounterId01 != 0 && mDigitCounterId001 != 0 && mDigitCounterId0001 != 0 && mDigitCounterColonId != 0 && replayer != nullptr)
	{
		ShowTimer(replayer, mDigitCounterId10, mDigitCounterId01, mDigitCounterId001, mDigitCounterId0001, mDigitCounterColonId);
	}

	if (mPortraitId01 != 0 && mPortraitId02 != 0 && mPortraitId03 != 0)
	{
		// 디지털 카운터와 초상화를 활성화하고, 초상화에 초상화를 설정한다.
		mGameObjectManager->QueryComponent<OnOff>(mPortraitId01)->IsOn = true;
		mGameObjectManager->QueryComponent<OnOff>(mPortraitId02)->IsOn = true;
		mGameObjectManager->QueryComponent<OnOff>(mPortraitId03)->IsOn = true;
		mGameObjectManager->QueryComponent<UI2DRenderer>(mPortraitId01)->FileName = MatchPortraitDim(firstSelectType);
		mGameObjectManager->QueryComponent<UI2DRenderer>(mPortraitId02)->FileName = MatchPortraitPlayer(secondSelectType);
		mGameObjectManager->QueryComponent<UI2DRenderer>(mPortraitId03)->FileName = MatchPortraitDim(thirdSelectType);
	}


	EventManager::ScheduleEvent("OnReady", mCharacterId2);
	EventManager::ScheduleEvent("OnSetCurrentSceneState", GetGameState());

	SetEntityIdentityNameOnOff(mGameObjectManager, "PleasePressAnyKey", false);

	assert(mCharacterId1 != 0);
	assert(mCharacterId2 != 0);
}

eSceneState SecondPlayer::HandleTransition()
{
	if (mbIsUndo)
	{
		return eSceneState::FirstPlayer;
	}
	if (mbIsFinished1 && mbIsFinished2)
	{
		return eSceneState::ThirdPlayer;
	}

	return GetGameState();
}
void SecondPlayer::Update(float deltaTime)
{
	if (!mbIsStart && Input::IsAnyKeyDown())
	{
		mbIsStart = true;
		EventManager::ScheduleEvent("OnStartReplay", mCharacterId1);
		EventManager::ScheduleEvent("OnStartMoving", mCharacterId2);
		ActiveTimer(mDigitCounterId10, mDigitCounterId01, mDigitCounterId001, mDigitCounterId0001);
	}

	if (Input::GetKeyDown(KEY::R)) // 언두
	{
		// 이동 중 언두
		if (mbIsStart)
		{
			EventManager::PublishImmediate("OnUndoLandingPoint", GetGameState());
			EventManager::ScheduleEvent("OnStopReplay", mCharacterId1);
			EventManager::PublishImmediate("OnUndo", mCharacterId2);

			Exit();
			Enter();
		}
		else // 정지 중 언두
		{
			mbIsUndo = true;

			EventManager::PublishImmediate("OnUndoLandingPoint", eSceneState::FirstPlayer);
			EventManager::PublishImmediate("OnUndo", mCharacterId2);
		}
	}
}

void SecondPlayer::Exit()
{
	for (EntityIdentity& entityIdentity : System::ComponentItr<EntityIdentity>())
	{
		if (entityIdentity.EntityUUID == "TimerUI")
		{
			entityIdentity.Sibling<OnOff>(mGameObjectManager)->IsOn = false;
		}
	}

	mbIsActivated = false;
	mCharacterId1 = 0;
	mCharacterId2 = 0;
	mbIsFinished1 = false;
	mbIsFinished2 = false;
	mbIsStart = false;
	mbIsUndo = false;

	SetTagTypeOnOff(mGameObjectManager, TagType::Cat, false);
	SetTagTypeOnOff(mGameObjectManager, TagType::Beaver, false);
	SetTagTypeOnOff(mGameObjectManager, TagType::Rabbit, false);

	// 타이머 
	if (mDigitCounterId10 != 0 && mDigitCounterId01 != 0 && mDigitCounterId001 != 0 && mDigitCounterId0001 != 0 && mDigitCounterColonId != 0)
	{
		InActiveTimer(mDigitCounterId10, mDigitCounterId01, mDigitCounterId001, mDigitCounterId0001, mDigitCounterColonId);
	}

	// 포트레이트
	if (mPortraitId01 != 0 && mPortraitId02 != 0 && mPortraitId03 != 0)
	{
		mGameObjectManager->QueryComponent<OnOff>(mPortraitId01)->IsOn = false;
		mGameObjectManager->QueryComponent<OnOff>(mPortraitId02)->IsOn = false;
		mGameObjectManager->QueryComponent<OnOff>(mPortraitId03)->IsOn = false;
	}

	EventManager::ScheduleEvent("OnResetScene");
}

void SecondPlayer::OnFinishTracing(std::any data)
{
	if (!mbIsActivated)
		return;

	auto id = std::any_cast<uint32_t>(data);

	if (mCharacterId2 == id)
	{
		mbIsFinished2 = true;
	}
}

void SecondPlayer::OnFinishReplay(std::any data)
{
	if (!mbIsActivated)
		return;

	auto id = std::any_cast<uint32_t>(data);

	if (mCharacterId1 == id)
	{
		mbIsFinished1 = true;
	}
}

void SecondPlayer::SetCharacterIndex(std::any data)
{
	SelectTurn turn = std::any_cast<SelectTurn>(data);
	firstType = turn.first.first;
	secondType = turn.second.first;
	thirdType = turn.third.first;
}

void SecondPlayer::OnSkipTracing(std::any data)
{
	if (!mbIsActivated)
		return;

	mbIsFinished1 = true;
	mbIsFinished2 = true;
}
