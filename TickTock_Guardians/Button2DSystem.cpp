#include "pch.h"
#include "Button2DSystem.h"

Button2DSystem::Button2DSystem(GameObjectManager* gameObjectManager)\
	: System(gameObjectManager)
{
	EventManager::Subscribe("OnSceneChange", CreateListenerInfo(&Button2DSystem::OnSceneChange));
}

void Button2DSystem::OnSceneChange(std::any sceneName)
{
	assert(nullptr);
}
