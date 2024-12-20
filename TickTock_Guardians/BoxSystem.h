#pragma once
#include "System.h"
#include "EventListener.h"
#include <SystemInterface.h>

class BoxSystem : public System, public IFixedUpdatable, public EventListener, public IStartable
{
public:

	BoxSystem(GameObjectManager* goManager);
	// IFixedUpdatable��(��) ���� ��ӵ�
	virtual void FixedUpdate(float deltaTime) override;
	void OnCollisionEnter(std::any data);
	void OnCollisionStay(std::any data);
	void OnCollisionExit(std::any data);
	void OnTriggerBox(std::any data);
	void OnExitBox(std::any data);
	void OnResetScene(std::any data);
	void OnBoxEffectActive(std::any data);
	// IStartable��(��) ���� ��ӵ�
	void Start(uint32_t gameObjectId) override;
	void Initialize() override;
	void Finish(uint32_t gameObjectId) override;
	void Finalize() override;
};

