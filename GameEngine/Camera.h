#pragma once
#include "Component.h"

struct Camera : public Component
{
	GENERATE_BODY(Camera)
	GENERATE_PROPERTY(Camera, NearClipPlane, FarClipPlane, Aspect, FieldOfView, Orthographic)

	float NearClipPlane;
	float FarClipPlane;
	float Aspect;
	float FieldOfView;

	bool Orthographic;
};
