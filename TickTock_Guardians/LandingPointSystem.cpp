#include "pch.h"
#include "LandingPointSystem.h"

#include "Character.h"
#include "Tag.h"
#include "Transform.h"
#include "LandingPoint.h"
#include <AnimationSystem.h>
#include "TickTockScene.h"
#include "../SoundEngine/ISoundEngine.h"

LandingPointSystem::LandingPointSystem(GameObjectManager* goManager)
	: System(goManager)
{
	EventManager::Subscribe("OnTriggerLandingPoint", CreateListenerInfo(&LandingPointSystem::OnTriggerLandingPoint));
	EventManager::Subscribe("OnSpawnLandingPointEffect", CreateListenerInfo(&LandingPointSystem::OnSpawnLandingPointEffect));

	EventManager::Subscribe("OnSetCurrentSceneState", CreateListenerInfo(&LandingPointSystem::OnSetCurrentSceneState));
	EventManager::Subscribe("OnUndoLandingPoint", CreateListenerInfo(&LandingPointSystem::OnUndoLandingPoint));
}

void LandingPointSystem::OnTriggerLandingPoint(std::any data)
{
	// lhs: Character, rhs: Landing Point
	auto [lhs, rhs] = std::any_cast<std::pair<uint32_t, uint32_t>>(data);

	auto character = m_GameObjectManager->QueryComponent<Character>(lhs);

	if (!character or !character->IsControllable)
		return;

	for (LandingPoint& landing : ComponentItr<LandingPoint>())
	{
		if (landing.GameObjectId == rhs)
		{
			landing.IsCurrentStartPoint = true;

			if(landing.LandingOrder == 0)
				landing.LandingOrder = mCurrentLandingOrder;

			EventManager::ScheduleEvent("OnSpawnLandingPointEffect", landing.GameObjectId);
		}
		else
		{
			landing.IsCurrentStartPoint = false;

			// 현재 씬에 이전에 닿은 랜딩 포인트일 경우 랜딩하지 않음 처리
			// 동일한 랜딩 오더를 갖는 랜딩 컴포넌트는 존재 하지 않음을 보장
			if (landing.LandingOrder == mCurrentLandingOrder)
				landing.LandingOrder = 0;
		}
	}
}

void LandingPointSystem::OnSpawnLandingPointEffect(std::any data)
{
	uint32_t id = std::any_cast<uint32_t> (data);
	auto landingPoint = m_GameObjectManager->QueryComponent<LandingPoint>(id);
	if (!landingPoint)
		return;
	auto transform = landingPoint->Sibling<Transform>(m_GameObjectManager);

	uint32_t gameobjectId = std::any_cast<uint32_t>(data);


	AnimationSystem::SpriteEffectCreateData SECdata1;
	AnimationSystem::SpriteEffectCreateData SECdata2;
	SECdata1.EffectDuration = 0.32; // 
	SECdata1.Offset = Vector3{ -0.5,1.7,.8 };
	SECdata1.Position = transform->WorldPosition;
	SECdata2.EffectDuration = 0.32; //
	SECdata2.Offset = Vector3{ .8,1.7,-0.5 };
	SECdata2.Position = transform->WorldPosition;

	SECdata1.EffectPrefabName = "Effect_StartingPoint_Left.prefab";
	EventManager::ScheduleEvent("OnSpriteEffectCreate", SECdata1);
	SECdata2.EffectPrefabName = "Effect_StartingPoint_Right.prefab";
	EventManager::ScheduleEvent("OnSpriteEffectCreate", SECdata2);

	Sound::GetInstance()->Play(0, "SFX_NewStartPoint");
}

void LandingPointSystem::OnSetCurrentSceneState(std::any data)
{
	auto mapEnum = std::any_cast<eSceneState>(data);

	// 현재 씬 설정
	switch (mapEnum)
	{
	case eSceneState::FirstPlayer:
		mCurrentLandingOrder = 1;
		break;
	case eSceneState::SecondPlayer:
		mCurrentLandingOrder = 2;
		break;
	case eSceneState::ThirdPlayer:
		mCurrentLandingOrder = 3;
		break;
	default:
		mCurrentLandingOrder = 0;
		break;
	}

}

void LandingPointSystem::OnUndoLandingPoint(std::any data)
{
	auto mapEnum = std::any_cast<eSceneState>(data);
	uint32_t targetLandingOrder = 0;

	switch (mapEnum)
	{
	case eSceneState::FirstPlayer:
		targetLandingOrder = 1;
		break;
	case eSceneState::SecondPlayer:
		targetLandingOrder = 2;
		break;
	case eSceneState::ThirdPlayer:
		targetLandingOrder = 3;
		break;
	default:
		targetLandingOrder = 0;
		break;
	}

	assert(targetLandingOrder);

	bool isChanged = false;
	uint32_t landingCount = 0;

	// 이전 씬까지 가장 마지막에 도달했던 랜딩 포인트까지 반복
	// 현재 씬에 닿은 랜딩 포인트는 무시
	while(!isChanged)
	{
		for (LandingPoint& landing : ComponentItr<LandingPoint>())
		{
			landingCount++;
			if (targetLandingOrder == 1) // 첫번째 씬일 경우
			{
				landing.IsCurrentStartPoint = false;
				landing.LandingOrder = 0;
				isChanged = true;
			}
			else
			{
				// 이전 씬에 찍혔던 랜딩 포인트로 이동
				if (landing.LandingOrder == targetLandingOrder - 1)
				{
					landing.IsCurrentStartPoint = true;
					isChanged = true;
				}
			}
		}

		if (landingCount == 0)
			isChanged = true;

		if (targetLandingOrder >= 2)
			targetLandingOrder--;
	}
	
}
