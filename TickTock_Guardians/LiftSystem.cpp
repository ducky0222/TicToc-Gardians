#include "pch.h"
#include "LiftSystem.h"
#include "Lift.h"
#include "Input.h"
#include "Transform.h"
#include "OnOff.h"
#include "Button.h"

LiftSystem::LiftSystem(GameObjectManager* goManager) :
	System(goManager)
{
	/// ���⿡ �̺�Ʈ ����
	EventManager::Subscribe("OnTriggerButton", CreateListenerInfo(&LiftSystem::OnTriggerButton));
	EventManager::Subscribe("OnExitButton", CreateListenerInfo(&LiftSystem::OnExitButton));

}

void LiftSystem::OnTriggerButton(std::any data)
{
	/// ���⿡ ��ư�� ĳ���Ͱ� ����� ��� ����

	Button* button = m_GameObjectManager->QueryComponent<Button>(std::any_cast<uint32_t>(data));

	for (Lift& lift : ComponentItr<Lift>())
	{
		if (lift.ChainedButtonId == std::any_cast<uint32_t>(data))
		{
			if (button->IsActivated)
				lift.IsLiftActive = true;
		}
	}
}

void LiftSystem::OnExitButton(std::any data)
{
	Button* button = m_GameObjectManager->QueryComponent<Button>(std::any_cast<uint32_t>(data));

	/// ���⿡ ��ư���� ĳ���Ͱ� �������� ��� ����
	for (Lift& lift : ComponentItr<Lift>())
	{
		if (lift.ChainedButtonId == std::any_cast<uint32_t>(data))
		{
			if (!button->IsActivated)
				lift.IsLiftActive = false;
		}
	}
}


void LiftSystem::Initialize()
{
	for (Lift& lift : ComponentItr<Lift>())
	{
		Start(lift.GameObjectId);
	}
}

void LiftSystem::Start(uint32_t id)
{
	auto lift = m_GameObjectManager->QueryComponent<Lift>(id);
	auto transform = m_GameObjectManager->QueryComponent<Transform>(id);

	if (lift && transform)
	{
		lift->StartPosition = transform->LocalPosition;
	}
}

void LiftSystem::Finish(uint32_t id)
{

}

void LiftSystem::Finalize()
{

}

void LiftSystem::FixedUpdate(float deltaTime)
{
	for (Lift& lift : ComponentItr<Lift>())
	{
		if (!lift.Sibling<OnOff>(m_GameObjectManager)->IsOn) continue;

		if (lift.IsLiftActive)
		{
			if (lift.MaxLiftDistance > 0)
			{
				auto transform = m_GameObjectManager->QueryComponent<Transform>(lift.GameObjectId);
				if (transform)
				{
					VSM::Vector3 moveDir = Vector3(0, 0, 0);
					if (lift.LiftDirectionUp)
					{
						moveDir.y = lift.LiftSpeed * deltaTime;
					}
					else
					{
						moveDir.y = -lift.LiftSpeed * deltaTime;
					}

					if (lift.LiftDistance < lift.MaxLiftDistance)
					{
						transform->LocalPosition = transform->LocalPosition + moveDir;
						lift.LiftDistance += fabs(moveDir.y);
					}
					else
					{
						transform->LocalPosition = lift.StartPosition + Vector3(0, lift.MaxLiftDistance * (lift.LiftDirectionUp ? 1 : -1), 0);
						lift.LiftDistance = lift.MaxLiftDistance;
					}

				}
			}
		}
		else
		{
			if (lift.LiftDistance > 0)
			{
				auto transform = m_GameObjectManager->QueryComponent<Transform>(lift.GameObjectId);
				if (transform)
				{
					VSM::Vector3 moveDir = Vector3(0, 0, 0);
					if (lift.LiftDirectionUp)
					{
						moveDir.y = -lift.LiftSpeed * deltaTime;
					}
					else
					{
						moveDir.y = lift.LiftSpeed * deltaTime;
					}

					if (lift.LiftDistance >= 0)
					{
						transform->LocalPosition = transform->LocalPosition + moveDir;
						lift.LiftDistance -= fabs(moveDir.y);
					}
					else
					{
						transform->LocalPosition = lift.StartPosition;
						lift.LiftDistance = 0;
					}

				}
			}
		}
	}
}
