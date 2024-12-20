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


	// IStartable을(를) 통해 상속됨
	void Initialize() override;
	void Start(uint32_t gameObjectId) override;
	void Finish(uint32_t gameObjectId) override;
	void Finalize() override;
	void ChangeStaticToDynamic(std::any data);
	void ChangeDynamicToStatic(std::any data);

	// 씬 전환 시 액터 재등록을 위한 이벤트, 추가:홍지환
	void OnSceneTransitionEnter(std::any data);
	void OnSceneTransitionExit(std::any data);
};

