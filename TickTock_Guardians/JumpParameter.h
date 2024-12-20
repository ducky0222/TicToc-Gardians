#pragma once
#include <Component.h>

struct JumpParameter :
	public Component
{
	GENERATE_BODY(JumpParameter)
	GENERATE_PROPERTY(JumpParameter, JumpHeight, JumpTimeUp, GravityMultiplier)

	float JumpHeight;		// 점프력 (칸수)

	float JumpTimeUp;		// 점프시간
	float GravityMultiplier;		// 점프시간

	// 점프 횟수도 늘어날수도 있다.

};

