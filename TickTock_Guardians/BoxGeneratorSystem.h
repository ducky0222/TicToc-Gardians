#pragma once
#include "System.h"
#include "EventListener.h"
#include <SystemInterface.h>

class BoxGeneratorSystem : public System, public IFixedUpdatable,public IStartable, public EventListener
{
public:

	BoxGeneratorSystem(GameObjectManager* goManager);


	// IFixedUpdatable을(를) 통해 상속됨
	void FixedUpdate(float deltaTime) override;
	void Start(uint32_t  GameobjectID) override;
	void OnTriggerButton(std::any data);
	void OnResetScene(std::any data);


	// IStartable을(를) 통해 상속됨
	void Initialize() override;

	void Finish(uint32_t gameObjectId) override;

	void Finalize() override;

};

