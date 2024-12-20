#pragma once
#include "IWindow.h"

class GameObjectManager;

class MenuBar : public IWindow
{
public:
	MenuBar(GameObjectManager* goManager);
	~MenuBar() override = default;

	void OnImGuiRender() override;


private:
	void AlertSaveScene();
	void RenderMenuFilie();
	void SaveCurrentScene();
	void AboutMenu();
	void RenderAboutPopup();

	void RenderTextCenter(const char* text);
	
public:
	static inline bool IsPlaying = false;
	static inline bool IsPause = false;

private:
	GameObjectManager* m_GameObjectManager;

	Matrix PrevCameraMatrix = Matrix::Identity;

	std::string CurrentSceneName = "";
	std::string CurrentScenePath = "";

	bool NewSceneFlag = false;

	std::list<std::string> m_RecentQueue;


	static inline bool DirtyFlag = true;

	friend class Asset;

};
