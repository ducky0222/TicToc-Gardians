#include "pch.h"
#include "PhysicsMovementSystem.h"
#include "Input.h"
#include "HysxEngine.h"
#include "PhysicsMovement.h"
//#include "RigidBody.h"

void PhysicsMovementSystem::Update(float deltaTime)
{
	for (PhysicsMovement& physicsMov : ComponentItr<PhysicsMovement>())
	{

		if (!Input::GetInstance().GetKey(KEY::SPACE))
		{
			if (Input::GetInstance().GetKey(KEY::UP) || Input::GetInstance().GetKey(KEY::DOWN) || Input::GetInstance().GetKey(KEY::LEFT) || Input::GetInstance().GetKey(KEY::RIGHT))
			{
				if (physicsMov.velocityDir.Length() > 0)
				{
					Physics::GetInstance()->AddVelocity(physicsMov.GameObjectId, physicsMov.velocityDir, physicsMov.moveforce * deltaTime);
				}
			}
		}
		else
		{
			if (Input::GetInstance().GetKey(KEY::UP) || Input::GetInstance().GetKey(KEY::DOWN) || Input::GetInstance().GetKey(KEY::LEFT) || Input::GetInstance().GetKey(KEY::RIGHT))
			{
				if (physicsMov.velocityDir.Length() > 0)
				{
					Physics::GetInstance()->AddVelocity(physicsMov.GameObjectId, physicsMov.velocityDir, physicsMov.moveforce * deltaTime);
				}
			}
		}

		if (Input::GetInstance().GetKeyDown(KEY::SPACE))
		{
			Physics::GetInstance()->AddVelocity(physicsMov.GameObjectId,{0,1,0}, physicsMov.jumpForce);
		}
	}
}


