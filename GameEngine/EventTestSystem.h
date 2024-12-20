#pragma once
#include "EventListener.h"
#include "System.h"

class EventTestSystem : public System, public EventListener
{
public:

	struct TestStruct
	{
		int a, b;
		bool d, e;

		int counter = 0;
	};

	EventTestSystem(GameObjectManager* goManager)
		: System(goManager)
	{
		EventManager::Subscribe("Test", CreateListenerInfo(&EventTestSystem::OnTest));
		EventManager::Subscribe("Inner Test", CreateListenerInfo(&EventTestSystem::OnInnerTest));
	}

	void OnTest(std::any data)
	{
		static int num = 0;
		TestStruct ts;
		ts.counter = ++num;
		EventManager::ScheduleEvent("Inner Test", ts, 0.5f);
	}

	void OnInnerTest(std::any data)
	{
		count = std::any_cast<TestStruct>(data).counter;
		int kk = 0;
	}

private:
	int count = 0;
};
