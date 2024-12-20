#include "pch.h"
#include "CollisionCallback.h"
#include "HysxEngine.h"
#include "../PhysicEngine/PhysicsEnums.h"

using namespace physx;

enum { ADJ_LIST_SIZE = 64 };

void CollisionCallback::onContact(const physx::PxContactPairHeader& pairHeader, const physx::PxContactPair* pairs, physx::PxU32 nbPairs)
{
	for (PxU32 i = 0; i < nbPairs; i++)
	{
		const PxContactPair& cp = pairs[i];

		// OnCollisionEnter
		if (cp.events & PxPairFlag::eNOTIFY_TOUCH_FOUND)
		{
			// component의 enum 바꾸기
			auto& actors = pairHeader.actors;

			for (auto& actor : actors)
			{
				ActorState* curActorState = static_cast<ActorState*>(actor->userData);
				uint32_t curID = curActorState->myCompID;

				//// 처음에도 추가해야 되고 나중에도 추가해야된다
				//if (Physics::GetInstance()->GetCollisionStayLists().find(curID) == Physics::GetInstance()->GetCollisionStayLists().end())
				//{
				//	Physics::GetInstance()->GetCollisionStayLists().insert({ curID, std::unordered_set<uint32_t>(ADJ_LIST_SIZE) });
				//}
			}

			static_cast<ActorState*>(actors[0]->userData)->collisionState = eCollisionState::ON_COLLISION_ENTER;
			static_cast<ActorState*>(actors[1]->userData)->collisionState = eCollisionState::ON_COLLISION_ENTER;

			uint32_t curID0 = static_cast<ActorState*>(actors[0]->userData)->myCompID;
			uint32_t curID1 = static_cast<ActorState*>(actors[1]->userData)->myCompID;
			Physics::GetInstance()->GetCollisionEnterDeque().push_back({ curID0, curID1 });

			//OutputDebugStringA("OnCollisionEnter\n");
		}

		// OnCollisionStay
		if (cp.events & PxPairFlag::eNOTIFY_TOUCH_PERSISTS)
		{
			// component의 enum 바꾸기
			auto& actors = pairHeader.actors;

			// 이전에 충돌한 상태이면
			static_cast<ActorState*>(actors[0]->userData)->collisionState = eCollisionState::ON_COLLISION_STAY;
			static_cast<ActorState*>(actors[1]->userData)->collisionState = eCollisionState::ON_COLLISION_STAY;

			uint32_t curID0 = static_cast<ActorState*>(actors[0]->userData)->myCompID;
			uint32_t curID1 = static_cast<ActorState*>(actors[1]->userData)->myCompID;

			static_cast<ActorState*>(actors[0]->userData)->otherCompID = curID1;
			static_cast<ActorState*>(actors[1]->userData)->otherCompID = curID0;


			//OutputDebugStringA("OnCollisionStay\n");
		}

		// OnCollsionEnd
		if (cp.events & PxPairFlag::eNOTIFY_TOUCH_LOST)
		{
			// component의 enum 바꾸기
			auto& actors = pairHeader.actors;

			// 여기서 날리는지 -> 최후의 보루

			static_cast<ActorState*>(actors[0]->userData)->collisionState = eCollisionState::ON_COLLISION_END;
			static_cast<ActorState*>(actors[1]->userData)->collisionState = eCollisionState::ON_COLLISION_END;


			uint32_t curID0 = static_cast<ActorState*>(actors[0]->userData)->myCompID;
			uint32_t curID1 = static_cast<ActorState*>(actors[1]->userData)->myCompID;

			static_cast<ActorState*>(actors[0]->userData)->otherCompID = curID1;
			static_cast<ActorState*>(actors[1]->userData)->otherCompID = curID0;

			auto& list = Physics::GetInstance()->GetCollisionStayLists();
			bool ID0Exist = list.find(curID0) != list.end();
			bool ID1Exist = list.find(curID1) != list.end();
			if (ID0Exist && ID1Exist)
			{
				auto& ID0Set = list.find(curID0)->second;
				auto& ID1Set = list.find(curID1)->second;

				if (ID0Set.find(curID1) != ID0Set.end() && ID1Set.find(curID0) != ID1Set.end())
				{
					Physics::GetInstance()->GetCollisionExitDeque().push_back({ curID0, curID1 });
				}
			}


			//OutputDebugStringA("OnCollsionEnd\n");
		}
	}
}

#pragma region not Using
void CollisionCallback::onTrigger(physx::PxTriggerPair* pairs, physx::PxU32 count)
{
	/*auto tempactor= pairs->triggerActor;
	auto id = static_cast<int*>(tempactor->userData);
	Physics::GetInstance()->m_CollisionEnter.insert(id,)
		id*/
}

void CollisionCallback::onConstraintBreak(physx::PxConstraintInfo* constraints, physx::PxU32 count)
{

}

void CollisionCallback::onWake(physx::PxActor** actors, physx::PxU32 count)
{

}

void CollisionCallback::onSleep(physx::PxActor** actors, physx::PxU32 count)
{

}

void CollisionCallback::onAdvance(const physx::PxRigidBody* const*, const physx::PxTransform*, const physx::PxU32)
{

}
#pragma endregion
