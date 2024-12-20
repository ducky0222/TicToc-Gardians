#pragma once
#include "System.h"
#include "EventListener.h"

class OnOffSystem : public System, public EventListener
{
public:
	OnOffSystem(GameObjectManager* goManager);
	~OnOffSystem() override = default;

private:
	void OnSetIsOn(std::any data);
};
