#include "pch.h"
#include "ConveyorBeltSystem.h"
#include "ConveyorBelt.h"
#include "RigidBody.h"
#include "Tag.h"
#include "Transform.h"
#include "Box.h"
#include "ConveyorBeltPlane.h"
#include "UIController.h"
#include "Button.h"
#include "NeoConveyorBelt.h"
#include "Character.h"

static float deltaTime = 0.01667f;


ConveyorBeltSystem::ConveyorBeltSystem(GameObjectManager* gameObjectManager)
	: System(gameObjectManager)
{
	EventManager::Subscribe("OnCollisionStay", CreateListenerInfo(&ConveyorBeltSystem::OnCollisionStay));
	EventManager::Subscribe("OnTriggerButton", CreateListenerInfo(&ConveyorBeltSystem::OnTriggerButton));
	EventManager::Subscribe("OnExitButton", CreateListenerInfo(&ConveyorBeltSystem::OnExitButton));
}

void ConveyorBeltSystem::Start(uint32_t gameObjectId)
{
	auto conveyorBelt = m_GameObjectManager->QueryComponent<NeoConveyorBelt>(gameObjectId);
	if (conveyorBelt)
	{
		switch (conveyorBelt->beltDirection)
		{
			case eBeltDir::SOUTHEAST:
			{
				conveyorBelt->dir = { 0.0f, 0.0f, -1.0f };
				auto& transformRot = conveyorBelt->Sibling<Transform>(m_GameObjectManager)->LocalRotation;
				transformRot = Quaternion::LookRotation(conveyorBelt->dir, { 0,1,0 });
			}
			break;
			case eBeltDir::SOUTHWEST:
			{
				conveyorBelt->dir = { -1.0f, 0.0f, 0.0f };
				auto& transformRot = conveyorBelt->Sibling<Transform>(m_GameObjectManager)->LocalRotation;
				transformRot = Quaternion::LookRotation(conveyorBelt->dir, { 0,1,0 });
			}
			break;
			case eBeltDir::NORTHEAST:
			{
				conveyorBelt->dir = { 1.0f, 0.0f, 0.0f };
				auto& transformRot = conveyorBelt->Sibling<Transform>(m_GameObjectManager)->LocalRotation;
				transformRot = Quaternion::LookRotation(conveyorBelt->dir, { 0,1,0 });
			}
			break;
			case eBeltDir::NORTHWEST:
			{
				conveyorBelt->dir = { 0.0f, 0.0f, 1.0f };
				auto& transformRot = conveyorBelt->Sibling<Transform>(m_GameObjectManager)->LocalRotation;
				VSM::Vector3 tempDir = conveyorBelt->dir;
				tempDir.z *= -1;
				transformRot = Quaternion::LookRotation(tempDir, { 0,1,0 });
			}
			break;
		}

		for (ConveyorBeltPlane& plane : ComponentItr<ConveyorBeltPlane>())
		{
			if (plane.chainedBeltID != conveyorBelt->GameObjectId)
				continue;

			VSM::Vector3 dir = conveyorBelt->dir;
			{
				dir.x *= -1;
				dir.z *= -1;
			}
			auto& planeTransformRot = plane.Sibling<Transform>(m_GameObjectManager)->LocalRotation;
			if (conveyorBelt->beltDirection == eBeltDir::SOUTHEAST)
			{
				planeTransformRot = { 0,1,0,0 };
			}
			else
			{
				planeTransformRot = Quaternion::LookRotation(dir, { 0,1,0 });
			}
		}
	}
}

void ConveyorBeltSystem::OnCollisionStay(std::any data)
{
	const auto [lhs, rhs] = any_cast<pair<uint32_t, uint32_t>>(data);
	const auto conveyorBelt = m_GameObjectManager->QueryComponent<NeoConveyorBelt>(lhs);

	if (conveyorBelt)
	{
		const auto& conveyorPos = conveyorBelt->Sibling<Transform>(m_GameObjectManager)->LocalPosition;
		if (conveyorBelt->isWorking)
		{
			const auto objectTag = m_GameObjectManager->QueryComponent<Tag>(rhs);

			switch (objectTag->tag)
			{
				case TagType::Beaver:
				case TagType::Cat:
				case TagType::Rabbit:
				{
					auto rigidBody = objectTag->Sibling<RigidBody>(m_GameObjectManager);
					auto& objectPos = objectTag->Sibling<Transform>(m_GameObjectManager)->LocalPosition;

					bool isObjectUp = (fabs(objectPos.x - conveyorPos.x) < 1.0f) && (fabs(objectPos.z - conveyorPos.z) < 1.0f);

					VSM::Vector3 beforePos = objectPos;

					if (isObjectUp)
					{
						objectPos += conveyorBelt->beltForce * conveyorBelt->dir * deltaTime;
					}

					if (beforePos == objectPos)
					{
						objectPos += conveyorBelt->beltForce * conveyorBelt->dir * deltaTime * 5;
					}
				}
				break;
				case TagType::Box:
				{
					auto rigidBody = objectTag->Sibling<RigidBody>(m_GameObjectManager);
					auto& objectPos = objectTag->Sibling<Transform>(m_GameObjectManager)->LocalPosition;

					bool isObjectUp = (fabs(objectPos.x - conveyorPos.x) < 0.1f) && (fabs(objectPos.z - conveyorPos.z) < 0.1f);
					if ((rigidBody != nullptr) && isObjectUp)
					{

						pair<uint32_t, uint32_t> newdata = make_pair(conveyorBelt->GameObjectId, rigidBody->GameObjectId);
						EventManager::ScheduleEvent("OnTriggerBox", newdata);

					}
				}
				break;
				default:
					break;
			}
		}
	}
}

void ConveyorBeltSystem::OnTriggerButton(std::any data)
{
	uint32_t id = std::any_cast<uint32_t>(data);

	for (NeoConveyorBelt& conveyorBelt : ComponentItr<NeoConveyorBelt>())
	{
		if (conveyorBelt.chainedButtonID != id)
			continue;

		if (Button* button = m_GameObjectManager->QueryComponent<Button>(id))
		{
			if (button->IsActivated)
			{
				switch (conveyorBelt.beltDirection)
				{
					case eBeltDir::NORTHEAST:
					{
						conveyorBelt.beltDirection = eBeltDir::SOUTHWEST;
						conveyorBelt.dir *= -1;
					}
					break;
					case eBeltDir::NORTHWEST:
					{
						conveyorBelt.beltDirection = eBeltDir::SOUTHEAST;
						conveyorBelt.dir *= -1;
					}
					break;
					case eBeltDir::SOUTHEAST:
					{
						conveyorBelt.beltDirection = eBeltDir::NORTHWEST;
						conveyorBelt.dir *= -1;
					}
					break;
					case eBeltDir::SOUTHWEST:
					{
						conveyorBelt.beltDirection = eBeltDir::NORTHEAST;
						conveyorBelt.dir *= -1;
					}
					break;
				}

				for (ConveyorBeltPlane& plane : ComponentItr<ConveyorBeltPlane>())
				{
					if (plane.chainedBeltID != conveyorBelt.GameObjectId)
						continue;

					VSM::Vector3 dir = conveyorBelt.dir;
					{
						dir.x *= -1;
						dir.z *= -1;
					}
					auto& planeTransformRot = plane.Sibling<Transform>(m_GameObjectManager)->LocalRotation;
					if (conveyorBelt.beltDirection == eBeltDir::SOUTHEAST)
					{
						planeTransformRot = { 0,1,0,0 };
					}
					else
					{
						planeTransformRot = Quaternion::LookRotation(dir, { 0,1,0 });
					}
				}
			}
		}

	}

}

void ConveyorBeltSystem::OnExitButton(std::any data)
{
	uint32_t id = std::any_cast<uint32_t>(data);

	for (NeoConveyorBelt& conveyorBelt : ComponentItr<NeoConveyorBelt>())
	{
		if (conveyorBelt.chainedButtonID != id)
			continue;

		if (Button* button = m_GameObjectManager->QueryComponent<Button>(id))
		{
			if (!button->IsActivated)
			{
				switch (conveyorBelt.beltDirection)
				{
					case eBeltDir::NORTHEAST:
					{
						conveyorBelt.beltDirection = eBeltDir::SOUTHWEST;
						conveyorBelt.dir *= -1;
					}
					break;
					case eBeltDir::NORTHWEST:
					{
						conveyorBelt.beltDirection = eBeltDir::SOUTHEAST;
						conveyorBelt.dir *= -1;
					}
					break;
					case eBeltDir::SOUTHEAST:
					{
						conveyorBelt.beltDirection = eBeltDir::NORTHWEST;
						conveyorBelt.dir *= -1;
					}
					break;
					case eBeltDir::SOUTHWEST:
					{
						conveyorBelt.beltDirection = eBeltDir::NORTHEAST;
						conveyorBelt.dir *= -1;
					}
					break;
				}

				for (ConveyorBeltPlane& plane : ComponentItr<ConveyorBeltPlane>())
				{
					if (plane.chainedBeltID != conveyorBelt.GameObjectId)
						continue;

					VSM::Vector3 dir = conveyorBelt.dir;
					{
						dir.x *= -1;
						dir.z *= -1;
					}
					auto& planeTransformRot = plane.Sibling<Transform>(m_GameObjectManager)->LocalRotation;
					if (conveyorBelt.beltDirection == eBeltDir::SOUTHEAST)
					{
						planeTransformRot = { 0,1,0,0 };
					}
					else
					{
						planeTransformRot = Quaternion::LookRotation(dir, { 0,1,0 });
					}
				}
			}
		}
	}
}

void ConveyorBeltSystem::Initialize()
{
	for (NeoConveyorBelt& cb : ComponentItr<NeoConveyorBelt>())
	{
		Start(cb.GameObjectId);
	}
}

void ConveyorBeltSystem::Finish(uint32_t gameObjectId)
{
}

void ConveyorBeltSystem::Finalize()
{
}
