#include "pch.h"

#include "BindableResourceManager.h"

namespace Bind
{
	ResourceManager& ResourceManager::Get()
	{
		static ResourceManager RM;
		return RM;
	}
}