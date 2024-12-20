#include "pch.h"
#include "FirstPlayer.h"

#include "Character.h"
#include "GameObjectManager.h"
#include "Input.h"
#include "System.h"
#include "TickTockComponentCommon.h"
#include "EntityIdentity.h"
#include "UIController.h"
#include "UI2DRenderer.h"

FirstPlayer::FirstPlayer(GameObjectManager* gameObjectManager)
	: TickTockScene(eSceneState::FirstPlayer, gameObjectManager)
{
	EventManager::Subscribe("OnFinishTracing", CreateListenerInfo(&FirstPlayer::OnFinishTracing));
	EventManager::Subscribe("OnTransitionNextPlayer", CreateListenerInfo(&FirstPlayer::OnFinishTracing));
	EventManager::Subscribe("SetCharacterIndex", CreateListenerInfo(&FirstPlayer::SetCharacterIndex));
	EventManager::Subscribe("OnSkipTracing", CreateListenerInfo(&FirstPlayer::OnSkipTracing));

}

void FirstPlayer::Enter()
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

	SetTagTypeOnOff(mGameObjectManager, TagType::Cat, false);
	SetTagTypeOnOff(mGameObjectManager, TagType::Beaver, false);
	SetTagTypeOnOff(mGameObjectManager, TagType::Rabbit, false);
	SetTagTypeOnOff(mGameObjectManager, firstSelectType, true);

	// 첫번째 캐릭터, 두번째, 세번쨰 캐릭터 찾기
	for (Character& character : System::ComponentItr<Character>())
	{
		Tag* tag = character.Sibling<Tag>(mGameObjectManager);
		assert(tag != nullptr);

		if (tag->tag == firstSelectType)
		{
			mCharacterId1 = character.GameObjectId;

		}
		else if (tag->tag == secondSelectType)
		{
			mCharacterId2 = character.GameObjectId;
		}
		else if (tag->tag == thirdSelectType)
		{
			mCharacterId3 = character.GameObjectId;
		}
	}

	for (Timer& timer : System::ComponentItr<Timer>())
	{
		timer.Sibling<OnOff>(mGameObjectManager)->IsOn = true;
	}

	Replayer* replayer = mGameObjectManager->QueryComponent<Replayer>(mCharacterId1);

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
		mGameObjectManager->QueryComponent<UI2DRenderer>(mPortraitId01)->FileName = MatchPortraitPlayer(firstSelectType);
		mGameObjectManager->QueryComponent<UI2DRenderer>(mPortraitId02)->FileName = MatchPortraitDim(secondSelectType);
		mGameObjectManager->QueryComponent<UI2DRenderer>(mPortraitId03)->FileName = MatchPortraitDim(thirdSelectType);
	}

	EventManager::ScheduleEvent("OnReady", mCharacterId1);
	EventManager::ScheduleEvent("OnSetCurrentSceneState", GetGameState());

	SetEntityIdentityNameOnOff(mGameObjectManager, "PleasePressAnyKey", false);

	assert(mCharacterId1);
}
eSceneState FirstPlayer::HandleTransition()
{
	if (mbIsFinished)
	{
		return eSceneState::SecondPlayer;
	}

	return GetGameState();
}
void FirstPlayer::Update(float deltaTime)
{
	if (!mbIsStart && Input::IsAnyKeyDown())
	{
		mbIsStart = true;
		EventManager::ScheduleEvent("OnStartMoving", mCharacterId1);
		ActiveTimer(mDigitCounterId10, mDigitCounterId01, mDigitCounterId001, mDigitCounterId0001);
	}


	if (Input::GetKeyDown(KEY::R)) // 언두
	{
		// First Player 씬에서는 이동 중 언두만 가능
		if (mbIsStart)
		{
			EventManager::PublishImmediate("OnUndoLandingPoint", GetGameState());
			EventManager::PublishImmediate("OnUndo", mCharacterId1);

			Exit();
			Enter();
		}
	}

	//auto digitCounter10 = mGameObjectManager->QueryComponent<UIController>(mDigitCounterId10);
}
void FirstPlayer::Exit()
{
	mbIsActivated = false;

	mCharacterId1 = 0;
	mbIsFinished = false;
	mbIsStart = false;

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

void FirstPlayer::OnFinishTracing(std::any data)
{
	if (!mbIsActivated)
		return;

	auto id = std::any_cast<uint32_t>(data);

	if (mCharacterId1 == id)
	{
		mbIsFinished = true;
	}
}

void FirstPlayer::OnTransitionNextPlayer(std::any data)
{
	if (!mbIsActivated)
		return;

	mbIsFinished = true;
}

void FirstPlayer::SetCharacterIndex(std::any data)
{
	SelectTurn turn = std::any_cast<SelectTurn>(data);
	firstType = turn.first.first;
	secondType = turn.second.first;
	thirdType = turn.third.first;
}

void FirstPlayer::OnSkipTracing(std::any data)
{
	if (!mbIsActivated)
		return;

	mbIsFinished = true;
}
