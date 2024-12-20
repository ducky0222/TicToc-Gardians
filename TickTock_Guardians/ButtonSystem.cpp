#include "pch.h"
#include "ButtonSystem.h"
#include"Transform.h"
#include "Button.h"
#include <AnimationSystem.h>

#include "../SoundEngine/ISoundEngine.h"

ButtonSystem::ButtonSystem(GameObjectManager* goManager):System(goManager)
{
	EventManager::Subscribe("OnButtonClicked", CreateListenerInfo(&ButtonSystem::OnButtonClicked));
	EventManager::Subscribe("OnButtonUnClicked", CreateListenerInfo(&ButtonSystem::OnButtonUnClicked));
	EventManager::Subscribe("OnSpawnButtonEffect", CreateListenerInfo(&ButtonSystem::OnSpawnButtonEffect));
}



void ButtonSystem::OnButtonClicked(std::any data)
{
	uint32_t buttonID = std::any_cast<uint32_t>(data);
	if (auto button = m_GameObjectManager->QueryComponent<Button>(buttonID))
	{
		button->CollisionCounter++;

		if (button->CollisionCounter == 1)
		{

			button->IsActivated = true;
			EventManager::PublishImmediate("OnTriggerButton", buttonID);
			EventManager::ScheduleEvent("OnSpawnButtonEffect", buttonID,0.1f);

			// 버튼 클릭 사운드
			Sound::GetInstance()->Play(buttonID, "SFX_Button");
		}
	}
}

void ButtonSystem::OnButtonUnClicked(std::any data)
{
	uint32_t buttonID = std::any_cast<uint32_t>(data);
	if (auto button = m_GameObjectManager->QueryComponent<Button>(buttonID))
	{
		if (button->CollisionCounter > 0)
		{
			button->CollisionCounter--;
		}
		if (button->CollisionCounter == 0)
		{
			button->IsActivated = false;
			EventManager::PublishImmediate("OnExitButton", buttonID);
		}
	}
}

void ButtonSystem::FixedUpdate(float deltaTime)
{




}

void ButtonSystem::OnSpawnButtonEffect(std::any data)
{
	uint32_t id = std::any_cast<uint32_t> (data);
	auto button = m_GameObjectManager->QueryComponent<Button>(id);
	if (!button)
		return;
	auto transform = button->Sibling<Transform>(m_GameObjectManager);
	;
	uint32_t gameobjectId = std::any_cast<uint32_t>(data);


	AnimationSystem::SpriteEffectCreateData SECdata1;
	AnimationSystem::SpriteEffectCreateData SECdata2;
	SECdata1.EffectDuration = 0.24; // 0.05*9개
	SECdata1.Offset = { -0.8,1.7,.8 };
	SECdata1.Position = transform->WorldPosition;
	SECdata2.EffectDuration = 0.24; // 0.05*9개
	SECdata2.Offset = { .8,1.7,-0.8 };
	SECdata2.Position = transform->WorldPosition;

	SECdata1.EffectPrefabName = "Effect_Button_Left.prefab";
	EventManager::ScheduleEvent("OnSpriteEffectCreate", SECdata1);
	SECdata2.EffectPrefabName = "Effect_Button_Right.prefab";
	EventManager::ScheduleEvent("OnSpriteEffectCreate", SECdata2);





}
