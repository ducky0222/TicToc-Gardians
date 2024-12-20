#include "pch.h"
#include "CharacterBarrierSystem.h"
#include "CharacterBarrier.h"
#include "HysxEngine.h"
#include "Character.h"
#include "Tag.h"
#include "RigidBody.h"
#include "Transform.h"

CharacterBarrierSystem::CharacterBarrierSystem(GameObjectManager* goManager)
	: System(goManager)
{

}

void CharacterBarrierSystem::Start(uint32_t gameObjectId)
{

}

void CharacterBarrierSystem::Initialize()
{
	for (CharacterBarrier& characterBarrier : System::ComponentItr<CharacterBarrier>())
	{
		Character* character = characterBarrier.Sibling<Character>(m_GameObjectManager);
		if (character == nullptr)
			continue;
		characterBarrier.CharacterSpeed = character->Speed;
	}
}

void CharacterBarrierSystem::Finish(uint32_t gameObjectId)
{

}

void CharacterBarrierSystem::Finalize()
{

}

void CharacterBarrierSystem::FixedUpdate(float deltaTime)
{
	for (CharacterBarrier& characterBarrier : System::ComponentItr<CharacterBarrier>())
	{
		Character* character = characterBarrier.Sibling<Character>(m_GameObjectManager);
		if (character == nullptr)
			continue;

		Transform* transform = character->Sibling<Transform>(m_GameObjectManager);

		auto& localQ = transform->LocalRotation;


		VSM::Vector3 offset1 = VSM::Vector3{ -1.f * characterBarrier.BarrierOffsetSize.x, 0.3f, -1.f * characterBarrier.BarrierOffsetSize.z };
		VSM::Vector3 offset2 = VSM::Vector3{ 1.f * characterBarrier.BarrierOffsetSize.x, 0.3f, -1.f * characterBarrier.BarrierOffsetSize.z };


		offset1 = VSM::Vector3::Transform(offset1, localQ);
		offset2 = VSM::Vector3::Transform(offset2, localQ);
		VSM::Vector3 offsetDir = offset2 - offset1;
		float distance = offsetDir.Length();
		offsetDir.Normalize();

		auto vec1 = Physics::GetInstance()->RaycastFromPositions(offset1 + transform->WorldPosition, offsetDir, distance);

		VSM::Vector3 offset3 = offset1 + VSM::Vector3{ 0.f, -1.8f, 0.f };
		VSM::Vector3 offset4 = offset2 + VSM::Vector3{ 0.f, -1.8f, 0.f };

		offsetDir = offset4 - offset3;
		distance = offsetDir.Length();
		offsetDir.Normalize();

		auto ved2 = Physics::GetInstance()->RaycastFromPositions(offset3 + transform->WorldPosition, offsetDir, distance);

		if (vec1.size() > 0 || ved2.size() > 0)
		{
			for (uint32_t id : vec1)
			{
				character->Speed = characterBarrier.CharacterSpeed;

				auto layer = m_GameObjectManager->QueryComponent<RigidBody>(id)->ePhysicLayer;
				if (layer == ePxLayer::TILE_LAYER)
				{
					character->Speed = 0.f;
					break;
				}
				else if (layer == ePxLayer::TRIGGER_TILE_LAYER)
				{
					character->Speed = 0.f;
					break;
				}
			}
			for (uint32_t id : ved2)
			{
				character->Speed = characterBarrier.CharacterSpeed;

				auto layer = m_GameObjectManager->QueryComponent<RigidBody>(id)->ePhysicLayer;
				if (layer == ePxLayer::TILE_LAYER)
				{
					character->Speed = 0.f;
					break;
				}
				else if (layer == ePxLayer::TRIGGER_TILE_LAYER)
				{
					character->Speed = 0.f;
					break;
				}
			}
		}
		else
		{
			character->Speed = characterBarrier.CharacterSpeed;
		}
	}

}
