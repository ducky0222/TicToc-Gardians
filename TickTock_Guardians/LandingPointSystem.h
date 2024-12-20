#pragma once
#include "EventListener.h"
#include "System.h"

enum class eSceneState;

class LandingPointSystem : public System, public EventListener
{
public:
	LandingPointSystem(GameObjectManager* goManager);
	~LandingPointSystem() override = default;

private:
	void OnTriggerLandingPoint(std::any data);
	void OnSpawnLandingPointEffect(std::any data);

	void OnSetCurrentSceneState(std::any data); // from. Scene
	void OnUndoLandingPoint(std::any data); // from. Scene


	uint32_t mCurrentLandingOrder = 0;
};
