#pragma once
#include "TickTockScene.h"

class TitleScene : public TickTockScene
{
public:
	TitleScene(GameObjectManager* gameObjectManager);

	virtual void Enter();
	virtual eSceneState HandleTransition();
	virtual void Update(float deltaTime);
	virtual void Exit();

private:
	const float INIT_INPUT_LIMIT_DURATION = 1.f;

	float minputlimitDuration = INIT_INPUT_LIMIT_DURATION;
};

