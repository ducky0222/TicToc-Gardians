#pragma once
#include "Component.h"

class BackgroundRenderer : public Component
{
	GENERATE_BODY(BackgroundRenderer)
		GENERATE_PROPERTY(BackgroundRenderer, FileName)

public:
	std::string FileName;
};

