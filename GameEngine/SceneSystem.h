#pragma once
#include "EventListener.h"
#include "System.h"

class SceneSystem : public System, public EventListener
{
public:
	SceneSystem(GameObjectManager* gameObjectManager);
	SceneSystem(GameObjectManager* gameObjectManager, std::set<std::string> searchPaths);
	virtual ~SceneSystem() = default;

	virtual void OnSceneTransition(std::any data);

protected:
	std::set<std::string> mSearchPaths;
	std::map<std::string, std::string> mSceneNamePaths;
};
