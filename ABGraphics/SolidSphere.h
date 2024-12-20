#pragma once

#include "IDrawable.h"

class SolidSphere : public IDrawable
{
public:
	SolidSphere(Graphics& graphics, float radius);
	virtual ~SolidSphere() = default;
};