#pragma once
#include "ComponentPool.h"
#include <list>

struct Component;
class System;

template <typename T>
concept IsComponent = std::is_base_of_v<Component, T> && !std::is_same_v<T, Component>;

// �̺�Ʈ�� ����� ComponentPoolManager �� �ʿ� ������
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
			// Ȱ��ȭ���� ���� ��ü�� �ִ��� Ȯ��
			if (activated < pool.size())
			{
				return &pool[activated++];
			}
			else continue;
		}

		// ��� ��ü�� Ȱ��ȭ�Ǿ� ���� ��� �߰� ����
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
			// ��� Ȱ��ȭ�� ��ü�� �ּ� ��
			for (size_t i = 0; i < activated; ++i)
			{
				if (used == &pool[i])
				{
					// �ǵڲ��� �ٲܰŴ�. �׷��� �ǵڲ��� ���ش�.
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

	/// ��ȣ���� ���ϱ�
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
