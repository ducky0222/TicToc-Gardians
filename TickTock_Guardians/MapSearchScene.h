#pragma once
#include "TickTockScene.h"

class MapSearchScene : public TickTockScene
{
public:
	MapSearchScene(GameObjectManager* gameObjectManager);

	virtual void Enter();
	virtual eSceneState HandleTransition();
	virtual void Update(float deltaTime);
	virtual void Exit();

	void OnCutSceneOut(std::any data);
	void OnEnterUIAppear(std::any data);
	void OnSceneTransition(std::any data);
	void OnEnterLevelSelect(std::any data);


	void OnDelayedFinalize(std::any data);
private:
	uint32_t mCutSceneCharacterId;
	uint32_t mCutSceneBackgroundId;
	float mCutSceneDuration = 5.f;
	bool mbEyeCatchUsed = false;

	bool mbIsActiveEvent = false;
};