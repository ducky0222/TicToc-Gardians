#pragma once
#include "AnimationSystem.h"
#include "Character.h"

class TickTockAnimationSystem : public AnimationSystem
{
public:
	struct CommonCharacterData
	{
		float speed;
		bool isMoving;
		bool isJumping;
		bool isFixable;
		bool isWaiting;
	};

	struct BeaverData : public CommonCharacterData
	{
		bool isPushing;
		bool isPushed;
		bool isSpawning;
	};

	struct SelectCharacterData
	{
		bool bIsCheckSelect;
	};

	struct ButtonData
	{
		bool IsActivating;
	};


public:
	TickTockAnimationSystem(GameObjectManager* gameObjectManager);

	virtual bool handleTransition(AnimationController& controller) override;
	
};

