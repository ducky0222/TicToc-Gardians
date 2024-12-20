#pragma once

/// <summary>
/// 시스템을 나타내는 클래스
/// 상속받아서 구현 
/// </summary>

#include "ComponentPoolManager.h"
#include "SystemInterface.h"

class GameObjectManager;

class System
{
public:
	System(GameObjectManager* gameObjManager) : m_GameObjectManager(gameObjManager) {}
	virtual ~System() = default;


	/// 시스템의 내부 클래스로 컴포넌트를 순회하는 클래스
	/// range based for 문을 사용하기 위해 구현 >> begin(), end() 함수를 구현했다.
	/// public으로 선언된 그 자체의 내용을 변경 할 수 있어야 하기 때문에 
	/// reference_wrapper를 사용했다.
	template <typename T>
	class ComponentItr
	{
	public:
		using type = T;
		std::vector<std::reference_wrapper<T>> innerVector{};

		/// 생성자 단계에서 컴포넌트를 순회하며 innerVector에 넣는다.
		ComponentItr()
		{
			for (auto& iter : ComponentPoolManager::m_MultiPool<T>)
			{
				innerVector.reserve(innerVector.size() + iter.ActivatedIndex);
				innerVector.insert(innerVector.end(), iter.Pool.begin(), iter.Pool.begin() + iter.ActivatedIndex);
			}
		}

		/// 어차피 range based for 문을 사용하기 위해 구현한 클래스이기 때문에
		/// begin(), end() 함수만 구현했다.
		typename std::vector<std::reference_wrapper<T>>::iterator begin()
		{
			return innerVector.begin();
		}

		typename std::vector<std::reference_wrapper<T>>::iterator end()
		{
			return innerVector.end();

		}
	};

protected:
	GameObjectManager* m_GameObjectManager;
};

