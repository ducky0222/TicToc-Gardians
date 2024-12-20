#include "pch.h"
#include "ComponentPoolManager.h"
#include "Component.h"

uint32_t ComponentPoolManager::Release(Component* handle)
{
	return handle->Release();
}
