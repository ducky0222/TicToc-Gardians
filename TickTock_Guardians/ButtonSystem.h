#pragma once
#include "System.h"
#include"EventListener.h"
#include <SystemInterface.h>

class ButtonSystem : public System, public IFixedUpdatable, public EventListener
{
public:
    ButtonSystem(GameObjectManager* goManager) ;
    void OnButtonClicked(std::any data);
    void OnButtonUnClicked(std::any data);
    void FixedUpdate(float deltaTime) override;
    void OnSpawnButtonEffect(std::any data);
};

