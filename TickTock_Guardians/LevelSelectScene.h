#pragma once
#include "TickTockScene.h"

class LevelSelectScene : public TickTockScene
{
public:
	LevelSelectScene(GameObjectManager* gameObjectManager);

	virtual void Enter();
	virtual eSceneState HandleTransition();
	virtual void Update(float deltaTime);
	virtual void Exit();

	void OnFirstSceneIn(std::any data);
	void OnFirstEnterTabletIn(std::any data);
	void OnFirstEnterNextBtn(std::any data);
	void OnFirstEnterBeforeBtn(std::any data);
	void OnFirstEnterTabletOut(std::any data);
	void OnFirstSceneOut(std::any data);

	void OnNormalSelectIn(std::any data);
	void OnNormalTableUIIn(std::any data);
	void OnNormalTableUIOut(std::any data);
	void OnNormalSelectOut(std::any data);

	void OnHardSelectIn(std::any data);
	void OnHardTableUIIn(std::any data);
	void OnHardTableUIOut(std::any data);
	void OnHardSelectOut(std::any data);

	void OnLevelCursorIn(std::any data);
	void OnLevelCursorOut(std::any data);

	void OnStageBtnClicked(std::any data);

	void OnPause(std::any data);
	void OnResume(std::any data);
	void OnCreditButtonClick(std::any data);

private:
	void handleTVUIDisplayScreen(const std::string& TVUIDisplayName);
	void handleTransformAnimActive(const std::string& entityIdentity, bool bIsActive);
	void handleChildEntityOnOff(const std::string& entityIdentity, bool bIsOn);
	void handleOnOffChildrenRecursive(uint32_t entityId, bool bIsOn);

private:
	bool bIsActiveEvent{ false };
	bool bIsFirstEnter{ true };
	bool bIsStageSelected{ false };

	const std::string LOBBY_CHARACTER_NAME = "LobbyCharacter";

	const std::string FIRST_ENTER_TABLET_NAME = "FirstEnterTablet";

	const std::string TV_UI_NAME = "TVUI";

	const std::string NORMAL_CAM_NAME = "NormalCamera";
	const std::string HARD_CAM_NAME = "HardCamera";

	const std::string NORMAL_TABLET_NAME = "NormalTablet";
	const std::string HARD_TABLET_NAME = "HardTablet";

	const std::string ENTER_TABLET_MINI_INDEX_NAME = "miniIndex";

	bool mbIsConfig{ false };
	uint32_t mConfigID{ 0 };
};

