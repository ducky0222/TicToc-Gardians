#pragma once

#include "Component.h"
#include "PhysicsEnums.h"
#include "VerySimpleMath.h"


struct RigidBody :public Component
{
	GENERATE_BODY(RigidBody)
	GENERATE_PROPERTY(RigidBody, eMaterial, ePhysicLayer, eactorType, LOCK_LINEAR, LOCK_ANGULAR, eshapeType, Size, Weight, Velocity, Force, MakeCollision, ecollisionType, Extention, NoGravity)

	ePxMaterial eMaterial;
	ePxLayer ePhysicLayer;
	eActorType eactorType;

	VSM::Boolean3 LOCK_LINEAR;
	VSM::Boolean3 LOCK_ANGULAR;

	eColliderShape eshapeType;
	VSM::Vector3 Size = VSM::Vector3{ 2.0f, 2.0f, 2.0f };
	VSM::Vector3 Scale;
	float Weight = 1.f;
	VSM::Vector3 Velocity;
	VSM::Vector3 Force;

	bool MakeCollision = false;
	eCollisionShape ecollisionType;
	VSM::Vector3 Extention = VSM::Vector3{ 0.0f, 0.0f, 0.0f };

	float offset = -0.6f;
	VSM::Vector3 pos;
	bool NoGravity = false;
};

