#pragma once

#include "System.h"

class PhysicsMovementSystem : public System, public IUpdatable
{
public:
	using System::System;

	// IUpdatable��(��) ���� ��ӵ�
	void Update(float deltaTime) override;
};

