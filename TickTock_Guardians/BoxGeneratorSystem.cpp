#include "pch.h"
#include "BoxGeneratorSystem.h"
#include "BoxGenerator.h"
#include <HysxEngine.h>
#include"Transform.h"
#include "Tag.h"
#include <SerializeSystem.h>
#include "Button.h"
#include <ComponentCommon.h>
#include <ComponentCommon.h>
#include "../SoundEngine/ISoundEngine.h"


BoxGeneratorSystem::BoxGeneratorSystem(GameObjectManager* goManager)
	:System(goManager)
{
	EventManager::Subscribe("OnTriggerButton", CreateListenerInfo(&BoxGeneratorSystem::OnTriggerButton));
	EventManager::Subscribe("OnResetScene", CreateListenerInfo(&BoxGeneratorSystem::OnResetScene));

}


void BoxGeneratorSystem::FixedUpdate(float deltaTime)
{
	for (BoxGenerator& boxGenerator : ComponentItr<BoxGenerator>())
	{
		if (boxGenerator.Sibling<Children>(m_GameObjectManager))
		{

			auto UI_IDs = m_GameObjectManager->QueryComponent<Children>(boxGenerator.GameObjectId)->ChildrenID;
			for (auto UI_ID : UI_IDs)
			{
				if (m_GameObjectManager->QueryComponent<SpriteController>(UI_ID)->bIsFinishTraversal)
				{
					m_GameObjectManager->QueryComponent<OnOff>(UI_ID)->IsOn = false;
				}

			}
		}
	}
}


void BoxGeneratorSystem::Start(uint32_t GameobjectID)
{
	auto boxgenerator = m_GameObjectManager->QueryComponent<BoxGenerator>(GameobjectID);
	if (boxgenerator)
	{
		auto boxGeneratorTransform = boxgenerator->Sibling<Transform>(m_GameObjectManager);
		VSM::Vector3 temppose = boxGeneratorTransform->WorldPosition;
		temppose += {0, 2, 0};
		boxgenerator->GeneratePose = temppose;
		boxgenerator->DoOnce = false;
	}
}

void BoxGeneratorSystem::OnTriggerButton(std::any data)
{
	uint32_t id = std::any_cast<uint32_t>(data);
	for (BoxGenerator& boxGenerator : ComponentItr<BoxGenerator>())
	{

		if (boxGenerator.chainedButtonID == id)
		{
			if (auto button = m_GameObjectManager->QueryComponent<Button>(id))

				if (button->IsActivated != true)
					return;
			if (!boxGenerator.DoOnce)
			{
				uint32_t tempID = Physics::GetInstance()->RaycastFromID_Nearest(boxGenerator.GameObjectId, { 0,1,0 }, 1.2f);
				//uint32_t tempID = Physics::GetInstance()->RaycastFromPosition_Nearest(boxGenerator.GeneratePose, { 0,-1,0 }, 0.2f);
				// 생성위치에 아무것도 존재하지 않을때
				if (!m_GameObjectManager->QueryComponent<Transform>(tempID))
				{
					SerializeSystem sSys{ m_GameObjectManager };

					uint32_t newBoxID = sSys.Instantiate("../Data/Prefab/BeaverBox.prefab", VSM::Vector3{ 0,-100,0 }, {});
					

					pair<uint32_t, uint32_t> newdata = make_pair(boxGenerator.GameObjectId, newBoxID);

					EventManager::ScheduleEvent("OnTriggerBox", newdata);
					auto UI_IDs = m_GameObjectManager->QueryComponent<Children>(boxGenerator.GameObjectId);
					if (UI_IDs)
					{
						for (auto UI_comp : UI_IDs->ChildrenID)
						{
							m_GameObjectManager->QueryComponent<OnOff>(UI_comp)->IsOn = true;
							m_GameObjectManager->QueryComponent<SpriteController>(UI_comp)->bIsReset = true;
						}
					}
					boxGenerator.DoOnce = true;	 ///한번 작동

					// 박스 생성장치 사운드
					Sound::GetInstance()->Play(boxGenerator.GameObjectId, "SFX_CreateBox");
				}
				// 생성위치에 태그가 존재할 때
				else if (m_GameObjectManager->QueryComponent<Tag>(tempID))
				{
					auto tag = m_GameObjectManager->QueryComponent<Tag>(tempID);
					switch (tag->tag)
					{
					case TagType::Cat:
					case TagType::Rabbit:
					case TagType::Beaver:
					{
						SerializeSystem sSys{ m_GameObjectManager };

						uint32_t newBoxID = sSys.Instantiate("../Data/Prefab/BeaverBox.prefab", VSM::Vector3{ 0,-100,0 }, {});
						pair<uint32_t, uint32_t> newdata = make_pair(boxGenerator.GameObjectId, newBoxID);

						EventManager::ScheduleEvent("OnTriggerBox", newdata);

						auto UI_IDs = m_GameObjectManager->QueryComponent<Children>(boxGenerator.GameObjectId);
						if (UI_IDs)
						{
							for (auto UI_comp : UI_IDs->ChildrenID)
							{
								m_GameObjectManager->QueryComponent<OnOff>(UI_comp)->IsOn = true;
								m_GameObjectManager->QueryComponent<SpriteController>(UI_comp)->bIsReset = true;
							}
						}

						boxGenerator.DoOnce = true;	 ///한번 작동

						// 박스 생성장치 사운드
						Sound::GetInstance()->Play(boxGenerator.GameObjectId, "SFX_CreateBox");
					}
					break;
					default:
						break;
					}
				}

			}

		}
	}
}

void BoxGeneratorSystem::OnResetScene(std::any data)
{
	for (BoxGenerator& boxGenerator : ComponentItr<BoxGenerator>())
	{
		boxGenerator.DoOnce = false;
	}
}





void BoxGeneratorSystem::Initialize()
{
	for (BoxGenerator& boxGenerator : ComponentItr<BoxGenerator>())
	{
		Start(boxGenerator.GameObjectId);
	}
}

void BoxGeneratorSystem::Finish(uint32_t gameObjectId)
{
}

void BoxGeneratorSystem::Finalize()
{
}
