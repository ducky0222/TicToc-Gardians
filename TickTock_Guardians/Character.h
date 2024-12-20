#pragma once
#include <bitset>

#include "Component.h"

enum ACTION : uint32_t
{
	LEFT,
	RIGHT,
	UP,
	DOWN,
	JUMP,
	BOX,
	EMPTY,
	SKIP,
};

/// <summary>
/// 여기에 transition
/// </summary>

struct Character : public Component
{
	GENERATE_BODY(Character)
	GENERATE_PROPERTY(Character, Speed, MaxSpeed, JumpForce, Friction, IsControllable, IsMovable)

	float Speed = 10.f;
	float MaxSpeed = 10.f;
	float JumpForce = 10.f;
	float Friction = 0.5f;

	float FootprintTime = 0.f;
	float FootprintCoolTime = 0.6f;

	bool IsTracing = false;
	bool IsControllable = false;
	bool IsMovable = false;
	bool IsJumping = false;
	bool IsArrowDown = false;

	bool IsJumpAble = false;
	int JumpCoolDownFrame = 0;
	static constexpr int JumpCoolDown = 15;

	uint32_t JumpAdjustDelay = 0;
	

	bool IsPushing = false;
	bool IsPushed = false;
	bool IsFixable = false;
	bool IsSkipping = false;
	bool IsSpawning = false;
	bool DoOnceBoxSpawn = true;
	bool IsConveying = false;

	VSM::Vector3 Direction;
	std::bitset<8> DirectionBitset;
	VSM::Vector3 OriginPosition = VSM::Vector3::Zero;
};
