#pragma once
#include "EventListener.h"
#include "System.h"

class DimensionCrackSystem : public System, public EventListener, public IStartable
{
public:
	DimensionCrackSystem(GameObjectManager* goManager);
	~DimensionCrackSystem() override = default;


	void Start(uint32_t gameObjectId) override;
	void Initialize() override;
	void Finish(uint32_t gameObjectId) override;
	void Finalize() override;


private:
	void OnTriggerDimensionCrack(std::any data);
	void OnExitDimensionCrack(std::any data);
	void OnCheckMissionClear(std::any data);
};
