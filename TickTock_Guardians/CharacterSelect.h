#pragma once
#include "TickTockScene.h"

class CharacterSelect : public TickTockScene
{
public:
	CharacterSelect(GameObjectManager* gameObjectManager);

	virtual void Enter();
	virtual eSceneState HandleTransition();
	virtual void Update(float deltaTime);
	virtual void Exit();

private:
	void onFinishSelect(std::any data);
	void OnNoBtnClick(std::any data);
	void OnCharacterClick(std::any data);

	void OnBindSceneName(std::any data);

private:
	bool mbIsTransition{ false };
	bool mbIsActiveEvent{ false };
	std::deque<uint32_t> selectedCharacters;

	std::string mSceneName;
};