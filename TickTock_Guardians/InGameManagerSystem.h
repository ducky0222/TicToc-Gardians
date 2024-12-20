#pragma once

#include "System.h"
#include "EventListener.h"

class InGameManagerSystem : 
	public System, 
	public EventListener, 
	public IUpdatable, 
	public IStartable
{
public:
	InGameManagerSystem(GameObjectManager* gameObjManager);

	void Update(float deltaTime) override;

	void Start(uint32_t gameObjectId) override;
	void Initialize() override;
	void Finish(uint32_t gameObjectId) override;
	void Finalize() override;


	void OnPause(std::any data);
	void OnResume(std::any data);
	void OnResetStage(std::any data);
	void OnLevelButtonClick(std::any data);
	void OnNextLevelButtonClick(std::any data);
	void OnClearAllMission(std::any data);
	void OnNextLevelClick(std::any data);

	void OnQuit(std::any data); // Ãß°¡: È«ÁöÈ¯
};

