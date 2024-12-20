#pragma once
#include "Component.h"

class SceneComponent : public Component
{
	GENERATE_BODY(SceneComponent)
	GENERATE_PROPERTY(SceneComponent, SceneName)

public:
	std::string SceneName;
};