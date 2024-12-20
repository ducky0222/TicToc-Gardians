#pragma once
#include "TickTockScene.h"
class StageOver :
    public TickTockScene
{
public:
	StageOver(GameObjectManager* gameObjectManager);

	virtual void Enter();
	virtual eSceneState HandleTransition();
	virtual void Update(float deltaTime);
	virtual void Exit();


private:
	bool mbIsActivated = false;
	bool mbIsCleared = false;
	std::string CurrentSceneName;
};

