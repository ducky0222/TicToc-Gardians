#pragma once
#include "System.h"
#include "EventListener.h"
#include "../PhysicEngine/HysxEngine.h"

class PhysicsSystem : public System, public IFixedUpdatable, public IStartable, public EventListener
{

public:
	using System::System;

	PhysicsSystem(GameObjectManager* gameObjectManager);

	void FixedUpdate(float deltaTime) override;


	// IStartable��(��) ���� ��ӵ�
	void Initialize() override;
	void Start(uint32_t gameObjectId) override;
	void Finish(uint32_t gameObjectId) override;
	void Finalize() override;
	void ChangeStaticToDynamic(std::any data);
	void ChangeDynamicToStatic(std::any data);

	// �� ��ȯ �� ���� ������ ���� �̺�Ʈ, �߰�:ȫ��ȯ
	void OnSceneTransitionEnter(std::any data);
	void OnSceneTransitionExit(std::any data);
};

