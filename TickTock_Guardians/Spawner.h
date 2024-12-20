#pragma once
#include "Component.h"

struct Spawner : public Component
{
	GENERATE_BODY(Spawner)
		GENERATE_PROPERTY(Spawner, Position)

		VSM::Vector3 Position;
};
