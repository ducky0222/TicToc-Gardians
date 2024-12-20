#pragma once
#include "TickTockScene.h"

class ThirdPlayer : public TickTockScene
{
public:
	ThirdPlayer(GameObjectManager* gameObjectManager);

	virtual void Enter();
	virtual eSceneState HandleTransition();
	virtual void Update(float deltaTime);
	virtual void Exit();

	void OnFinishTracing(std::any data);
	void OnFinishReplay(std::any data);
	void SetCharacterIndex(std::any data);
	void OnSkipTracing(std::any data);

private:
	bool mbIsActivated = false;

	uint32_t mCharacterId1 = 0;	// 첫 번째 캐릭터의 아이디
	bool mbIsFinished1 = false;	// 첫 번째 캐릭터의 움직임이 끝났는가

	uint32_t mCharacterId2 = 0;	// 두 번째 캐릭터의 아이디
	bool mbIsFinished2 = false;	// 두 번째 캐릭터의 움직임이 끝났는가

	uint32_t mCharacterId3 = 0;	// 세 번째 캐릭터의 아이디
	bool mbIsFinished3 = false;	// 세 번째 캐릭터의 움직임이 끝났는가

	uint32_t mDigitCounterColonId = 0;	// 디지털 카운터의 아이디 (0.01의 자릿수)
	uint32_t mDigitCounterId0001 = 0;	// 디지털 카운터의 아이디 (0.01의 자릿수)
	uint32_t mDigitCounterId001 = 0;	// 디지털 카운터의 아이디 (0.1의 자릿수)
	uint32_t mDigitCounterId01 = 0;	// 디지털 카운터의 아이디 (1의 자릿수)
	uint32_t mDigitCounterId10 = 0;	// 디지털 카운터의 아이디 (10의 자릿수)

	uint32_t mPortraitId01 = 0;		// 초상화의 아이디
	uint32_t mPortraitId02 = 0;		// 초상화의 아이디
	uint32_t mPortraitId03 = 0;		// 초상화의 아이디

	bool mbIsStart = false;		// 세 번째 캐릭터가 움직였는가

	bool mbIsResetStage = false;
	bool mbIsUndo = false;

	//TagType firstType;
	//TagType secondType;
	//TagType thirdType;
};
