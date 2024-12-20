#include "pch.h"
#include "OnOffSystem.h"

#include "OnOff.h"
#include "GameObjectManager.h"

OnOffSystem::OnOffSystem(GameObjectManager* goManager)
	: System(goManager)
{
	EventManager::Subscribe("OnSetIsOn", CreateListenerInfo(&OnOffSystem::OnSetIsOn));
}

void OnOffSystem::OnSetIsOn(std::any data)
{
	auto [id, isOn] = std::any_cast<std::pair<uint32_t, bool>>(data);

	m_GameObjectManager->QueryComponent<OnOff>(id)->IsOn = isOn;
}
