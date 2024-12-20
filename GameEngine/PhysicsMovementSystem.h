#pragma once

#include "System.h"

class PhysicsMovementSystem : public System, public IUpdatable
{
public:
	using System::System;

	// IUpdatable을(를) 통해 상속됨
	void Update(float deltaTime) override;
};

