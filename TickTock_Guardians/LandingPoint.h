#pragma once
#include "Component.h"

struct LandingPoint : public Component
{
	GENERATE_BODY(LandingPoint)
	GENERATE_PROPERTY(LandingPoint, IsOrigin, IsCurrentStartPoint)

	bool IsOrigin = false;
	bool IsCurrentStartPoint = false;

	uint32_t LandingOrder = 0; // 어떤 씬에서 랜딩 되었는가...
};
