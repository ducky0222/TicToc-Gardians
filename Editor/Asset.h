#pragma once
#include "IWindow.h"

class MenuBar;
class GameObjectManager;

class Asset : public IWindow
{
public:
	Asset(GameObjectManager* goManager, MenuBar* menuBar);
	~Asset() override = default;


	void OnImGuiRender() override;

private:
	void RenderAssetDirectory(const std::string& path);
	void RenderSelectedPathFiles();

private:
	GameObjectManager* m_GameObjectManager;
	MenuBar* m_MenuBar;

	std::string AssetPath;
	std::string SelectedPath;

	std::string input;

};
