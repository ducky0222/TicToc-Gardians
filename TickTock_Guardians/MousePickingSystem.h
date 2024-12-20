#pragma once
#include "System.h"
#include "EventListener.h"

class MousePickingSystem : public System, public IUpdatable, public EventListener
{
public:
	struct ScreenClickData
	{
		int ScreenX;
		int ScreenY;
	};

public:
	MousePickingSystem(GameObjectManager* gameObjectManager);

	void Update(float deltaTime) override;

	void HandleMouseDownEvent(int ScreenX, int ScreenY);
	void HandleMouseUpEvent(int ScreenX, int ScreenY);
	void HandleMouseOverEvent(int ScreenX, int ScreenY);
	void HandleMouseOutEvent(int ScreenX, int ScreenY);

private:
	void renderDebugDraw();
};

