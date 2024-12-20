#pragma once
#include "Component.h"

struct Movement : public Component
{
	GENERATE_BODY(Movement)
	GENERATE_PROPERTY(Movement, Velocity)

	VSM::Vector3 Velocity = VSM::Vector3::Zero;
};

