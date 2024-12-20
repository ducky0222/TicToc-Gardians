#include "pch.h"
#include "TutorialPlayer.h"

#include "Character.h"
#include "Input.h"
#include "EntityIdentity.h"
#include "OnOff.h"
#include "UIController.h"
#include "Replayer.h"
#include "UI2DRenderer.h"
#include "Tag.h"
#include "SkyBoxRenderer.h"
#include "../SoundEngine/ISoundEngine.h"

TutorialPlayer::TutorialPlayer(GameObjectManager* goManager)
	: TickTockScene(eSceneState::TutorialPlayer, goManager)
{
	EventManager::Subscribe("OnFinishTracing", CreateListenerInfo(&TutorialPlayer::OnFinishTracing));
	EventManager::Subscribe("OnResetStage", CreateListenerInfo(&TutorialPlayer::OnResetStage));
	EventManager::Subscribe("OnSceneTransition", CreateListenerInfo(&TutorialPlayer::OnSceneTransition));
	EventManager::Subscribe("OnEnterLevelSelect", CreateListenerInfo(&TutorialPlayer::OnEnterLevelSelect));
}

void TutorialPlayer::Enter()
{
	Sound::GetInstance()->Stop(0, "BGM_Main");
	Sound::GetInstance()->Play(0, "BGM_Ingame");

	mbIsActivated = true;

	FindTimerID(&mDigitCounterId10, &mDigitCounterId01, &mDigitCounterId001, &mDigitCounterId0001, &mDigitCounterColonId);

	for (EntityIdentity& entityIdentity : System::ComponentItr<EntityIdentity>())
	{
		// 카운터 찾기
		if (entityIdentity.EntityUUID == "DigitCounter01")
		{
			mDigitCounterId01 = entityIdentity.GameObjectId;
		}
		else if (entityIdentity.EntityUUID == "DigitCounter10")
		{
			mDigitCounterId10 = entityIdentity.GameObjectId;
		}
		else if (entityIdentity.EntityUUID == "DigitCounter.10")
		{
			mDigitCounterId001 = entityIdentity.GameObjectId;
		}
		else if (entityIdentity.EntityUUID == "DigitCounter.01")
		{
			mDigitCounterId0001 = entityIdentity.GameObjectId;
		}

		// 초상화 찾기
		if (entityIdentity.EntityUUID == "Portrait01")
		{
			mPortraitId = entityIdentity.GameObjectId;
		}
	}

	for (SkyBoxRenderer& skyBox : System::ComponentItr<SkyBoxRenderer>())
	{
		skyBox.bUseSkyBox = false;
	}

	mCharacterId = 0;
	mbIsStart = false;
	mbIsFinished = false;

	uint32_t count = 0;

	for (Character& character : System::ComponentItr<Character>())
	{
		mCharacterId = character.GameObjectId;
		count++;
	}

	// 씬에 캐릭터가 둘 이상이거나 존재하지 않을 경우
	assert(count == 1 && "튜토리얼이라는 단어가 들어간 씬에는 캐릭터를 하나만 배치해주세용!");

	if (mDigitCounterId01 == 0 || mDigitCounterId10 == 0 || mPortraitId == 0)
	{
		assert(false && "튜토리얼이라는 단어가 들어간 씬에는 디지털 카운터 두개와 초상화를 배치해주세용!");
	}

	Replayer* replayer = mGameObjectManager->QueryComponent<Replayer>(mCharacterId);

	if (mDigitCounterId10 != 0 && mDigitCounterId01 != 0 && mDigitCounterId001 != 0 && mDigitCounterId0001 != 0 && mDigitCounterColonId != 0 && replayer != nullptr)
	{
		ShowTimer(replayer, mDigitCounterId10, mDigitCounterId01, mDigitCounterId001, mDigitCounterId0001, mDigitCounterColonId);
	}

	mGameObjectManager->QueryComponent<OnOff>(mCharacterId)->IsOn = true;
	mGameObjectManager->QueryComponent<OnOff>(mPortraitId)->IsOn = true;

	Tag* tag = mGameObjectManager->QueryComponent<Tag>(mCharacterId);

	mGameObjectManager->QueryComponent<UI2DRenderer>(mPortraitId)->FileName = MatchPortraitPlayer(tag->tag);

	EventManager::ScheduleEvent("OnReady", mCharacterId);
}

eSceneState TutorialPlayer::HandleTransition()
{
	// 튜토리얼 클리어 씬으로 전환
	if (mbIsFinished)
	{
		return eSceneState::CheckTutorialClear;
	}
		
	return GetGameState();
}

void TutorialPlayer::Update(float deltaTime)
{
	if (!mbIsStart && Input::IsAnyKeyDown())
	{
		mbIsStart = true;
		EventManager::ScheduleEvent("OnStartMoving", mCharacterId);
		ActiveTimer(mDigitCounterId10, mDigitCounterId01, mDigitCounterId001, mDigitCounterId0001);
	}

}

void TutorialPlayer::Exit()
{
	mGameObjectManager->QueryComponent<OnOff>(mCharacterId)->IsOn = false;
	mGameObjectManager->QueryComponent<OnOff>(mPortraitId)->IsOn = false;

	if (mDigitCounterId10 != 0 && mDigitCounterId01 != 0 && mDigitCounterId001 != 0 && mDigitCounterId0001 != 0 && mDigitCounterColonId != 0)
	{
		InActiveTimer(mDigitCounterId10, mDigitCounterId01, mDigitCounterId001, mDigitCounterId0001, mDigitCounterColonId);
	}

	for (SkyBoxRenderer& skyBox : System::ComponentItr<SkyBoxRenderer>())
	{
		skyBox.bUseSkyBox = true;
	}

	EventManager::ScheduleEvent("OnResetScene");

}

void TutorialPlayer::OnFinishTracing(std::any data)
{
	if (std::any_cast<uint32_t>(data) == mCharacterId)
	{
		mbIsFinished = true;
	}
}

void TutorialPlayer::OnResetStage(std::any data)
{
	if (!mbIsActivated)
		return;

	mbIsResetStage = true;
}

void TutorialPlayer::OnSceneTransition(std::any data)
{
	//if (!mbIsActivated)
	//{
	//	return;
	//}


	//if (!mbEyeCatchUsed)
	//{
	//	mbEyeCatchUsed = true;
	//	std::string sceneName = std::any_cast<std::string>(data);
	//	//EventManager::ScheduleEvent(/*OnMapSearch*/"SceneTransition", sceneName, 3.0f);
	//	EventManager::ScheduleEvent("OnSceneTransition", sceneName, 3.0f);

	//	/// eyecatcher 랜덤으로 찾아서 연결
	//	/// 이거 rand라 항상 cat이 먼저 나옴

	//	static std::random_device rd;
	//	static std::mt19937 gen(rd());
	//	static std::uniform_int_distribution<int> distrib(0, 2);
	//	auto randomNum = std::views::iota(0, 3) | std::views::transform([&](auto) {return distrib(gen); });
	//	std::vector<std::pair<std::string, std::string>> eyeCatches = { {"EyeCatchBeaver.scene","EyeCatchBeaver"},{"EyeCatchCat.scene","EyeCatchCat"},{"EyeCatchRabbit.scene","EyeCatchRabbit"} };
	//	EventManager::PublishImmediate("OnSceneTransition", eyeCatches[randomNum.front()].first);
	//	Sound::GetInstance()->Play(0, eyeCatches[randomNum.front()].second);
	//}
}

void TutorialPlayer::OnEnterLevelSelect(std::any data)
{
	mbEyeCatchUsed = false;
}
