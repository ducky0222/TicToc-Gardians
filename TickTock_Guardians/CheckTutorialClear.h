#pragma once
#include "TickTockScene.h"

class CheckTutorialClear : public TickTockScene
{
public:
	CheckTutorialClear(GameObjectManager* goManager);

	void Enter() override;
	eSceneState HandleTransition() override;
	void Update(float deltaTime) override;
	void Exit() override;

private:
	void OnFinishReplay(std::any data);
	void OnClearAllMission(std::any data);
	void OnFailMission(std::any data);

	void OnReplayBtnClick(std::any data);
	void OnNextBtnClick(std::any data);
	void OnLobbyBtnClick(std::any data);

	void OnResetStage(std::any data);
	void OnTransitionStageOver(std::any data);
private:
	uint32_t mCharacterId = 0;
	bool mbIsActivated = false;
	bool mbIsChecked = false;
	bool mbIsFinished = false;
	bool mbIsRetryBtnClicked = false;
	bool mbIsStageOver = false;
	bool mbIsResetStage = false;

	uint32_t mDigitCounterColonId = 0;
	uint32_t mDigitCounterId0001 = 0;
	uint32_t mDigitCounterId001 = 0;
	uint32_t mDigitCounterId01 = 0;
	uint32_t mDigitCounterId10 = 0;
};
