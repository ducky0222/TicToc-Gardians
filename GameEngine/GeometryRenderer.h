#pragma once
#include "Component.h"

struct GeometryRenderer : public Component
{
	GENERATE_BODY(GeometryRenderer)
	GENERATE_PROPERTY(GeometryRenderer, KeyName, Alpha, bUseAlpha)

public:
	// render common
	std::string KeyName;
	float Alpha;
	bool bUseAlpha;
};

