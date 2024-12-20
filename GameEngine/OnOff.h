#pragma once
#include "Component.h"

struct OnOff : public Component
{
	GENERATE_BODY(OnOff)
	GENERATE_PROPERTY(OnOff, IsOn);

	bool IsOn = true;
};
