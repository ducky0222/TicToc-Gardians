#include "pch.h"
#include "ResetSystem.h"
#include "GameObjectManager.h"
#include "Component.h"

void ResetSystem::Reset()
{
	m_GameObjectManager->Reset();
	for (auto cp : TypeInfo::GetComponentMap() | std::views::values)
	{
		cp->ResetPool();
	}

}
