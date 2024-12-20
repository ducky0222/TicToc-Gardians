#pragma once
#pragma once

#include "IDrawable.h"

class SolidPlane : public IDrawable
{
public:
	SolidPlane(Graphics& graphics, float size, DirectX::SimpleMath::Color color = { 1.0f,1.0f,1.0f,0.0f });
	~SolidPlane() = default;

	void SpawnControlWindow(Graphics& graphics, const std::string& name);
};