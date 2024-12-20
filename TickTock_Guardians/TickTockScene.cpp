#include "pch.h"
#include "TickTockScene.h"
#include "ComponentCommon.h"
#include "TickTockComponentCommon.h"
#include "Tag.h"

TickTockScene::TickTockScene(eSceneState gameState, GameObjectManager* gameObjectManager)
	: mGameState(gameState)
	, mGameObjectManager(gameObjectManager)
{
}

bool TickTockScene::GetEntityIdentityNameID(GameObjectManager* gameObjectManager, const std::string& uuid, uint32_t* outID)
{
	for (EntityIdentity& entityIdentity : System::ComponentItr<EntityIdentity>())
	{
		if (entityIdentity.EntityUUID == uuid)
		{
			*outID = entityIdentity.GameObjectId;

			return true;
		}
	}

	return false;
}

void TickTockScene::SetEntityIdentityNameOnOff(GameObjectManager* gameObjectManager, const std::string& uuid, bool bIsOn)
{
	for (EntityIdentity& entityIdentity : System::ComponentItr<EntityIdentity>())
	{
		if (entityIdentity.EntityUUID == uuid)
		{
			entityIdentity.Sibling<OnOff>(gameObjectManager)->IsOn = bIsOn;
		}
	}
}

void TickTockScene::SetEntityIdentityNameOnOffs(GameObjectManager* gameObjectManager, const std::set<std::string>& uuids, bool bIsOn)
{
	for (EntityIdentity& entityIdentity : System::ComponentItr<EntityIdentity>())
	{
		auto find = uuids.find(entityIdentity.EntityUUID);

		if (find != uuids.end())
		{
			entityIdentity.Sibling<OnOff>(gameObjectManager)->IsOn = bIsOn;
		}
	}
}

TagType TickTockScene::GetSelectTagMappingPlayerTag(TagType tag)
{
	if (tag == TagType::SelectCat)
	{
		return TagType::Cat;
	}
	else if (tag == TagType::SelectBeaver)
	{
		return TagType::Beaver;
	}
	else if (tag == TagType::SelectRabbit)
	{
		return TagType::Rabbit;
	}
}

void TickTockScene::SetTagTypeOnOff(GameObjectManager* gameObjectManager, TagType tagType, bool bIsOn)
{
	for (Tag& tag : System::ComponentItr<Tag>())
	{
		if (tag.tag == tagType)
		{
			tag.Sibling<OnOff>(gameObjectManager)->IsOn = bIsOn;
		}
	}
}

void TickTockScene::FindTimerID(uint32_t* timer1000, uint32_t* timer100, uint32_t* timer10, uint32_t* timer1, uint32_t* colon)
{
	for (EntityIdentity& entityIdentity : System::ComponentItr<EntityIdentity>())
	{
		// 카운터 찾기
		if (entityIdentity.EntityUUID == "DigitCounter10")
		{
			*timer1000 = entityIdentity.GameObjectId;
		}
		else if (entityIdentity.EntityUUID == "DigitCounter01")
		{
			*timer100 = entityIdentity.GameObjectId;
		}
		else if (entityIdentity.EntityUUID == "DigitCounter.10")
		{
			*timer10 = entityIdentity.GameObjectId;
		}
		else if (entityIdentity.EntityUUID == "DigitCounter.01")
		{
			*timer1 = entityIdentity.GameObjectId;
		}
		else if (entityIdentity.EntityUUID == "UITimerColon")
		{
			*colon = entityIdentity.GameObjectId;
		}
	}
}

void TickTockScene::ShowTimer(Replayer* replayer, uint32_t timer1000, uint32_t timer100, uint32_t timer10, uint32_t timer1, uint32_t colon)
{
	if (timer1000 != 0)
	{
		mGameObjectManager->QueryComponent<OnOff>(timer1000)->IsOn = true;
		mGameObjectManager->QueryComponent<Timer>(timer1000)->InitTimePos = (replayer->TraceSize / 60.f);
		mGameObjectManager->QueryComponent<Timer>(timer1000)->bIsReset = true;
		mGameObjectManager->QueryComponent<Timer>(timer1000)->bIsActive = false;
	}
	if (timer100 != 0)
	{
		mGameObjectManager->QueryComponent<OnOff>(timer100)->IsOn = true;
		mGameObjectManager->QueryComponent<Timer>(timer100)->InitTimePos = (replayer->TraceSize / 60.f);
		mGameObjectManager->QueryComponent<Timer>(timer100)->bIsReset = true;
		mGameObjectManager->QueryComponent<Timer>(timer100)->bIsActive = false;
	}
	if (timer10 != 0)
	{
		mGameObjectManager->QueryComponent<OnOff>(timer10)->IsOn = true;
		mGameObjectManager->QueryComponent<Timer>(timer10)->InitTimePos = (replayer->TraceSize / 60.f);
		mGameObjectManager->QueryComponent<Timer>(timer10)->bIsReset = true;
		mGameObjectManager->QueryComponent<Timer>(timer10)->bIsActive = false;
	}
	if (timer1 != 0)
	{
		mGameObjectManager->QueryComponent<OnOff>(timer1)->IsOn = true;
		mGameObjectManager->QueryComponent<Timer>(timer1)->InitTimePos = (replayer->TraceSize / 60.f);
		mGameObjectManager->QueryComponent<Timer>(timer1)->bIsReset = true;
		mGameObjectManager->QueryComponent<Timer>(timer1)->bIsActive = false;
	}
	if (colon != 0)
	{
		mGameObjectManager->QueryComponent<OnOff>(colon)->IsOn = true;
	}
}

void TickTockScene::ActiveTimer(uint32_t timer1000, uint32_t timer100, uint32_t timer10, uint32_t timer1)
{
	if (timer1000 != 0)
	{
		mGameObjectManager->QueryComponent<Timer>(timer1000)->bIsActive = true;
	}
	if (timer100 != 0)
	{
		mGameObjectManager->QueryComponent<Timer>(timer100)->bIsActive = true;
	}
	if (timer10 != 0)
	{
		mGameObjectManager->QueryComponent<Timer>(timer10)->bIsActive = true;
	}
	if (timer1 != 0)
	{
		mGameObjectManager->QueryComponent<Timer>(timer1)->bIsActive = true;
	}
}

void TickTockScene::InActiveTimer(uint32_t timer1000, uint32_t timer100, uint32_t timer10, uint32_t timer1, uint32_t colon)
{
	if (timer1000 != 0)
	{
		mGameObjectManager->QueryComponent<OnOff>(timer1000)->IsOn = false;
		mGameObjectManager->QueryComponent<Timer>(timer1000)->bIsActive = false;
	}
	if (timer100 != 0)
	{
		mGameObjectManager->QueryComponent<OnOff>(timer100)->IsOn = false;
		mGameObjectManager->QueryComponent<Timer>(timer100)->bIsActive = false;
	}
	if (timer10 != 0)
	{
		mGameObjectManager->QueryComponent<OnOff>(timer10)->IsOn = false;
		mGameObjectManager->QueryComponent<Timer>(timer10)->bIsActive = false;
	}
	if (timer1 != 0)
	{
		mGameObjectManager->QueryComponent<OnOff>(timer1)->IsOn = false;
		mGameObjectManager->QueryComponent<Timer>(timer1)->bIsActive = false;
	}
	if (colon != 0)
	{
		mGameObjectManager->QueryComponent<OnOff>(colon)->IsOn = false;
	}
}


std::string TickTockScene::MatchPortraitPlayer(TagType tag)
{
	if (tag == TagType::Cat)
	{
		return "UI_Ingame_Portrait_Cat.png";
	}
	else if (tag == TagType::Beaver)
	{
		return "UI_Ingame_Portrait_Beaver.png";
	}
	else if (tag == TagType::Rabbit)
	{
		return "UI_Ingame_Portrait_Rabbit.png";
	}

}

std::string TickTockScene::MatchPortraitDim(TagType tag)
{
	if (tag == TagType::Cat)
	{
		return "UI_Ingame_Portrait_Cat_Dim.png";
	}
	else if (tag == TagType::Beaver)
	{
		return "UI_Ingame_Portrait_Beaver_Dim.png";
	}
	else if (tag == TagType::Rabbit)
	{
		return "UI_Ingame_Portrait_Rabbit_Dim.png";
	}
}
