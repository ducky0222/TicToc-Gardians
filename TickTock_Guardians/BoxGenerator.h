#pragma once
#include <Component.h>

class BoxGenerator :public Component
{
	GENERATE_BODY(BoxGenerator)
	GENERATE_PROPERTY(BoxGenerator, chainedButtonID)

	uint32_t chainedButtonID;
	bool DoOnce = false;
	VSM::Vector3 GeneratePose;
};

