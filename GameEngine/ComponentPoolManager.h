#pragma once
#include "ComponentPool.h"
#include <list>

struct Component;
class System;

template <typename T>
concept IsComponent = std::is_base_of_v<Component, T> && !std::is_same_v<T, Component>;

// 이벤트로 만들면 ComponentPoolManager 가 필요 없을듯
class ComponentPoolManager
{
public:
	ComponentPoolManager() = default;
	~ComponentPoolManager() = default;

	template <IsComponent T>
	static T* Get()
	{
		for (auto& cp : m_MultiPool<T>)
		{
			auto& pool = cp.Pool;
			auto& activated = cp.ActivatedIndex;
			// 활성화되지 않은 객체가 있는지 확인
			if (activated < pool.size())
			{
				return &pool[activated++];
			}
			else continue;
		}

		// 모든 객체가 활성화되어 있을 경우 추가 생성
		m_MultiPool<T>.emplace_back();
		auto& pool = m_MultiPool<T>.back().Pool;
		auto& activated = m_MultiPool<T>.back().ActivatedIndex;
		return &pool[activated++];
	}

	template <IsComponent T>
	static uint32_t Release(T* used)
	{
		for (auto& cp : m_MultiPool<T>)
		{
			auto& pool = cp.Pool;
			auto& activated = cp.ActivatedIndex;
			// 모든 활성화된 객체와 주소 비교
			for (size_t i = 0; i < activated; ++i)
			{
				if (used == &pool[i])
				{
					// 맨뒤꺼랑 바꿀거다. 그래서 맨뒤꺼는 빼준다.
					m_MultiPool<T>.back().ActivatedIndex--;
					pool[i] = std::move(*(m_MultiPool<T>.back().Pool.begin() + m_MultiPool<T>.back().ActivatedIndex));
					if (m_MultiPool<T>.back().ActivatedIndex == 0)
					{
						m_MultiPool<T>.pop_back();
					}

					if (m_MultiPool<T>.empty())
					{
						return 0;
					}

					return pool[i].GameObjectId;
				}
			}
		}

		return 0;
	}

	/// 상호참조 피하기
	static uint32_t Release(Component* handle);
	
	template <IsComponent T>
	static void Reset()
	{
		m_MultiPool<T>.clear();
	}

private:
	template <typename T> requires IsComponent<T>
	inline static std::list<ComponentPool<T>> m_MultiPool;

	friend System;
	friend class GameProcess;
};
