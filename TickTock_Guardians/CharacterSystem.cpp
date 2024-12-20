#include "pch.h"
#include "CharacterSystem.h"

#include "Input.h"
#include "Character.h"
#include "Replayer.h"
#include "RigidBody.h"
#include "Tag.h"
#include "HysxEngine.h"
#include "TransformSystem.h"
#include "Transform.h"
#include "OnOff.h"

#include <SerializeSystem.h>

#include "LandingPoint.h"
#include <AnimationSystem.h>

#include "CollisionCustom.h"
#include "JumpParameter.h"
#include "../SoundEngine/ISoundEngine.h"
#include "GroundCheckHelper.h"

CharacterSystem::CharacterSystem(GameObjectManager* gameObjectManager) :
	System(gameObjectManager)
{
	EventManager::Subscribe("OnCollisionEnter", CreateListenerInfo(&CharacterSystem::OnCollisionEnter));
	EventManager::Subscribe("OnCollisionExit", CreateListenerInfo(&CharacterSystem::OnCollisionExit));
	EventManager::Subscribe("OnStartMoving", CreateListenerInfo(&CharacterSystem::OnStartMoving));
	EventManager::Subscribe("OnStartReplay", CreateListenerInfo(&CharacterSystem::OnStartReplay));
	EventManager::Subscribe("OnFinishTracing", CreateListenerInfo(&CharacterSystem::OnFinishTracing));
	EventManager::Subscribe("OnJump", CreateListenerInfo(&CharacterSystem::OnJump));
	EventManager::Subscribe("OnUnlockInput", CreateListenerInfo(&CharacterSystem::OnUnlockInput));
	EventManager::Subscribe("SetIsSpawningFalse", CreateListenerInfo(&CharacterSystem::SetIsSpawningFalse));
	EventManager::Subscribe("OnSpawnBox", CreateListenerInfo(&CharacterSystem::OnSpawnBox));
	//EventManager::Subscribe("OnSpawnRandomStepEffect", CreateListenerInfo(&CharacterSystem::OnSpawnRandomStepEffect));
	//EventManager::Subscribe("OnSpawnLandingEffect", CreateListenerInfo(&CharacterSystem::OnSpawnLandingEffect));
	EventManager::Subscribe("OnUndo", CreateListenerInfo(&CharacterSystem::OnUndo));
	EventManager::Subscribe("OnReady", CreateListenerInfo(&CharacterSystem::OnReady));
}

void CharacterSystem::Update(float deltaTime)
{
	for (Character& character : ComponentItr<Character>())
	{
		if (!character.Sibling<OnOff>(m_GameObjectManager)->IsOn)
			continue;


		if (!character.IsControllable)
			continue;
#ifdef _DEBUG
		IsGround(character.Sibling<RigidBody>(m_GameObjectManager));
#endif

		if (Input::GetKey(KEY::LEFT))
		{
			character.Direction += VSM::Vector3(-0.99f, 0.f, 0.99f);
			character.DirectionBitset |= 1 << static_cast<uint32_t>(ACTION::LEFT);
		}
		if (Input::GetKey(KEY::RIGHT))
		{
			character.Direction += VSM::Vector3(0.99f, 0, -0.99f);
			character.DirectionBitset |= 1 << static_cast<uint32_t>(ACTION::RIGHT);
		}
		if (Input::GetKey(KEY::UP))
		{
			character.Direction += VSM::Vector3(1.f, 0, 1.f);
			character.DirectionBitset |= 1 << static_cast<uint32_t>(ACTION::UP);
		}
		if (Input::GetKey(KEY::DOWN))
		{
			character.Direction += VSM::Vector3(-1.f, 0, -1.f);
			character.DirectionBitset |= 1 << static_cast<uint32_t>(ACTION::DOWN);
		}
		if (character.IsJumpAble && !character.IsJumping && Input::GetKey(KEY::SPACE))
		{
			character.DirectionBitset |= 1 << static_cast<uint32_t>(ACTION::JUMP);
		}
		if (Input::GetKey(KEY::B))
		{
			character.DirectionBitset |= 1 << static_cast<uint32_t>(ACTION::BOX);
		}
		if (Input::GetKey(KEY::X))
		{
			character.DirectionBitset |= 1 << static_cast<uint32_t>(ACTION::SKIP);
			character.IsControllable = false;
			character.IsMovable = false;
			EventManager::ScheduleEvent("OnSkipTracing", character.GameObjectId);
			continue;
		}

		character.Direction.Normalize();
	}
}

void CharacterSystem::FixedUpdate(float deltaTime)
{


	for (Character& character : ComponentItr<Character>())
	{
		if (!character.Sibling<OnOff>(m_GameObjectManager)->IsOn)
			continue;

		if (!character.IsJumpAble)
		{
			if (character.JumpCoolDownFrame < character.JumpCoolDown)
			{
				character.JumpCoolDownFrame++;
			}
			else
			{
				character.IsJumpAble = true;
				character.JumpCoolDownFrame = 0;
			}
		}

		if (character.IsJumping)
		{
			character.JumpAdjustDelay++;
		}

		if (!character.IsMovable)
			continue;

		auto rigidbody = character.Sibling<RigidBody>(m_GameObjectManager);
		assert(rigidbody);

		character.IsArrowDown = character.Direction != VSM::Vector3::Zero;

		///캐릭터 이동로직
		Vector3 movement = {};
		character.Direction.Normalize();

		movement += character.Direction * character.Speed * deltaTime;
		character.Sibling<Transform>(m_GameObjectManager)->LocalPosition += movement;
		rigidbody->Velocity.x = 0;
		rigidbody->Velocity.z = 0;

		if (character.DirectionBitset[static_cast<uint32_t>(ACTION::JUMP)])
		{
			EventManager::PublishImmediate("OnJump", character.GameObjectId);
		}

		if (rigidbody->Velocity.y < 0)
		{
			if (JumpParameter* jParam = rigidbody->Sibling<JumpParameter>(m_GameObjectManager))
			{
				VSM::Vector3 globalG = Physics::GetInstance()->GetGravity();
				float gravityMagnitude = globalG.Length();
				rigidbody->Velocity.y -= jParam->GravityMultiplier * gravityMagnitude * deltaTime;
			}
		}
		// 		else if (rigidbody->Velocity.y > 0)
		// 		{
		// 			if (JumpParameter* jParam = rigidbody->Sibling<JumpParameter>(m_GameObjectManager))
		// 			{
		// 
		// 				VSM::Vector3 globalG = Physics::GetInstance()->GetGravity();
		// 				float gravityMagnitude = globalG.Length();
		// 
		// 				rigidbody->Velocity.y += -jParam->JumpTimeUp * gravityMagnitude * deltaTime;
		// 			}
		/*		}*/
		if (character.DirectionBitset[static_cast<uint32_t>(ACTION::BOX)])
		{
			EventManager::PublishImmediate("OnSpawnBox", character.GameObjectId);
			EventManager::PublishImmediate("OnSpawnLandingEffect", character.GameObjectId);

		}

		if (!character.IsJumping && movement.Length() > 0.005f)
		{
			if (character.FootprintTime > character.FootprintCoolTime)
			{
				EventManager::PublishImmediate("OnSpawnRandomStepEffect", character.GameObjectId);
				character.FootprintTime = 0.f;
			}
			character.FootprintTime += deltaTime;
		}
		else
			character.FootprintTime = 0;






		// Replayer System 이 수신할 이벤트
		EventManager::PublishImmediate("OnTraceDirection", std::make_pair(character.GameObjectId, character.DirectionBitset));

		VSM::Vector2 curDir = { character.Direction.x, character.Direction.z };

		character.Direction = Vector3::Zero;
		character.DirectionBitset = 0;

		if (curDir.Length() > 0.5f)
		{
			auto& localQ = character.Sibling<Transform>(m_GameObjectManager)->LocalRotation;
			localQ = Quaternion::LookRotation({ curDir.x, 0, curDir.y }, { 0,1,0 });
		}
	}
}

void CharacterSystem::OnCollisionEnter(std::any data)
{
	const auto [lhs, rhs] = any_cast<std::pair<uint32_t, uint32_t>>(data);

	const auto characterMovement = m_GameObjectManager->QueryComponent<Character>(lhs);

	if (!characterMovement)
		return;

	const auto rhsLayer = m_GameObjectManager->QueryComponent<RigidBody>(rhs)->ePhysicLayer;

	if (!rhsLayer)
		return;

	const auto lhsEvent = m_GameObjectManager->QueryComponent<CollisionCustom>(lhs);
	const auto rhsEvent = m_GameObjectManager->QueryComponent<CollisionCustom>(rhs);

	if (lhsEvent)
		EventManager::ScheduleEvent(lhsEvent->OnCollisionEnter);
	if (rhsEvent)
		EventManager::ScheduleEvent(rhsEvent->OnCollisionEnter);

	switch (rhsLayer)
	{
	case TILE_LAYER:
	case TRIGGER_TILE_LAYER:
	{
		if (IsGround(m_GameObjectManager->QueryComponent<RigidBody>(lhs)))
		{
			if (characterMovement->IsJumping and characterMovement->JumpAdjustDelay > 5)
			{
				characterMovement->IsJumping = false;
				characterMovement->IsJumpAble = false;
				characterMovement->JumpAdjustDelay = 0;
			}
		}
		auto rhsTagComponent = m_GameObjectManager->QueryComponent<Tag>(rhs);

		if (!rhsTagComponent)
			break;

		switch (rhsTagComponent->tag)
		{
		case TagType::LandingPoint:
		{
			EventManager::ScheduleEvent("OnTriggerLandingPoint", data);
			break;
		}

		case TagType::Box:
		{
			EventManager::ScheduleEvent("OnTriggerBox", data);
			break;
		}
		case TagType::Button:
		{
			EventManager::ScheduleEvent("OnButtonClicked", rhs);
			break;
		}
		case TagType::Lift:
		{

			if (IsGround(lhs, rhs))
			{
				ParentChildrenSystem(m_GameObjectManager).AddChild(rhs, lhs);
			}
			break;
		}
		default:
			break;
		}
	}
	break;

	case NO_COLLISION_LAYER:
	{
		auto rhsTagComponent = m_GameObjectManager->QueryComponent<Tag>(rhs);

		if (!rhsTagComponent)
			break;

		switch (rhsTagComponent->tag)
		{
		case TagType::DimensionCrack:
		{
			EventManager::ScheduleEvent("OnTriggerDimensionCrack", data);
			break;
		}
		default:
			break;
		}
	}
	break;

	default: // 캐릭터 간의 충돌
		break;
	}
}

void CharacterSystem::OnCollisionExit(std::any data)
{
	const auto [lhs, rhs] = any_cast<std::pair<uint32_t, uint32_t>>(data);

	const auto character = m_GameObjectManager->QueryComponent<Character>(lhs);

	if (!character)
		return;

	const auto rhsLayer = m_GameObjectManager->QueryComponent<RigidBody>(rhs)->ePhysicLayer;
	const auto rhsTagComponent = m_GameObjectManager->QueryComponent<RigidBody>(rhs)->Sibling<Tag>(m_GameObjectManager);

	const auto lhsEvent = m_GameObjectManager->QueryComponent<CollisionCustom>(lhs);
	const auto rhsEvent = m_GameObjectManager->QueryComponent<CollisionCustom>(rhs);

	if (lhsEvent)
		EventManager::ScheduleEvent(lhsEvent->OnCollisionExit);
	if (rhsEvent)
		EventManager::ScheduleEvent(rhsEvent->OnCollisionExit);

	switch (rhsLayer)
	{
	case TILE_LAYER:
	case TRIGGER_TILE_LAYER:
	{
		if (!IsGround(character->Sibling<RigidBody>(m_GameObjectManager)))
			character->IsJumping = true;

		if (!rhsTagComponent)
			break;

		switch (rhsTagComponent->tag)
		{
		case TagType::Box:
		{
			EventManager::ScheduleEvent("OnExitBox", data);
			break;
		}
		case TagType::Button:
		{
			EventManager::ScheduleEvent("OnButtonUnClicked", rhs);
			break;
		}
		case TagType::Lift:
		{
			ParentChildrenSystem(m_GameObjectManager).RemoveChild(rhs, lhs);
			break;
		}
		default:
			break;
		}

	}
	break;

	case NO_COLLISION_LAYER:
	{
		if (!rhsTagComponent)
			break;

		switch (rhsTagComponent->tag)
		{
		case TagType::DimensionCrack:
		{
			EventManager::ScheduleEvent("OnExitDimensionCrack", data);
			break;
		}
		default:
			break;
		}
	}
	break;

	default: // 캐릭터 간의 충돌
	{
		break;
	}
	}
}

void CharacterSystem::OnStartMoving(std::any data)
{
	auto id = std::any_cast<uint32_t>(data);

	auto character = m_GameObjectManager->QueryComponent<Character>(id);

	if (!character)
		return;

	VSM::Vector3 startingPosition = character->Sibling<Character>(m_GameObjectManager)->OriginPosition;

	for (LandingPoint& landing : ComponentItr<LandingPoint>())
	{
		if (landing.IsCurrentStartPoint)
		{
			startingPosition = landing.Sibling<Transform>(m_GameObjectManager)->WorldPosition;
			startingPosition.y += 3.f;
		}
	}

	character->Sibling<Transform>(m_GameObjectManager)->LocalPosition = startingPosition;


	character->IsControllable = true;
	character->IsMovable = true;

	character->Direction = VSM::Vector3::Zero;
	character->DirectionBitset = 0;
}

void CharacterSystem::OnFinishTracing(std::any data)
{
	auto id = std::any_cast<uint32_t>(data);

	auto character = m_GameObjectManager->QueryComponent<Character>(id);

	if (!character)
		return;

	auto replayer = m_GameObjectManager->QueryComponent<Replayer>(id);

	if (!replayer)
		return;

	character->IsControllable = false;
	character->IsMovable = false;

	// 	character->Direction = VSM::Vector3::Zero;
	// 	character->IsArrowDown = false;
	// 	ParentChildrenSystem(m_GameObjectManager).RemoveParent(id);
	// 	character->Sibling<Transform>(m_GameObjectManager)->LocalPosition = replayer->StartPosition;
	// 	character->Sibling<Transform>(m_GameObjectManager)->LocalRotation = replayer->StartRotation;


}

void CharacterSystem::OnJump(std::any data)
{
	auto id = std::any_cast<uint32_t>(data);

	auto character = m_GameObjectManager->QueryComponent<Character>(id);

	if (!character)
		return;

	auto rigidbody = character->Sibling<RigidBody>(m_GameObjectManager);

	if (!character->IsJumping)
	{
		auto jumpParam = character->Sibling<JumpParameter>(m_GameObjectManager);
		if (jumpParam)
		{
			VSM::Vector3 globalG = Physics::GetInstance()->GetGravity();
			float gravityMagnitude = globalG.Length();

			// 점프 높이를 계산합니다.
			float jumpHeight = jumpParam->JumpHeight;

			// 점프에 필요한 초기 속도(v₀)를 계산합니다.
			float initialVelocityMagnitude = sqrt(2 * gravityMagnitude * jumpHeight);

			// 초기 속도(v₀)를 이용하여 점프합니다.
			Physics::GetInstance()->AddVelocity(id, { 0.f, 1.f, 0.f }, initialVelocityMagnitude);

		}
		else
		{
			Physics::GetInstance()->AddVelocity(id, { 0.f, 1.f, 0.f }, character->JumpForce);
		}

		character->IsJumping = true;

		// 점프 사운드
		Sound::GetInstance()->Play(id, "SFX_JUMP_3");
	}
}

void CharacterSystem::OnSpawnBox(std::any data)
{
	uint32_t id = std::any_cast<uint32_t>(data);
	auto character = m_GameObjectManager->QueryComponent<Character>(id);
	auto charactertag = character->Sibling<Tag>(m_GameObjectManager);

	if (charactertag->tag != TagType::Beaver)
	{
		return;
	}

	auto createBox = [&]()
		{
			auto characterTransform = character->Sibling<Transform>(m_GameObjectManager);
			Vector3 direction = Vector3::Transform(-Vector3::UnitZ, characterTransform->LocalRotation);
			uint32_t Front2mID = Physics::GetInstance()->RaycastFromID_Offset_Nearest(character->GameObjectId, direction * 2.2, { 0,-1,0 }, 10.f);
			uint32_t InfrontObject = Physics::GetInstance()->RaycastFromID_Nearest(character->GameObjectId, direction , 2.2f);

			if (InfrontObject==0&&Front2mID != 0 && character->DoOnceBoxSpawn)
			{
				auto spawnGround = m_GameObjectManager->QueryComponent<Transform>(Front2mID);
				Vector3 SpawnPose = spawnGround->WorldPosition;
				SpawnPose.y = character->Sibling<Transform>(m_GameObjectManager)->WorldPosition.y;
				SerializeSystem sSys{ m_GameObjectManager };
				uint32_t newBoxID = sSys.Instantiate("../Data/Prefab/BeaverBox.prefab", SpawnPose, {});
				character->IsSpawning = true;
				character->DoOnceBoxSpawn = false;
				Sound::GetInstance()->Play(id, "SFX_CreateBox");

				return true;
			}

			return false;
		};

	// 리플레이어 핸들링
	auto replayer = m_GameObjectManager->QueryComponent<Replayer>(id);

	if (replayer->IsReplaying)
	{
		createBox();
		EventManager::ScheduleEvent("SetIsSpawningFalse", character->GameObjectId, 1.625f);
		return;
	}

	// 컨트롤러 핸들링
	if (character->IsControllable)
	{
		if (createBox())
		{
			character->IsControllable = false;
			EventManager::ScheduleEvent("OnUnlockInput", character->GameObjectId, 1.625f);///TODO:: 애니메이션 을 위해서 나중에 false시켜주기.
			EventManager::ScheduleEvent("SetIsSpawningFalse", character->GameObjectId, 1.625f);
		}
	}
}

//void CharacterSystem::OnSpawnRandomStepEffect(std::any data)
//{
//	uint32_t gameobjectId = std::any_cast<uint32_t>(data);
//
//	auto character = m_GameObjectManager->QueryComponent<Character>(gameobjectId);
//	if (!character)
//		return;
//
//	std::random_device rd;  // 난수 생성을 위한 하드웨어 장치
//	std::mt19937 gen(rd()); // 난수 생성 알고리즘 (Mersenne Twister)
//	std::uniform_int_distribution<> distrib(1, 3); // 범위 1~3의 균일 분포
//	int randomNum = distrib(gen); // 1~8 사이의 난수 생성
//	AnimationSystem::SpriteEffectCreateData SECdata;
//	SECdata.EffectDuration = 0.45; // 0.05*9개
//	SECdata.Offset = {0,-1,0};
//	SECdata.Position = character->Sibling<Transform>(m_GameObjectManager)->WorldPosition;
//	switch (randomNum)
//	{
//	case 1:
//	{
//		SECdata.EffectPrefabName = "Effect_Walk_A.prefab";
//		EventManager::ScheduleEvent("OnSpriteEffectCreate", SECdata);
//	}
//		break;
//	case 2:
//	{
//		SECdata.EffectPrefabName = "Effect_Walk_B.prefab";
//		EventManager::ScheduleEvent("OnSpriteEffectCreate", SECdata);
//	}
//		break;
//	case 3:
//	{
//		SECdata.EffectPrefabName = "Effect_Walk_C.prefab";
//		EventManager::ScheduleEvent("OnSpriteEffectCreate", SECdata);
//	}
//		break;
//	default:
//		break;
//	}
//} 

/*void CharacterSystem::OnSpawnLandingEffect(std::any data)
{
	uint32_t gameobjectId = std::any_cast<uint32_t>(data);

	auto character = m_GameObjectManager->QueryComponent<Character>(gameobjectId);
	if (!character)
		return;

	AnimationSystem::SpriteEffectCreateData SECdata1;
	AnimationSystem::SpriteEffectCreateData SECdata2;
	SECdata1.EffectDuration = 0.18f; // 이펙트 루프 1회기준
	SECdata1.Offset = { -1.228,-1,0.392 };
	SECdata1.Position = character->Sibling<Transform>(m_GameObjectManager)->WorldPosition;
	SECdata2.EffectDuration = 0.18f; // 이펙트 루프 1회기준
	SECdata2.Offset = { 0.392,-1,-1.228 };
	SECdata2.Position = character->Sibling<Transform>(m_GameObjectManager)->WorldPosition;

	SECdata1.EffectPrefabName = "Effect_PlayerLanding_left.prefab";
	EventManager::ScheduleEvent("OnSpriteEffectCreate", SECdata1);
	SECdata2.EffectPrefabName = "Effect_PlayerLanding_right.prefab";
	EventManager::ScheduleEvent("OnSpriteEffectCreate", SECdata2);
}*/

bool CharacterSystem::IsGround(uint32_t characterID, uint32_t groundID)
{
	auto character = m_GameObjectManager->QueryComponent<RigidBody>(characterID);

	if (!character)
		return false;

	auto ground = character->Sibling<RigidBody>(m_GameObjectManager);

	if (!ground)
		return false;

	float offset = 0.4f;
	float distance = 1.6f;
	GroundCheckHelper* groundCheck = character->Sibling<GroundCheckHelper>(m_GameObjectManager);
	if (groundCheck)
	{
		offset = groundCheck->Offset;
		distance = groundCheck->GroundCheckDistance;
	}


	VSM::Vector3 direction[4] = { {1.f, 0, 1.f}, {-1.f, 0, 1.f}, {1.f, 0, -1.f}, {-1.f, 0, -1.f} };

	uint32_t id[4] = { 0, 0, 0, 0 };

	for (int i = 0; i < 4; i++)
	{
		id[i] = Physics::GetInstance()->RaycastFromID_Offset_Nearest(characterID, direction[i] * offset, { 0,-1,0 }, distance);
	}



	return id[0] == groundID || id[1] == groundID || id[2] == groundID || id[3] == groundID;
}

bool CharacterSystem::IsGround(RigidBody* character)
{
	if (!character)
		return false;


 	float offset = 0.4f;
	float distance = 1.6f;
	GroundCheckHelper* groundCheck = character->Sibling<GroundCheckHelper>(m_GameObjectManager);
	if (groundCheck)
	{
		offset = groundCheck->Offset;
		distance = groundCheck->GroundCheckDistance;
	}

	VSM::Vector3 direction[4] = { {1.f, 0, 1.f}, {-1.f, 0, 1.f}, {1.f, 0, -1.f}, {-1.f, 0, -1.f} };
	uint32_t id[4] = { 0, 0, 0, 0 };

	for (int i = 0; i < 4; i++)
	{
		id[i] = Physics::GetInstance()->RaycastFromID_Offset_Nearest(character->GameObjectId, direction[i] * offset, { 0,-1,0 }, distance);
	}


 	return id[0] != 0 || id[1] != 0 || id[2] != 0 || id[3] != 0;

}

void CharacterSystem::OnUnlockInput(std::any data)
{
	uint32_t id = any_cast<uint32_t>(data);
	auto character = m_GameObjectManager->QueryComponent<Character>(id);

	if (character)
	{
		character->IsControllable = true;
	}
}

void CharacterSystem::SetIsSpawningFalse(std::any data)
{
	uint32_t id = any_cast<uint32_t>(data);
	auto character = m_GameObjectManager->QueryComponent<Character>(id);

	if (character)
	{
		character->IsSpawning = false;
	}
}

void CharacterSystem::OnStartReplay(std::any data)
{
	uint32_t gameobjectId = std::any_cast<uint32_t>(data);
	auto character = m_GameObjectManager->QueryComponent<Character>(gameobjectId);
	if (character)
	{
		character->DoOnceBoxSpawn = true;
	}
}

void CharacterSystem::OnUndo(std::any data)
{
	auto id = std::any_cast<uint32_t>(data);

	auto character = m_GameObjectManager->QueryComponent<Character>(id);

	if (!character)
		return;
	character->DoOnceBoxSpawn = true;
	character->IsControllable = false;
	character->IsMovable = false;
}

void CharacterSystem::OnReady(std::any data)
{
	auto id = std::any_cast<uint32_t>(data);

	auto character = m_GameObjectManager->QueryComponent<Character>(id);

	if (!character)
		return;

	// 초기 시작 지점 세팅
	if (character->OriginPosition == VSM::Vector3::Zero)
	{
		character->OriginPosition = character->Sibling<Transform>(m_GameObjectManager)->WorldPosition;
	}

	character->Sibling<Transform>(m_GameObjectManager)->LocalPosition = character->OriginPosition;


	for (LandingPoint& landing : ComponentItr<LandingPoint>())
	{
		if (!landing.IsCurrentStartPoint)
			continue;

		auto& position = character->Sibling<Transform>(m_GameObjectManager)->LocalPosition;

		position = landing.Sibling<Transform>(m_GameObjectManager)->WorldPosition;
		position.y += 3.f;
	}
}
