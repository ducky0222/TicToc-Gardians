#pragma once

/// <summary>
/// �ý����� ��Ÿ���� Ŭ����
/// ��ӹ޾Ƽ� ���� 
/// </summary>

#include "ComponentPoolManager.h"
#include "SystemInterface.h"

class GameObjectManager;

class System
{
public:
	System(GameObjectManager* gameObjManager) : m_GameObjectManager(gameObjManager) {}
	virtual ~System() = default;


	/// �ý����� ���� Ŭ������ ������Ʈ�� ��ȸ�ϴ� Ŭ����
	/// range based for ���� ����ϱ� ���� ���� >> begin(), end() �Լ��� �����ߴ�.
	/// public���� ����� �� ��ü�� ������ ���� �� �� �־�� �ϱ� ������ 
	/// reference_wrapper�� ����ߴ�.
	template <typename T>
	class ComponentItr
	{
	public:
		using type = T;
		std::vector<std::reference_wrapper<T>> innerVector{};

		/// ������ �ܰ迡�� ������Ʈ�� ��ȸ�ϸ� innerVector�� �ִ´�.
		ComponentItr()
		{
			for (auto& iter : ComponentPoolManager::m_MultiPool<T>)
			{
				innerVector.reserve(innerVector.size() + iter.ActivatedIndex);
				innerVector.insert(innerVector.end(), iter.Pool.begin(), iter.Pool.begin() + iter.ActivatedIndex);
			}
		}

		/// ������ range based for ���� ����ϱ� ���� ������ Ŭ�����̱� ������
		/// begin(), end() �Լ��� �����ߴ�.
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

