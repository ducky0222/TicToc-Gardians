#include "pch.h"
#include "LevelSelectScene.h"
#include "GameObjectManager.h"
#include "ComponentCommon.h"
#include "TickTockComponentCommon.h"
#include "Input.h"
#include "SerializeSystem.h"

#include "../SoundEngine/ISoundEngine.h"

LevelSelectScene::LevelSelectScene(GameObjectManager* gameObjectManager)
	: TickTockScene(eSceneState::LevelSelect, gameObjectManager)
{
	EventManager::Subscribe("OnFirstSceneIn", CreateListenerInfo(&LevelSelectScene::OnFirstSceneIn));
	EventManager::Subscribe("OnFirstEnterTabletIn", CreateListenerInfo(&LevelSelectScene::OnFirstEnterTabletIn));
	EventManager::Subscribe("OnFirstEnterNextBtn", CreateListenerInfo(&LevelSelectScene::OnFirstEnterNextBtn));
	EventManager::Subscribe("OnFirstEnterBeforeBtn", CreateListenerInfo(&LevelSelectScene::OnFirstEnterBeforeBtn));
	EventManager::Subscribe("OnFirstEnterTabletOut", CreateListenerInfo(&LevelSelectScene::OnFirstEnterTabletOut));
	EventManager::Subscribe("OnFirstSceneOut", CreateListenerInfo(&LevelSelectScene::OnFirstSceneOut));

	EventManager::Subscribe("OnNormalSelectIn", CreateListenerInfo(&LevelSelectScene::OnNormalSelectIn));
	EventManager::Subscribe("OnNormalTableUIIn", CreateListenerInfo(&LevelSelectScene::OnNormalTableUIIn));
	EventManager::Subscribe("OnNormalTableUIOut", CreateListenerInfo(&LevelSelectScene::OnNormalTableUIOut));
	EventManager::Subscribe("OnNormalSelectOut", CreateListenerInfo(&LevelSelectScene::OnNormalSelectOut));

	EventManager::Subscribe("OnHardSelectIn", CreateListenerInfo(&LevelSelectScene::OnHardSelectIn));
	EventManager::Subscribe("OnHardTableUIIn", CreateListenerInfo(&LevelSelectScene::OnHardTableUIIn));
	EventManager::Subscribe("OnHardTableUIOut", CreateListenerInfo(&LevelSelectScene::OnHardTableUIOut));
	EventManager::Subscribe("OnHardSelectOut", CreateListenerInfo(&LevelSelectScene::OnHardSelectOut));

	EventManager::Subscribe("OnLevelCursorIn", CreateListenerInfo(&LevelSelectScene::OnLevelCursorIn));
	EventManager::Subscribe("OnLevelCursorOut", CreateListenerInfo(&LevelSelectScene::OnLevelCursorOut));

	EventManager::Subscribe("OnStageBtnClicked", CreateListenerInfo(&LevelSelectScene::OnStageBtnClicked));

	EventManager::Subscribe("OnPause", CreateListenerInfo(&LevelSelectScene::OnPause));
	EventManager::Subscribe("OnResume", CreateListenerInfo(&LevelSelectScene::OnResume));
	EventManager::Subscribe("OnCreditButtonClick", CreateListenerInfo(&LevelSelectScene::OnCreditButtonClick));
}

void LevelSelectScene::Enter()
{
	bIsActiveEvent = true;
	mbIsConfig = false;
	mConfigID = 0;
	EventManager::PublishImmediate("OnFirstSceneIn");
}
eSceneState LevelSelectScene::HandleTransition()
{
	// 스테이지가 선택 되었을때 씬 트랜지션 전환
	if (bIsStageSelected)
	{
		// 맵서치씬 Enter 에게 씬 로드 위임
		return eSceneState::MapSearchScene;
	}

	return GetGameState();
}

void LevelSelectScene::Update(float deltaTime)
{
	if (Input::GetKeyDown(KEY::ESC) && !mbIsConfig)
	{
		EventManager::ScheduleEvent("OnPause");
	}
	if (Input::GetKeyDown(KEY::F4))
	{
		EventManager::ScheduleEvent("OnSceneTransition", std::string("TitleHong.scene"));
	}
}
void LevelSelectScene::Exit()
{
	// 맴버 변수 초기화
	bIsActiveEvent = false;
	bIsStageSelected = false;

	// 아이캐처 씬을 보여주기 위한 이벤트
	EventManager::PublishImmediate("OnEnterLevelSelect");

	// 아웃 이벤트 모두 호출하여 상태 초기화
	OnFirstEnterTabletOut({});
	OnFirstSceneOut({});

	OnNormalSelectOut({});
	OnHardSelectOut({});

	OnNormalTableUIOut({});
	OnHardTableUIOut({});

	OnLevelCursorOut({});
}


void LevelSelectScene::OnFirstSceneIn(std::any data)
{
	if (!bIsActiveEvent)
	{
		return;
	}

	handleTransformAnimActive(FIRST_ENTER_TABLET_NAME, true);
	handleTransformAnimActive(TV_UI_NAME, false);

	uint32_t entityId;

	if (GetEntityIdentityNameID(mGameObjectManager, FIRST_ENTER_TABLET_NAME, &entityId))
	{
		NeoTransformAnim2D* transformAnim2D = mGameObjectManager->QueryComponent<NeoTransformAnim2D>(entityId);

		if (transformAnim2D != nullptr)
		{
			EventManager::ScheduleEvent("OnFirstEnterTabletIn", {}, transformAnim2D->ProcessingTime * 1.2f);
		}
	}

	if (GetEntityIdentityNameID(mGameObjectManager, LOBBY_CHARACTER_NAME, &entityId))
	{
		Character* character = mGameObjectManager->QueryComponent<Character>(entityId);

		if (character != nullptr)
		{
			character->IsMovable = false;
			character->IsControllable = false;
		}
	}
}

void LevelSelectScene::OnFirstEnterTabletIn(std::any data)
{
	if (!bIsActiveEvent)
	{
		return;
	}

	uint32_t entityId;

	if (GetEntityIdentityNameID(mGameObjectManager, FIRST_ENTER_TABLET_NAME, &entityId))
	{
		NeoTransformAnim2D* transformAnim = mGameObjectManager->QueryComponent<NeoTransformAnim2D>(entityId);

		// 델타타임 제대로 반영 안되어 임시로 주석
		// if (transformAnim->ProcessingTime <= transformAnim->TimePos)
		{
			handleChildEntityOnOff(FIRST_ENTER_TABLET_NAME, true);
		}
	}
}

void LevelSelectScene::OnFirstEnterNextBtn(std::any data)
{
	if (!bIsActiveEvent)
	{
		return;
	}

	Sound::GetInstance()->Play(0, "SFX_Button");

	uint32_t entityId;

	// 컷신 스프라이트 반영
	if (GetEntityIdentityNameID(mGameObjectManager, std::any_cast<std::string>(data), &entityId))
	{
		SpriteController* spriteController = mGameObjectManager->QueryComponent<SpriteController>(entityId);

		if (spriteController != nullptr)
		{
			int index = spriteController->TimePos / spriteController->FrameInterval;

			if (index >= spriteController->SpriteCount - 1)
			{
				spriteController->TimePos = 0;
				OnFirstSceneOut({});
			}
			else
			{
				spriteController->TimePos += spriteController->FrameInterval + 0.01f;
			}
		}
	}

	// 미니인덱스 반영
	if (GetEntityIdentityNameID(mGameObjectManager, ENTER_TABLET_MINI_INDEX_NAME, &entityId))
	{
		SpriteController* spriteController = mGameObjectManager->QueryComponent<SpriteController>(entityId);

		if (spriteController != nullptr)
		{
			int index = spriteController->TimePos / spriteController->FrameInterval;

			if (index >= spriteController->SpriteCount - 1)
			{
				spriteController->TimePos = 0;
				OnFirstSceneOut({});
			}
			else
			{
				spriteController->TimePos += spriteController->FrameInterval + 0.01f;
			}
		}
	}
}

void LevelSelectScene::OnFirstEnterBeforeBtn(std::any data)
{
	if (!bIsActiveEvent)
	{
		return;
	}

	Sound::GetInstance()->Play(0, "SFX_Button");

	uint32_t entityId;

	// 컷신 스프라이트 반영
	if (GetEntityIdentityNameID(mGameObjectManager, std::any_cast<std::string>(data), &entityId))
	{
		SpriteController* spriteController = mGameObjectManager->QueryComponent<SpriteController>(entityId);

		if (spriteController != nullptr)
		{
			int index = spriteController->TimePos / spriteController->FrameInterval;

			if (index > 0)
			{
				spriteController->TimePos -= spriteController->FrameInterval + 0.01f;
			}
		}
	}

	// 미니인덱스 반영
	if (GetEntityIdentityNameID(mGameObjectManager, ENTER_TABLET_MINI_INDEX_NAME, &entityId))
	{
		SpriteController* spriteController = mGameObjectManager->QueryComponent<SpriteController>(entityId);

		if (spriteController != nullptr)
		{
			int index = spriteController->TimePos / spriteController->FrameInterval;

			if (index > 0)
			{
				spriteController->TimePos -= spriteController->FrameInterval + 0.01f;
			}
		}
	}
}

void LevelSelectScene::OnFirstEnterTabletOut(std::any data)
{
	if (!bIsActiveEvent)
	{
		return;
	}

	handleChildEntityOnOff(FIRST_ENTER_TABLET_NAME, false);
}

void LevelSelectScene::OnFirstSceneOut(std::any data)
{
	if (!bIsActiveEvent)
	{
		return;
	}

	handleTransformAnimActive(FIRST_ENTER_TABLET_NAME, false);
	handleTransformAnimActive(TV_UI_NAME, true);

	OnFirstEnterTabletOut(data);

	uint32_t entityId;

	if (GetEntityIdentityNameID(mGameObjectManager, FIRST_ENTER_TABLET_NAME, &entityId))
	{
		handleOnOffChildrenRecursive(entityId, false);
	}

	if (GetEntityIdentityNameID(mGameObjectManager, LOBBY_CHARACTER_NAME, &entityId))
	{
		Character* character = mGameObjectManager->QueryComponent<Character>(entityId);

		if (character != nullptr)
		{
			character->IsMovable = true;
			character->IsControllable = true;
		}
	}
}


void LevelSelectScene::OnNormalSelectIn(std::any data)
{
	if (!bIsActiveEvent)
	{
		return;
	}

	SetEntityIdentityNameOnOff(mGameObjectManager, NORMAL_CAM_NAME, true);
	SetEntityIdentityNameOnOff(mGameObjectManager, NORMAL_TABLET_NAME, true);
	SetEntityIdentityNameOnOff(mGameObjectManager, HARD_CAM_NAME, false);

	handleTransformAnimActive(NORMAL_CAM_NAME, true);
	handleTransformAnimActive(NORMAL_TABLET_NAME, true);

	handleTVUIDisplayScreen("TVGlitch");
	OnNormalTableUIOut(data);

	uint32_t entityId;

	if (GetEntityIdentityNameID(mGameObjectManager, NORMAL_TABLET_NAME, &entityId))
	{
		NeoTransformAnim2D* transformAnim2D = mGameObjectManager->QueryComponent<NeoTransformAnim2D>(entityId);

		if (transformAnim2D != nullptr)
		{
			EventManager::ScheduleEvent("OnNormalTableUIIn", {}, transformAnim2D->ProcessingTime * 1.2f);
		}
	}
}

void LevelSelectScene::OnNormalTableUIIn(std::any data)
{
	if (!bIsActiveEvent)
	{
		return;
	}

	handleTVUIDisplayScreen("TVNormal");

	uint32_t entityId;

	if (GetEntityIdentityNameID(mGameObjectManager, NORMAL_TABLET_NAME, &entityId))
	{
		NeoTransformAnim2D* transformAnim = mGameObjectManager->QueryComponent<NeoTransformAnim2D>(entityId);

		if (transformAnim->ProcessingTime <= transformAnim->TimePos)
		{
			handleChildEntityOnOff(NORMAL_TABLET_NAME, true);
		}
	}
}

void LevelSelectScene::OnNormalTableUIOut(std::any data)
{
	if (!bIsActiveEvent)
	{
		return;
	}

	handleChildEntityOnOff(NORMAL_TABLET_NAME, false);
}

void LevelSelectScene::OnNormalSelectOut(std::any data)
{
	if (!bIsActiveEvent)
	{
		return;
	}

	handleTransformAnimActive(NORMAL_CAM_NAME, false);
	handleTransformAnimActive(NORMAL_TABLET_NAME, false);

	handleTVUIDisplayScreen("TVSelect");
	OnNormalTableUIOut(data);

	uint32_t entityId;

	if (GetEntityIdentityNameID(mGameObjectManager, NORMAL_TABLET_NAME, &entityId))
	{
		handleOnOffChildrenRecursive(entityId, false);
	}
}

void LevelSelectScene::OnHardSelectIn(std::any data)
{
	if (!bIsActiveEvent)
	{
		return;
	}

	SetEntityIdentityNameOnOff(mGameObjectManager, NORMAL_CAM_NAME, false);
	SetEntityIdentityNameOnOff(mGameObjectManager, HARD_CAM_NAME, true);

	handleTransformAnimActive(HARD_CAM_NAME, true);
	handleTransformAnimActive(HARD_TABLET_NAME, true);

	handleTVUIDisplayScreen("TVGlitch");
	OnHardTableUIOut(data);

	uint32_t entityId;

	if (GetEntityIdentityNameID(mGameObjectManager, NORMAL_TABLET_NAME, &entityId))
	{
		NeoTransformAnim2D* transformAnim2D = mGameObjectManager->QueryComponent<NeoTransformAnim2D>(entityId);

		if (transformAnim2D != nullptr)
		{
			EventManager::ScheduleEvent("OnHardTableUIIn", {}, transformAnim2D->ProcessingTime * 1.2f);
		}
	}
}

void LevelSelectScene::OnHardTableUIIn(std::any data)
{
	if (!bIsActiveEvent)
	{
		return;
	}

	handleTVUIDisplayScreen("TVHard");

	uint32_t entityId;

	if (GetEntityIdentityNameID(mGameObjectManager, HARD_TABLET_NAME, &entityId))
	{
		NeoTransformAnim2D* transformAnim = mGameObjectManager->QueryComponent<NeoTransformAnim2D>(entityId);

		if (transformAnim->ProcessingTime <= transformAnim->TimePos)
		{
			handleChildEntityOnOff(HARD_TABLET_NAME, true);
		}
	}
}

void LevelSelectScene::OnHardTableUIOut(std::any data)
{
	if (!bIsActiveEvent)
	{
		return;
	}

	handleChildEntityOnOff(HARD_TABLET_NAME, false);
}

void LevelSelectScene::OnHardSelectOut(std::any data)
{
	if (!bIsActiveEvent)
	{
		return;
	}

	handleTransformAnimActive(HARD_CAM_NAME, false);
	handleTransformAnimActive(HARD_TABLET_NAME, false);

	handleTVUIDisplayScreen("TVSelect");
	OnHardTableUIOut(data);

	uint32_t entityId;

	if (GetEntityIdentityNameID(mGameObjectManager, HARD_TABLET_NAME, &entityId))
	{
		handleOnOffChildrenRecursive(entityId, false);
	}
}



void LevelSelectScene::OnLevelCursorIn(std::any data)
{
	if (!bIsActiveEvent)
	{
		return;
	}

	Sound::GetInstance()->Play(0, "SFX_UI_Select_1");
	handleChildEntityOnOff(std::any_cast<std::string>(data), true);
}

void LevelSelectScene::OnLevelCursorOut(std::any data)
{
	if (!bIsActiveEvent)
	{
		return;
	}

	handleChildEntityOnOff(std::any_cast<std::string>(data), false);
}

void LevelSelectScene::OnStageBtnClicked(std::any data)
{
	bIsStageSelected = true;
	Sound::GetInstance()->Play(0, "SFX_UI_Select_2");

	EventManager::ScheduleEvent("OnMapSearchSceneTransition", data);

}

void LevelSelectScene::handleTVUIDisplayScreen(const std::string& TVUIDisplayName)
{
	uint32_t entityId;

	if (GetEntityIdentityNameID(mGameObjectManager, TV_UI_NAME, &entityId))
	{
		Children* children = mGameObjectManager->QueryComponent<Children>(entityId);

		if (children != nullptr)
		{
			for (auto child : children->ChildrenID)
			{
				if (mGameObjectManager->GetGameObjectName(child) == TVUIDisplayName)
				{
					mGameObjectManager->QueryComponent<OnOff>(child)->IsOn = true;
				}
				else
				{
					mGameObjectManager->QueryComponent<OnOff>(child)->IsOn = false;
				}
			}
		}
	}
}

void LevelSelectScene::handleTransformAnimActive(const std::string& entityIdentity, bool bIsActive)
{
	uint32_t entityId;

	if (GetEntityIdentityNameID(mGameObjectManager, entityIdentity, &entityId))
	{
		NeoTransformAnim3D* transformAnim = mGameObjectManager->QueryComponent<NeoTransformAnim3D>(entityId);

		if (transformAnim != nullptr)
		{
			if (bIsActive)
			{
				transformAnim->bIsReverse = false;
				transformAnim->bIsReset = true;
				transformAnim->bIsActive = true;
			}
			else
			{
				transformAnim->bIsReverse = true;
			}
		}
	}

	if (GetEntityIdentityNameID(mGameObjectManager, entityIdentity, &entityId))
	{
		NeoTransformAnim2D* transformAnim = mGameObjectManager->QueryComponent<NeoTransformAnim2D>(entityId);

		if (transformAnim != nullptr)
		{
			if (bIsActive)
			{
				transformAnim->bIsReverse = false;
				transformAnim->bIsReset = true;
				transformAnim->bIsActive = true;
			}
			else
			{
				transformAnim->bIsReverse = true;
			}
		}
	}
}

void LevelSelectScene::handleChildEntityOnOff(const std::string& entityIdentity, bool bIsOn)
{
	uint32_t entityId;

	if (GetEntityIdentityNameID(mGameObjectManager, entityIdentity, &entityId))
	{
		Children* children = mGameObjectManager->QueryComponent<Children>(entityId);

		if (children != nullptr)
		{
			for (auto child : children->ChildrenID)
			{
				mGameObjectManager->QueryComponent<OnOff>(child)->IsOn = bIsOn;
			}
		}
	}
}

void LevelSelectScene::handleOnOffChildrenRecursive(uint32_t entityId, bool bIsOn)
{
	Children* children = mGameObjectManager->QueryComponent<Children>(entityId);

	if (children != nullptr)
	{
		for (auto child : children->ChildrenID)
		{
			mGameObjectManager->QueryComponent<OnOff>(child)->IsOn = bIsOn;
			handleOnOffChildrenRecursive(child, bIsOn);
		}
	}
}

void LevelSelectScene::OnPause(std::any data)
{
	if (!bIsActiveEvent)
	{
		return;
	}

	mConfigID = SerializeSystem(mGameObjectManager).Instantiate("../Data/prefab/UILevelSelectConfig.prefab");
}
void LevelSelectScene::OnResume(std::any data)
{
	if (!bIsActiveEvent)
	{
		return;
	}

	if (mConfigID != 0)
	{
		EventManager::OnDestroy(mConfigID);
		mConfigID = 0;
		mbIsConfig = false;
	}
}

void LevelSelectScene::OnCreditButtonClick(std::any data)
{
	EventManager::PublishImmediate("OnResume", data);
	EventManager::ScheduleEvent("OnSceneTransition", data);
	EventManager::ScheduleEvent("OnSceneTransition", std::string("LevelSelectHong.scene"), 6.f);
}