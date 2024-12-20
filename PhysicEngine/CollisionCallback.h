#pragma once

//#include "PxPhysicsAPI.h"
//#include "PxSimulationEventCallback.h"
#include <physx/PxPhysicsAPI.h>
#include <physx/PxSimulationEventCallback.h>

class CollisionCallback : public physx::PxSimulationEventCallback
{
#pragma region Using
	// 충돌 이벤트 처리 로직
	virtual void onContact(const physx::PxContactPairHeader& pairHeader, const physx::PxContactPair* pairs, physx::PxU32 nbPairs) PX_OVERRIDE;
#pragma endregion

#pragma region not Using
	// 트리거 이벤트 처리 로직
	virtual void onTrigger(physx::PxTriggerPair* pairs, physx::PxU32 count) PX_OVERRIDE;
	virtual void onConstraintBreak(physx::PxConstraintInfo* constraints, physx::PxU32 count) PX_OVERRIDE;
	virtual void onWake(physx::PxActor** actors, physx::PxU32 count) PX_OVERRIDE;
	virtual void onSleep(physx::PxActor** actors, physx::PxU32 count) PX_OVERRIDE;
	virtual void onAdvance(const physx::PxRigidBody* const*, const physx::PxTransform*, const physx::PxU32) PX_OVERRIDE;
#pragma endregion
};

