#pragma once
#include "System.h"

class PlayerSystem : public System, public IUpdatable
{
public:
	using System::System;

	void Update(float deltaTime) override;
};

