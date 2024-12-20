#pragma once
#include <bitset>

#include "Component.h"

struct Character : public Component
{
	GENERATE_BODY(Character)
	GENERATE_PROPERTY(Character, Speed, JumpForce)

	float Speed;
	float JumpForce;

	std::bitset<4> Bitsets;
	Vector3 Direction = Vector3::Zero;
	// 안녕칭구들
	int i = 0;
	int replayi = 0;


	/// 분산의 여지가 보이는 컴포넌트
	Vector3 InitialPosition = Vector3::Zero;
	bool IsReplayable = false;
	std::array<bitset<4>, 600> Tracer;
};