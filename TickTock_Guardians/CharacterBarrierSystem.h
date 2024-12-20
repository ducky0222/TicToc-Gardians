#pragma once
#include <System.h>

#include "EventListener.h"

class CharacterBarrierSystem :
    public System, public IFixedUpdatable, public EventListener, public IStartable
{
public:
    CharacterBarrierSystem(GameObjectManager* goManager);

    void Start(uint32_t gameObjectId) override;
    void Initialize() override;
    void Finish(uint32_t gameObjectId) override;
    void Finalize() override;

    void FixedUpdate(float deltaTime) override;

};

