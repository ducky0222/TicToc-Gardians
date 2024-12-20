#pragma once
#include "IWindow.h"

class GameObjectManager;

class Hierarchy : public IWindow
{
public:
	Hierarchy(GameObjectManager* goManager);
	~Hierarchy() override = default;

	void OnImGuiRender() override;

public:
	inline static bool IsMouseIn = false;

private:
	GameObjectManager* m_GameObjectManager;
};
