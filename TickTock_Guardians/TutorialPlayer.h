#pragma once
#include "TickTockScene.h"

class TutorialPlayer : public TickTockScene
{
public:
	TutorialPlayer(GameObjectManager* goManager);

	void Enter() override;
	eSceneState HandleTransition() override;
	void Update(float deltaTime) override;
	void Exit() override;

private:
	void OnFinishTracing(std::any data);

	void OnResetStage(std::any data);
	void OnSceneTransition(std::any data);
	void OnEnterLevelSelect(std::any data);
		
private:
	bool mbIsActivated = false;

	uint32_t mCharacterId = 0;
	bool mbIsStart = false;
	bool mbIsFinished = false;

	uint32_t mDigitCounterColonId = 0;
	uint32_t mDigitCounterId0001 = 0;
	uint32_t mDigitCounterId001 = 0;
	uint32_t mDigitCounterId01 = 0;
	uint32_t mDigitCounterId10 = 0;

	uint32_t mPortraitId = 0;

	bool mbIsResetStage = false;

	bool mbEyeCatchUsed = false;
};
