#pragma once
#include "EventManager.h"

class EventListener
{
public:
	virtual ~EventListener()
	{
		EventManager::RemoveListener(this);
	}

protected:
	template <typename T> requires std::derived_from<T, EventListener>
	ListenerInfo CreateListenerInfo(void (T::* func)(std::any))
	{
		return ListenerInfo(this, [this, func](std::any data) { (static_cast<T*>(this)->*func)(data); });
	}
};
