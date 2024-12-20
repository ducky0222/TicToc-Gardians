#pragma once

#include "Component.h"

struct PhysicsMovement : public Component
{
	GENERATE_BODY(PhysicsMovement)
	GENERATE_PROPERTY(PhysicsMovement, velocityDir, moveforce, jumpForce)

	VSM::Vector3 velocityDir;
	float moveforce;
	float jumpForce;
	float maxVelocity;
};

