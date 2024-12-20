#pragma once
#include "System.h"

class MovementSystem : public System, public IUpdatable
{
public:
	using System::System;

	void Update(float deltaTime) override;
};
