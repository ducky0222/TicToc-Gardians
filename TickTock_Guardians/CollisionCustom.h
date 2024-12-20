#pragma once
#include "Component.h"

struct CollisionCustom : public Component
{
	GENERATE_BODY(CollisionCustom)
	GENERATE_PROPERTY(CollisionCustom, OnCollisionEnter, OnCollisionExit)

public:
	std::string OnCollisionEnter;
	std::string OnCollisionExit;
};
