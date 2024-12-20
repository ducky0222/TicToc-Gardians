#pragma once
#include "Component.h"

struct Button : public Component
{
	GENERATE_BODY(Button)
	GENERATE_PROPERTY(Button, IsActivated)

	bool IsActivated = false;
	uint32_t CollisionCounter = 0;
};