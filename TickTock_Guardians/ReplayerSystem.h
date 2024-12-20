#pragma once
#include "System.h"
#include "EventListener.h"

class ReplayerSystem : public System, public IFixedUpdatable, public EventListener
{
public:
	ReplayerSystem(GameObjectManager* goManager);
	~ReplayerSystem() override = default;

	void FixedUpdate(float deltaTime) override;

private:
	void OnStartReplay(std::any data);
	void OnStartMoving(std::any data);
	void OnSkipTracing(std::any data);
	void OnTraceDirection(std::any data);
	void OnUndo(std::any data);
	void OnStopReplay(std::any data);
};
