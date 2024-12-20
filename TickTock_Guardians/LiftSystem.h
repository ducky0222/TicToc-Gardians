#pragma once
#include <System.h>
#include "EventListener.h"

/// <summary>
/// 리프트 시스템 
/// 
/// 해당 리프트 시스템은 업데이트와 이벤트 2개로 처리할수 있는데
/// 일단은 이벤트를 받고 업데이트에서 처리하는 느낌으로 처리한다. 
/// </summary>
class LiftSystem :
    public System, public IFixedUpdatable, public EventListener, public IStartable
{
public:
    LiftSystem(GameObjectManager* goManager);
    
    void OnTriggerButton(std::any data);
    void OnExitButton(std::any data);
    void Initialize() override;
    void Start(uint32_t id) override;
    void Finish(uint32_t id) override;
    void Finalize() override;

    void FixedUpdate(float deltaTime) override;
};

