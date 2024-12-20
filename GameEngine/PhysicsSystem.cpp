#include "pch.h"
#include "PhysicsSystem.h"
#include "RigidBody.h"
#include "Transform.h"
#include "../PhysicEngine/HysxEngine.h"
#include "Input.h"
#include "../TickTock_Guardians/Character.h"
#include "OnOff.h"
#include "../ABGraphics/DebugRenderManager.h"
#include "../TickTock_Guardians/Box.h"
#include "../TickTock_Guardians/Button.h"


PhysicsSystem::PhysicsSystem(GameObjectManager* gameObjectManager)
	: System(gameObjectManager)
{
	EventManager::Subscribe("ChangeStaticToDynamic", CreateListenerInfo(&PhysicsSystem::ChangeStaticToDynamic));
	EventManager::Subscribe("ChangeDynamicToStatic", CreateListenerInfo(&PhysicsSystem::ChangeDynamicToStatic));

	EventManager::Subscribe("OnSceneTransitionExit", CreateListenerInfo(&PhysicsSystem::OnSceneTransitionExit));
	EventManager::Subscribe("OnSceneTransitionEnter", CreateListenerInfo(&PhysicsSystem::OnSceneTransitionEnter));
}

void PhysicsSystem::FixedUpdate(float deltaTime)
{
	/// hysxengine 업데이트를 하면 queue를 초기화해서 그전에 집어 넣음
	auto& onCollisionStayMap = Physics::GetInstance()->GetCollisionStayLists();

	auto& onCollisionEnterDeque = Physics::GetInstance()->GetCollisionEnterDeque();
	for (auto& actorPair : onCollisionEnterDeque)
	{
		EventManager::PublishImmediate("OnCollisionEnter", actorPair);
		EventManager::PublishImmediate("OnCollisionEnter", std::make_pair(actorPair.second, actorPair.first));
	}

	auto& onCollisionExitDeque = Physics::GetInstance()->GetCollisionExitDeque();
	for (auto& actorPair : onCollisionExitDeque)
	{
		EventManager::PublishImmediate("OnCollisionExit", actorPair);
		EventManager::PublishImmediate("OnCollisionExit", std::make_pair(actorPair.second, actorPair.first));
	}


	for (RigidBody& rigidBody : ComponentItr<RigidBody>())
	{
		if (rigidBody.Sibling<OnOff>(m_GameObjectManager)->IsOn == false)
			continue;

		Transform* transform = rigidBody.Sibling<Transform>(m_GameObjectManager);

		///전체적인 rigidBody component를 돌며 가지고 있는 velocity로 setvelocity를 한다.  
// 		if (rigidBody.Velocity.Length() > 0.005f)
// 		{
// 			Physics::GetInstance()->SetVelocity(rigidBody.GameObjectId, rigidBody.Velocity);
// 		}



		///Force가 0보다 커졌을 때.
		if (rigidBody.Force.Length() > 0.005f)
		{
			Physics::GetInstance()->AddForce(rigidBody.GameObjectId, rigidBody.Force);
			rigidBody.Force = VSM::Vector3{};
		}

		///스케일이 바뀌었을 때.
		if ((transform->LocalScale - rigidBody.Scale).Length() > 0.0001f)
		{
			Physics::GetInstance()->ChangeScale(rigidBody.GameObjectId, rigidBody.Weight, rigidBody.Size, rigidBody.Scale, transform->LocalScale, rigidBody.Extention);
			rigidBody.Scale = transform->LocalScale;

		}

		///다른곳에서 transform->Worldlocation = {a,b,c}; 를썻을때

		if (rigidBody.Sibling<Character>(m_GameObjectManager) != nullptr)
		{
			rigidBody.pos = transform->WorldPosition;
			rigidBody.pos.y += rigidBody.offset;
			VSM::Vector3 temp = rigidBody.pos - (VSM::Vector3)Physics::GetInstance()->GetGolbalTranslation(rigidBody.GameObjectId);

			if ((rigidBody.pos - (VSM::Vector3)Physics::GetInstance()->GetGolbalTranslation(rigidBody.GameObjectId)).Length() > 0.001f)
			{
				//Physics::GetInstance()->SetGolbalTranslation(transform->GameObjectId, rigidBody.pos);
				Physics::GetInstance()->SetGolbalTransform(transform->GameObjectId, rigidBody.pos, transform->WorldRotation);
			}
		}
		else
		{
			rigidBody.pos = transform->WorldPosition;
			if ((rigidBody.pos - (VSM::Vector3)Physics::GetInstance()->GetGolbalTranslation(rigidBody.GameObjectId)).Length() > 0.001f)
			{
				Physics::GetInstance()->SetGolbalTranslation(transform->GameObjectId, rigidBody.pos);
			}
		}
	}


	Physics::GetInstance()->Update(deltaTime);


	for (RigidBody& rigidBody : ComponentItr<RigidBody>())
	{
		if (rigidBody.Sibling<OnOff>(m_GameObjectManager)->IsOn == false)
			continue;
		//맵툴 Id 
		Transform* transform = rigidBody.Sibling<Transform>(m_GameObjectManager);
		Parent* parent = rigidBody.Sibling<Parent>(m_GameObjectManager);
		uint32_t parentID = parent ? parent->ParentID : 0;

		//if (transform->WorldPosition != (VSM::Vector3)Physics::GetInstance()->GetGolbalTranslation(rigidBody.GameObjectId))
		if (rigidBody.Sibling<Character>(m_GameObjectManager) != nullptr)
		{
			rigidBody.pos = Physics::GetInstance()->GetGolbalTranslation(transform->GameObjectId);
			rigidBody.pos.y -= rigidBody.offset;
			if (parentID != 0)
			{
				transform->LocalPosition = rigidBody.pos - m_GameObjectManager->QueryComponent<Transform>(parentID)->WorldPosition;
				transform->LocalPosition = rigidBody.pos - m_GameObjectManager->QueryComponent<Transform>(parentID)->WorldPosition;
			}
			else
				transform->LocalPosition = rigidBody.pos;
		}
		else
		{
			rigidBody.pos = Physics::GetInstance()->GetGolbalTranslation(transform->GameObjectId);
			if (parentID != 0)
				transform->LocalPosition = rigidBody.pos - m_GameObjectManager->QueryComponent<Transform>(parentID)->WorldPosition;
			else
				transform->LocalPosition = rigidBody.pos;
		}

		//transform->LocalRotation = Physics::GetInstance()->GetQuaternion(rigidBody.GameObjectId);

		rigidBody.Velocity = Physics::GetInstance()->GetCurrentVelocity(rigidBody.GameObjectId);


		/// 아마 hysx 엔진의 업데이트에서 enter에 있으면 staymap에 추가하는 로직이 제대로 걸리면 존재할거임
		if (onCollisionStayMap.size() >= 2)
		{
			if (onCollisionStayMap.find(rigidBody.GameObjectId) != onCollisionStayMap.end())
			{
				auto& curStaySet = onCollisionStayMap[rigidBody.GameObjectId];

				for (auto& actorID : curStaySet)
				{
					std::pair<uint32_t, uint32_t> pp = { rigidBody.GameObjectId, actorID };

					EventManager::PublishImmediate("OnCollisionStay", pp);
					EventManager::PublishImmediate("OnCollisionStay", std::make_pair(pp.second, pp.first));
				}
			}
		}

	}

	///박스형태의 리지드바디초록+ 트리거빨간
	for (RigidBody& rigidBody : ComponentItr<RigidBody>())
	{
		auto characeter = rigidBody.Sibling<Character>(m_GameObjectManager);
		if (characeter)
		{
			switch (rigidBody.eshapeType)
			{
				case eColliderShape::BOX_COLLIDER:
				{
					DirectX::BoundingBox tempbox = {};
					auto rigidbodytransform = rigidBody.Sibling<Transform>(m_GameObjectManager);
					tempbox.Center = rigidBody.Sibling<Transform>(m_GameObjectManager)->WorldPosition;
					tempbox.Center = tempbox.Center - VSM::Vector3{ 0,0.6f,0 };
					tempbox.Extents = { rigidBody.Size.x * rigidBody.Scale.x / 2
						, rigidBody.Size.y * rigidBody.Scale.y / 2
						, rigidBody.Size.z * rigidBody.Scale.z / 2 };

					DebugRenderManager::Get().Submit(tempbox);

				}
				break;
				case eColliderShape::CAPSULE_COLLIDER:
				{

					DirectX::BoundingSphere tempsphere1 = {};
					DirectX::BoundingSphere tempsphere2 = {};

					auto rigidbodytransform = rigidBody.Sibling<Transform>(m_GameObjectManager);
					VSM::Vector3 tempPose = rigidBody.Sibling<Transform>(m_GameObjectManager)->WorldPosition;
					tempPose -= VSM::Vector3{ 0,0.6f,0 };


					float tempradius = 0.f;
					if (rigidBody.Size.x * rigidBody.Scale.x > rigidBody.Size.z * rigidBody.Scale.z)
						tempradius = rigidBody.Size.x * rigidBody.Scale.x;
					else
						tempradius = rigidBody.Size.z * rigidBody.Scale.z;
					tempradius = tempradius / 2;
					tempsphere1.Radius = tempradius;
					tempsphere2.Radius = tempradius;
					tempPose += {0, rigidBody.Size.y* rigidBody.Scale.y / 2, 0};
					tempsphere1.Center = tempPose - VSM::Vector3{ 0, tempradius,0 };
					tempPose -= {0, rigidBody.Size.y* rigidBody.Scale.y, 0};
					tempsphere2.Center = tempPose + VSM::Vector3{ 0, tempradius,0 };

					DirectX::SimpleMath::Ray ray1 = {};
					DirectX::SimpleMath::Ray ray2 = {};
					DirectX::SimpleMath::Ray ray3 = {};
					DirectX::SimpleMath::Ray ray4 = {};
					ray1.direction = { 0,tempsphere1.Center.y - tempsphere2.Center.y,0 };
					ray2.direction = { 0,tempsphere1.Center.y - tempsphere2.Center.y,0 };
					ray3.direction = { 0,tempsphere1.Center.y - tempsphere2.Center.y,0 };
					ray4.direction = { 0,tempsphere1.Center.y - tempsphere2.Center.y,0 };
					ray1.position = tempsphere2.Center + VSM::Vector3{ tempradius,0,0 };
					ray2.position = tempsphere2.Center + VSM::Vector3{ -tempradius,0,0 };
					ray3.position = tempsphere2.Center + VSM::Vector3{ 0,0,tempradius };
					ray4.position = tempsphere2.Center + VSM::Vector3{ 0,0,-tempradius };

					DebugRenderManager::Get().Submit(tempsphere1);
					DebugRenderManager::Get().Submit(tempsphere2);
					DebugRenderManager::Get().Submit(ray1);
					DebugRenderManager::Get().Submit(ray2);
					DebugRenderManager::Get().Submit(ray3);
					DebugRenderManager::Get().Submit(ray4);
				}
				break;

				default:
					break;
			}



		}
		else
		{
			if (rigidBody.eshapeType == BOX_COLLIDER)
			{
				//DebugRenderManager::DirectX::BoundingBox
				DirectX::BoundingBox tempbox = {};
				auto rigidbodytransform = rigidBody.Sibling<Transform>(m_GameObjectManager);
				tempbox.Center = rigidBody.Sibling<Transform>(m_GameObjectManager)->WorldPosition;
				tempbox.Extents = { rigidBody.Size.x * rigidBody.Scale.x / 2
					, rigidBody.Size.y * rigidBody.Scale.y / 2
					, rigidBody.Size.z * rigidBody.Scale.z / 2 };

				DebugRenderManager::Get().Submit(tempbox);


				if (rigidBody.ecollisionType == BOX_COLLISION)
				{
					DirectX::BoundingBox tempbox = {};
					tempbox.Center = rigidBody.Sibling<Transform>(m_GameObjectManager)->WorldPosition;
					tempbox.Extents = { rigidBody.Size.x * rigidBody.Scale.x / 2 + rigidBody.Extention.x
					, rigidBody.Size.y * rigidBody.Scale.y / 2 + rigidBody.Extention.y
					, rigidBody.Size.z * rigidBody.Scale.z / 2 + rigidBody.Extention.z };

					DebugRenderManager::Get().Submit(tempbox, { 1,0,0,1 });
				}
				else if (rigidBody.ecollisionType == GIMMICK_BOX_COLLISION)
				{
					DirectX::BoundingBox tempbox1 = {};
					DirectX::BoundingBox tempbox2 = {};

					tempbox1.Center = rigidBody.Sibling<Transform>(m_GameObjectManager)->WorldPosition;
					tempbox1.Extents = { rigidBody.Size.x * rigidBody.Scale.x / 2 + rigidBody.Extention.x
					, rigidBody.Size.y * rigidBody.Scale.y / 2 + rigidBody.Extention.y
					, rigidBody.Size.z * rigidBody.Scale.z / 2 - rigidBody.Extention.z };
					tempbox2.Center = rigidBody.Sibling<Transform>(m_GameObjectManager)->WorldPosition;
					tempbox2.Extents = { rigidBody.Size.x * rigidBody.Scale.x / 2 - rigidBody.Extention.x
					, rigidBody.Size.y * rigidBody.Scale.y / 2 + rigidBody.Extention.y
					, rigidBody.Size.z * rigidBody.Scale.z / 2 + rigidBody.Extention.z };
					DebugRenderManager::Get().Submit(tempbox1, { 1,0,0,1 });
					DebugRenderManager::Get().Submit(tempbox2, { 1,0,0,1 });

				}
			}
		}
	}
}

void PhysicsSystem::Start(uint32_t gameObjectId)
{
	///해당 GameObject가 RigidBody Comp를 가지고 있지만, Actor가 생성 되어있는지 없는지에 따라 생성하는 코드 짜기
	/// 그리고 콜라이더가 있을경우 AttachCollision이 일어나도록 설정
	auto temp = m_GameObjectManager->QueryComponent<RigidBody>(gameObjectId);
	if (!Physics::GetInstance()->HaveRigidActor(gameObjectId) && temp != nullptr)
	{
		RigidBody* rigidBody = m_GameObjectManager->QueryComponent<RigidBody>(gameObjectId);
		Transform* transform = rigidBody->Sibling<Transform>(m_GameObjectManager);
		rigidBody->Scale = transform->WorldScale;
		auto character = rigidBody->Sibling<Character>(m_GameObjectManager);

		// 현재 태크로 안해뒀는데 나중에 바꾸려면 바꿔야지
		if (character)
		{
			Physics::GetInstance()->AddPxRigidActor(
				rigidBody->GameObjectId,
				rigidBody->eMaterial,
				rigidBody->ePhysicLayer,
				rigidBody->eactorType,
				rigidBody->LOCK_LINEAR.x,
				rigidBody->LOCK_LINEAR.y,
				rigidBody->LOCK_LINEAR.z,
				rigidBody->LOCK_ANGULAR.x,
				rigidBody->LOCK_ANGULAR.y,
				rigidBody->LOCK_ANGULAR.z,
				rigidBody->eshapeType,
				rigidBody->Size,
				rigidBody->Weight,
				rigidBody->Scale,
				transform->WorldMatrix,
				true,
				rigidBody->NoGravity);

		}

		else
		{
			Physics::GetInstance()->AddPxRigidActor(
				rigidBody->GameObjectId,
				rigidBody->eMaterial,
				rigidBody->ePhysicLayer,
				rigidBody->eactorType,
				rigidBody->LOCK_LINEAR.x,
				rigidBody->LOCK_LINEAR.y,
				rigidBody->LOCK_LINEAR.z,
				rigidBody->LOCK_ANGULAR.x,
				rigidBody->LOCK_ANGULAR.y,
				rigidBody->LOCK_ANGULAR.z,
				rigidBody->eshapeType,
				rigidBody->Size,
				rigidBody->Weight,
				rigidBody->Scale,
				transform->WorldMatrix,
				false,
				rigidBody->NoGravity);
		}

		if (rigidBody->MakeCollision)
		{
			// for(makecollsionnum)
			Physics::GetInstance()->AttachCollision(
				rigidBody->GameObjectId,
				rigidBody->ecollisionType,
				rigidBody->eMaterial,
				rigidBody->Size,
				rigidBody->Scale,
				rigidBody->Extention
			);
		}
		else
		{
			Physics::GetInstance()->DetachCollision(rigidBody->GameObjectId);
		}
	}


}

void PhysicsSystem::Initialize()
{
	for (RigidBody& rigidBody : ComponentItr<RigidBody>())
	{
		Start(rigidBody.GameObjectId);
	}
}

void PhysicsSystem::Finish(uint32_t gameObjectId)
{
	auto actors = Physics::GetInstance()->GetPhysicsActors();
	for (auto& actor : actors)
	{
		auto actorState = static_cast<ActorState*>(actor.second->userData);
		if (actorState->collisionState == eCollisionState::ON_COLLISION_STAY)
		{
			auto box = m_GameObjectManager->QueryComponent<Box>(actor.first);
			if (box)
			{
				auto button = m_GameObjectManager->QueryComponent<Button>(actorState->otherCompID);
				if (button)
				{
					pair<uint32_t, uint32_t> tempPair = { actor.first, actorState->otherCompID };
					EventManager::PublishImmediate("OnCollisionExit", tempPair);
					EventManager::PublishImmediate("OnCollisionExit", std::make_pair(tempPair.second, tempPair.first));
				}
			}
		}
	}

	auto rigidbody = m_GameObjectManager->QueryComponent<RigidBody>(gameObjectId);
	if (rigidbody)
	{
		Physics::GetInstance()->ReleaseActor(rigidbody->GameObjectId);

	}

	// 나중에 테스트
	//Physics::GetInstance()->ReleaseActor(gameObjectId);
	//Physics::GetInstance()->DeletePxScene();
}

void PhysicsSystem::Finalize()
{
	for (RigidBody& rigidBody : ComponentItr<RigidBody>())
	{
		Finish(rigidBody.GameObjectId);
	}
}

void PhysicsSystem::ChangeStaticToDynamic(std::any data)
{
	uint32_t Boxid = std::any_cast<uint32_t>(data);

	auto rigidBody = m_GameObjectManager->QueryComponent<RigidBody>(Boxid);
	Physics::GetInstance()->ReleaseActor(Boxid);

	if (!Physics::GetInstance()->HaveRigidActor(Boxid))
	{
		Transform* transform = rigidBody->Sibling<Transform>(m_GameObjectManager);
		rigidBody->Scale = transform->WorldScale;

		// 현재 태크로 안해뒀는데 나중에 바꾸려면 바꿔야지
		if (rigidBody->Sibling<Character>(m_GameObjectManager) != nullptr)
		{
			Physics::GetInstance()->AddPxRigidActor(
				rigidBody->GameObjectId,
				rigidBody->eMaterial,
				rigidBody->ePhysicLayer,
				1,
				rigidBody->LOCK_LINEAR.x,
				rigidBody->LOCK_LINEAR.y,
				rigidBody->LOCK_LINEAR.z,
				rigidBody->LOCK_ANGULAR.x,
				rigidBody->LOCK_ANGULAR.y,
				rigidBody->LOCK_ANGULAR.z,
				rigidBody->eshapeType,
				rigidBody->Size,
				rigidBody->Weight,
				rigidBody->Scale,
				transform->WorldMatrix,
				true,
				rigidBody->NoGravity);
		}

		else
		{
			Physics::GetInstance()->AddPxRigidActor(
				rigidBody->GameObjectId,
				rigidBody->eMaterial,
				rigidBody->ePhysicLayer,
				1,
				rigidBody->LOCK_LINEAR.x,
				rigidBody->LOCK_LINEAR.y,
				rigidBody->LOCK_LINEAR.z,
				rigidBody->LOCK_ANGULAR.x,
				rigidBody->LOCK_ANGULAR.y,
				rigidBody->LOCK_ANGULAR.z,
				rigidBody->eshapeType,
				rigidBody->Size,
				rigidBody->Weight,
				rigidBody->Scale,
				transform->WorldMatrix,
				false,
				rigidBody->NoGravity);
		}

		if (rigidBody->MakeCollision)
		{
			// for(makecollsionnum)
			Physics::GetInstance()->AttachCollision(
				rigidBody->GameObjectId,
				rigidBody->ecollisionType,
				rigidBody->eMaterial,
				rigidBody->Size,
				rigidBody->Scale,
				rigidBody->Extention
			);
		}
		else
		{
			Physics::GetInstance()->DetachCollision(rigidBody->GameObjectId);
		}
	}


}

void PhysicsSystem::ChangeDynamicToStatic(std::any data)
{
	uint32_t Boxid = std::any_cast<uint32_t>(data);

	auto rigidBody = m_GameObjectManager->QueryComponent<RigidBody>(Boxid);
	Physics::GetInstance()->ReleaseActor(Boxid);

	if (!Physics::GetInstance()->HaveRigidActor(Boxid))
	{
		Transform* transform = rigidBody->Sibling<Transform>(m_GameObjectManager);
		rigidBody->Scale = transform->WorldScale;

		// 현재 태크로 안해뒀는데 나중에 바꾸려면 바꿔야지
		if (rigidBody->Sibling<Character>(m_GameObjectManager) != nullptr)
		{
			Physics::GetInstance()->AddPxRigidActor(
				rigidBody->GameObjectId,
				rigidBody->eMaterial,
				rigidBody->ePhysicLayer,
				0,
				rigidBody->LOCK_LINEAR.x,
				rigidBody->LOCK_LINEAR.y,
				rigidBody->LOCK_LINEAR.z,
				rigidBody->LOCK_ANGULAR.x,
				rigidBody->LOCK_ANGULAR.y,
				rigidBody->LOCK_ANGULAR.z,
				rigidBody->eshapeType,
				rigidBody->Size,
				rigidBody->Weight,
				rigidBody->Scale,
				transform->WorldMatrix,
				true,
				rigidBody->NoGravity);
		}

		else
		{
			Physics::GetInstance()->AddPxRigidActor(
				rigidBody->GameObjectId,
				rigidBody->eMaterial,
				rigidBody->ePhysicLayer,
				0,
				rigidBody->LOCK_LINEAR.x,
				rigidBody->LOCK_LINEAR.y,
				rigidBody->LOCK_LINEAR.z,
				rigidBody->LOCK_ANGULAR.x,
				rigidBody->LOCK_ANGULAR.y,
				rigidBody->LOCK_ANGULAR.z,
				rigidBody->eshapeType,
				rigidBody->Size,
				rigidBody->Weight,
				rigidBody->Scale,
				transform->WorldMatrix,
				false,
				rigidBody->NoGravity);
		}

		if (rigidBody->MakeCollision)
		{
			// for(makecollsionnum)
			Physics::GetInstance()->AttachCollision(
				rigidBody->GameObjectId,
				rigidBody->ecollisionType,
				rigidBody->eMaterial,
				rigidBody->Size,
				rigidBody->Scale,
				rigidBody->Extention
			);
		}
		else
		{
			Physics::GetInstance()->DetachCollision(rigidBody->GameObjectId);
		}
	}
}

void PhysicsSystem::OnSceneTransitionEnter(std::any data)
{
	Initialize();
}
void PhysicsSystem::OnSceneTransitionExit(std::any data)
{
	Finalize();
}