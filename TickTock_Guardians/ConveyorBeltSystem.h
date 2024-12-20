#pragma once

#include "System.h"
#include "EventListener.h"

class ConveyorBeltSystem : public System, public IStartable, public EventListener
{
public:
	ConveyorBeltSystem(GameObjectManager* gameObjectManager);

	void Start(uint32_t gameObjectId) override;
	void OnCollisionStay(std::any data);
	void OnTriggerButton(std::any data);
	void OnExitButton(std::any data);

	// IStartable을(를) 통해 상속됨
	void Initialize() override;
	void Finish(uint32_t gameObjectId) override;
	void Finalize() override;
};

