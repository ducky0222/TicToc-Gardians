#pragma once

#include "Component.h"

class AnimationController : public Component
{
	GENERATE_BODY(AnimationController)
	GENERATE_PROPERTY(AnimationController, EntryFileName, TransitionTime, AnimSpeedWeight, bUseTransition, bIsActive)

	// 에디터에 표현할 데이터
	std::string EntryFileName{ ".fbx" };
	float TransitionTime{ 0.2f };
	float AnimSpeedWeight{ 1.f };
	bool bUseTransition{ true };
	bool bIsActive{ true };

	// 로직 처리용 데이터
	std::string CurrentFileName;
	std::string BeforeFineName;
	bool bIsInit{ false };
	bool bIsTransitionState{ false };
	float TimePos{ 0.f };
	float TransitionTimePos{ 0.f };
	size_t CurrentFrameIndex{ 0u };
	bool ThrowFrameEvent{ false };
};

