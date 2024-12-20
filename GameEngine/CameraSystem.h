#pragma once
#include "System.h"

class CameraSystem : public System, public IUpdatable
{

public:
	using System::System;

	void Update(float deltaTime) override;
};

