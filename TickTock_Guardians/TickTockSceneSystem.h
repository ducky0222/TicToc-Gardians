#pragma once
#include "SceneSystem.h"
#include "Component.h"
#include "EventListener.h"

class TickTockScene;
enum class eSceneState;

class TickTockSceneSystem : 
	public SceneSystem, public IStartable, public IUpdatable
{
public:

	TickTockSceneSystem(GameObjectManager* gameObjectManager);
	virtual ~TickTockSceneSystem() = default;

	virtual void Initialize() override;
	virtual void Start(uint32_t gameObjectId) override;
	virtual void Finish(uint32_t gameObjectId) override;
	virtual void Finalize() override;

	virtual void Update(float deltaTime) override;

	void OnMapSearchSceneTransition(std::any data);

private:
	std::map<eSceneState, std::shared_ptr<TickTockScene>> mScenes;
	std::shared_ptr<TickTockScene> mCurrentScene;
	
	bool IsIWantGoToSearchScene = false;

};
