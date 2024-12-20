#pragma once
#include <System.h>
#include <EventListener.h>

class Button2DSystem :
	public System, public EventListener
{
public:
	Button2DSystem(GameObjectManager* gameObjectManager);

	void OnSceneChange(std::any sceneName);
};

