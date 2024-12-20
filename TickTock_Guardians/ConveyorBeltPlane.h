#pragma once

#include "Component.h"

class ConveyorBeltPlane : public Component
{
	GENERATE_BODY(ConveyorBeltPlane)
	GENERATE_PROPERTY(ConveyorBeltPlane, chainedBeltID)

	uint32_t chainedBeltID;
};

