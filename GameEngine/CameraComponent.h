#pragma once
#include "Component.h"

struct CameraComponent : public Component
{
	GENERATE_BODY(CameraComponent)
	GENERATE_PROPERTY(CameraComponent, NearClipPlane, FarClipPlane, Aspect, FieldOfView, OrthoSize, Orthographic)

	float NearClipPlane = 0.1f;
	float FarClipPlane = 1000.f;
	float Aspect = 1.778f;
	float FieldOfView = 45.0f * (XM_PI / 180.0f);
	
	float OrthoSize = 10.0f;

	bool Orthographic;
};
