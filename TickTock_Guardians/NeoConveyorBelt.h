#pragma once

#include "Component.h"
#include "PhysicsEnums.h"

class NeoConveyorBelt : public Component
{
	GENERATE_BODY(NeoConveyorBelt)
	GENERATE_PROPERTY(NeoConveyorBelt, chainedButtonID, beltForce, beltDirection, isWorking)

	uint32_t chainedButtonID;
	float beltForce = 0.0f;
	eBeltDir beltDirection;
	VSM::Vector3 dir;
	bool isWorking = true;
	VSM::Vector3 movement;
};

