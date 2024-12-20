#pragma once
#include "System.h"

class PlayableSystem : public System, public IUpdatable, public IFixedUpdatable
{
public:
	using System::System;

	void Update(float deltaTime) override;

	void FixedUpdate(float deltaTime) override;
};

