#include "pch.h"
#include "DimensionCrackSystem.h"
#include "GameObjectManager.h"
#include "DimensionCrack.h"
#include "EventManager.h"
#include "SerializeSystem.h"
#include "Transform.h"
#include "OnOff.h"
#include "Character.h"
#include "../SoundEngine/ISoundEngine.h"

DimensionCrackSystem::DimensionCrackSystem(GameObjectManager* goManager)
	: System(goManager)
{
	EventManager::Subscribe("OnTriggerDimensionCrack", CreateListenerInfo(&DimensionCrackSystem::OnTriggerDimensionCrack));
	EventManager::Subscribe("OnExitDimensionCrack", CreateListenerInfo(&DimensionCrackSystem::OnExitDimensionCrack));
	EventManager::Subscribe("OnCheckMissionClear", CreateListenerInfo(&DimensionCrackSystem::OnCheckMissionClear));
}

void DimensionCrackSystem::Start(uint32_t gameObjectId)
{
	
}

void DimensionCrackSystem::Initialize()
{
	for (DimensionCrack& crack : System::ComponentItr<DimensionCrack>())
	{
		crack.CharactersNearCrack = 0;
	}
}

void DimensionCrackSystem::Finish(uint32_t gameObjectId)
{

}

void DimensionCrackSystem::Finalize()
{

}

void DimensionCrackSystem::OnTriggerDimensionCrack(std::any data)
{
	const auto [lhs, rhs] = any_cast<std::pair<uint32_t, uint32_t>>(data);
	auto children = m_GameObjectManager->QueryComponent<Children>(lhs);
	if (children)
	{
		for (auto child : children->ChildrenID)
		{
			m_GameObjectManager->QueryComponent<OnOff>(child)->IsOn = true;
		}
	}

	DimensionCrack* crack = m_GameObjectManager->QueryComponent<DimensionCrack>(rhs);
	if (crack)
	{
		crack->CharactersNearCrack += 1;
	}
}

void DimensionCrackSystem::OnExitDimensionCrack(std::any data)
{
	const auto [lhs, rhs] = any_cast<std::pair<uint32_t, uint32_t>>(data);
	auto children = m_GameObjectManager->QueryComponent<Children>(lhs);
	if (children)
	{
		for (auto child : children->ChildrenID)
		{
			m_GameObjectManager->QueryComponent<OnOff>(child)->IsOn = false;
		}
	}

	DimensionCrack* crack = m_GameObjectManager->QueryComponent<DimensionCrack>(rhs);
	if (crack)
	{
		if(crack->CharactersNearCrack > 0)
			crack->CharactersNearCrack -= 1;
	}
}

void DimensionCrackSystem::OnCheckMissionClear(std::any data)
{
	int clearIndex = 0;
	int crackCount = 0;
	
	for (DimensionCrack& crack : System::ComponentItr<DimensionCrack>())
	{
		if (crack.CharactersNearCrack > 0)
		{
			clearIndex += 1;
		}
		crackCount += 1;
	}

	if (clearIndex == crackCount)
	{
		EventManager::ScheduleEvent("OnClearAllMission");

		// 차원의 틈 수리 사운드
		Sound::GetInstance()->Play(0, "SFX_Repair_Tada_Fixing");
	}
	else
	{
		EventManager::ScheduleEvent("OnFailMission");
	}
}
