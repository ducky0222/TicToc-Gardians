#pragma once

#include <any>
#include <queue>

class EventListener;
using EventType = std::string;
using EventCallback = std::function<void(std::any)>;

// 이벤트와 그 남은 실행 시간을 관리할 구조체
struct DelayedEvent {
	float remainingTime; // 이벤트 실행까지 남은 시간
	std::function<void()> event;

	DelayedEvent(float delay, std::function<void()> event)
		: remainingTime(delay), event(event) {}
};

struct ListenerInfo {
	EventListener* listener;
	EventCallback callback;

	ListenerInfo(EventListener* handler, EventCallback callback)
		: listener(handler), callback(std::move(callback)) {}
};


class EventManager
{
public:
	static void Update(float deltaTime)
	{
		// 실행할 이벤트를 별도의 목록으로 이동
		std::vector<DelayedEvent> eventsToExecute;

		auto it = ranges::remove_if(
			m_DelayedEvents, [deltaTime, &eventsToExecute](DelayedEvent& event)
			{
				event.remainingTime -= deltaTime;
				if (event.remainingTime <= 0)
				{
					eventsToExecute.push_back(std::move(event));
					return true; // 원본 목록에서 제거
				}
				return false;
			}).begin();

		m_DelayedEvents.erase(it, m_DelayedEvents.end());

		// 지연 이벤트 실행
		for (auto& event : eventsToExecute)
			event.event();

		// 즉시 실행 이벤트 처리
		for (auto& event : m_ImmediateEvents)
			event(); // 이벤트 실행

		m_ImmediateEvents.clear(); // 처리된 즉시 실행 이벤트 리스트 클리어
	}

	// 이벤트 구독
	static void Subscribe(const EventType& type, const ListenerInfo& listenerInfo)
	{
		m_Listeners[type].push_back(listenerInfo);
	}

	// 지연 이벤트 예약 (PublishImmediate 바인딩)
	static void ScheduleEvent(const EventType& type, std::any data = std::any{}, float delay = 0)
	{
		if (delay <= 0)
			m_ImmediateEvents.emplace_back([=]() { PublishImmediate(type, data); });
		else
			m_DelayedEvents.emplace_back(delay, [=]() { PublishImmediate(type, data); });
	}

	// 즉각 이벤트 실행
	static void PublishImmediate(const EventType& type, std::any data = std::any{})
	{
		if (m_Listeners.find(type) != m_Listeners.end())
			for (auto& listener : m_Listeners[type])
				listener.callback(data);
	}

	// Engine Functions
	static void OnDestroy(uint32_t id)
	{
		ScheduleEvent("OnDestroy", id);
	}

	static void OnDeleteGameObject(uint32_t id)
	{
		ScheduleEvent("OnDeleteGameObject", id);
	}

	static void OnDeleteComponent(Component* ptr)
	{
		PublishImmediate("OnDeleteComponent", ptr);
	}

	static void OnLoadScene(std::string sceneName)
	{
		ScheduleEvent("OnLoadScene", sceneName);
	}

private:
	static void RemoveListener(EventListener* handler) {
		for (auto& listenerInfos : m_Listeners | views::values)
		{
			auto it = std::remove_if(listenerInfos.begin(), listenerInfos.end(),
				[handler](const ListenerInfo& info) { return info.listener == handler; });

			listenerInfos.erase(it, listenerInfos.end());
		}
	}

private:
	inline static std::unordered_map<EventType, std::vector<ListenerInfo>> m_Listeners;
	inline static std::vector<DelayedEvent> m_DelayedEvents;
	inline static std::list<std::function<void()>> m_ImmediateEvents; // 즉시 실행할 이벤트 리스트

	friend class EventListener;
};