#pragma once
#include "Component.h"

class UIController : public Component
{
	GENERATE_BODY(UIController)
		GENERATE_PROPERTY(UIController,
			bIsActive, bIsReset,
			bUseUVAnimation, UVSpeed,
			bUseSpriteAnimation, FrameInterval, SpriteSize, ClipSize, SpriteAnimInterval,
			bUseAlphaAnimation, InitAlpha, AlphaSpeed)

public:
	// 에디터 데이터
	bool bIsActive{ false };
	bool bIsReset{ false };

	bool bUseUVAnimation{ false };
	VSM::Vector2 UVSpeed{ 0.1f, 0.1f };

	bool bUseSpriteAnimation{ false };
	float FrameInterval{ 0.1f };
	VSM::Vector2 SpriteSize{ 1000, 100 };
	VSM::Vector2 ClipSize{ 100, 100 };
	VSM::Vector2 SpriteAnimInterval{ 100, 0 };

	bool bUseAlphaAnimation{ false };
	float InitAlpha{ 1.f };
	float AlphaSpeed{ -0.1f };

	// todo
	bool bUseTransformAnimation;

	// 로직 데이터
	float TimePos{ 0.f };
};

class SpriteController : public Component
{
	GENERATE_BODY(SpriteController)
		GENERATE_PROPERTY(SpriteController,
			bIsActive, bIsReset, bIsRecursive,
			SpriteCount, FrameInterval, SpriteSize, ClipSize, ClipInterval)
public:
	bool bIsActive{ false };
	bool bIsReset{ false };
	bool bIsRecursive{ false };

	float FrameInterval{ 0.1f };
	unsigned int SpriteCount{ 10 };
	VSM::Vector2 SpriteSize{ 1000, 100 };
	VSM::Vector2 ClipSize{ 100, 100 };
	VSM::Vector2 ClipInterval{ 100, 0 };

	// 로직데이터
	float TimePos;
	bool bIsFinishTraversal{ false };
};

class Timer : public Component
{
	GENERATE_BODY(Timer)
		GENERATE_PROPERTY(Timer,
			bIsActive, bIsReset, bIsReverse,
			InitTimePos, TimePos, FrameInterval,
			SpriteCount, SpriteSize, ClipSize, ClipInterval)
public:
	bool bIsActive{ false };
	bool bIsReset{ false };
	bool bIsReverse{ false };
	float InitTimePos{ 10.f };
	float TimePos{ 0.f };

	float FrameInterval{ 1.f };

	unsigned int SpriteCount{ 10 };
	VSM::Vector2 SpriteSize{ 1000, 100 };
	VSM::Vector2 ClipSize{ 100, 100 };
	VSM::Vector2 ClipInterval{ 100, 0 };
};

class UVAnimController : public Component
{
	GENERATE_BODY(UVAnimController)
		GENERATE_PROPERTY(UVAnimController,
			bIsActive, bIsReset, bIsRecursive,
			UVSpeed)

public:
	// 에디터 데이터
	bool bIsActive{ false };
	bool bIsReset{ false };
	bool bIsRecursive{ false };

	VSM::Vector2 UVSpeed{ 0.1f, 0.1f };

	// 로직 데이터
	float TimePos{ 0.f };
};

class AlphaController : public Component
{
	GENERATE_BODY(AlphaController)
		GENERATE_PROPERTY(AlphaController,
			bIsActive, bIsReset, bIsRecursive,
			InitAlpha, AlphaSpeed);

public:
	// 에디터 데이터
	bool bIsActive{ false };
	bool bIsReset{ false };
	bool bIsRecursive{ false };

	float InitAlpha{ 1.f };
	float AlphaSpeed{ -0.1f };

	// 로직 데이터
	float TimePos{ 0.f };
};
