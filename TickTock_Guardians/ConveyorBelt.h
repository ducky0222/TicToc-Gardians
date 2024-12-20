#pragma once

#include "Component.h"
#include "PhysicsEnums.h"

class ConveyorBelt : public Component
{
	GENERATE_BODY(ConveyorBelt)
	GENERATE_PROPERTY(ConveyorBelt, chainedButtonID, beltForce, beltDirection)

	uint32_t chainedButtonID;
	float beltForce = 0.0f;
	eBeltDir beltDirection;
	VSM::Vector3 dir;
	bool isWorking = false;
};

