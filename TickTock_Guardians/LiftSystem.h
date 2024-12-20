#pragma once
#include <System.h>
#include "EventListener.h"

/// <summary>
/// ����Ʈ �ý��� 
/// 
/// �ش� ����Ʈ �ý����� ������Ʈ�� �̺�Ʈ 2���� ó���Ҽ� �ִµ�
/// �ϴ��� �̺�Ʈ�� �ް� ������Ʈ���� ó���ϴ� �������� ó���Ѵ�. 
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

