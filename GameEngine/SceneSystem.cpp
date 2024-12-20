#include "pch.h"
#include <filesystem>

#include "EntityIdentity.h"
#include "SceneSystem.h"
#include "EventManager.h"
#include "SerializeSystem.h"
#include "SceneComponent.h"

SceneSystem::SceneSystem(GameObjectManager* gameObjectManager)
	: SceneSystem(gameObjectManager, std::set<std::string>())
{
}

SceneSystem::SceneSystem(GameObjectManager* gameObjectManager, std::set<std::string> searchPaths)
	: System(gameObjectManager)
	, mSearchPaths(searchPaths)
{
	EventManager::Subscribe("OnSceneTransition", CreateListenerInfo(&SceneSystem::OnSceneTransition));

	mSearchPaths.insert("../Data/Scene/");

	for (const std::string searchPath : mSearchPaths)
	{
		for (auto& scenePath : std::filesystem::directory_iterator(searchPath))
		{
			const std::string& filePath = scenePath.path().string();
			const std::string& fileName = scenePath.path().filename().string();
			mSceneNamePaths.insert({ fileName, filePath });
		}
	}
}

void SceneSystem::OnSceneTransition(std::any data)
{
	std::string sceneName = std::any_cast<std::string>(data);
	auto find = mSceneNamePaths.find(sceneName);

	if (find != mSceneNamePaths.end())
	{
		SerializeSystem sSys{ m_GameObjectManager };
		EventManager::PublishImmediate("OnFinalize");
		sSys.DeserializeScene(nullptr, find->second);
		EventManager::PublishImmediate("OnInitialize");
	}
}