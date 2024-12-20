#include "pch.h"
#include "TransformSystem.h"

#include "Transform.h"

void TransformSystem::Update(float deltaTime)
{
	std::list<uint32_t> transformQueue;

	for (Transform& transform : ComponentItr<Transform>())
	{
		if (transform.Sibling<Parent>(m_GameObjectManager))
			continue;

		// 최상위 트랜스폼만 큐에 삽입
		transformQueue.push_back(transform.GameObjectId);
	}

	// 게임 오브젝트별 최상위 트랜스폼을 통해 계층적으로 업데이트
	while (!transformQueue.empty())
	{
		uint32_t transformid = transformQueue.front();
		Transform* transform = m_GameObjectManager->QueryComponent<Transform>(transformid);

		transformQueue.pop_front();

		if (!transform)
			continue;

		ProcessTransform(transform);

		auto children = transform->Sibling<Children>(m_GameObjectManager);
		if (children)
		{
			// 다음 계층 삽입
			transformQueue.insert(transformQueue.end(), children->ChildrenID.begin(), children->ChildrenID.end());
		}
	}
}

void TransformSystem::Initialize()
{
	Update(0);
}

void TransformSystem::Start(uint32_t id)
{
	std::list<uint32_t> transformQueue;

	transformQueue.push_back(id);

	while (!transformQueue.empty())
	{
		uint32_t transformid = transformQueue.front();
		Transform* transform = m_GameObjectManager->QueryComponent<Transform>(transformid);
		transformQueue.pop_front();

		ProcessTransform(transform);

		auto children = transform->Sibling<Children>(m_GameObjectManager);
		if (children)
		{
			transformQueue.insert(transformQueue.end(), children->ChildrenID.begin(), children->ChildrenID.end());
		}
	}
}

void TransformSystem::Finish(uint32_t id)
{

}

void TransformSystem::Finalize()
{

}


void TransformSystem::Optimize()
{
	uint32_t count = 0;
	uint32_t size = m_GameObjectManager->m_AttachedComponents.size();
	for (int i = 0; i < size; i++)
	{
		auto findedMap = m_GameObjectManager->m_AttachedComponents.find(count);
		if (findedMap != m_GameObjectManager->m_AttachedComponents.end())
		{
			count++;
			continue;
		}

		auto iter = m_GameObjectManager->m_AttachedComponents.begin();
		for (int i = 0; i < count; i++)
		{
			iter++;
		}

		if (iter->first <= count)
		{
			count++;
			continue;
		}

		// 여기 밑에서부턴 최적화 대상
		int temp = iter->first;
		auto newfindedMap = m_GameObjectManager->m_AttachedComponents.find(temp);
		m_GameObjectManager->m_AttachedComponents.emplace(count, newfindedMap->second);
		m_GameObjectManager->m_AttachedComponents.erase(iter);
		auto compMap = m_GameObjectManager->m_AttachedComponents.find(count)->second;
		for (auto comp : compMap)
		{
			comp.second->GameObjectId = count;
		}

		auto chechIter = std::find_if(m_GameObjectManager->m_GameObjects.begin(), m_GameObjectManager->m_GameObjects.end(), [count, temp](GameObject& obj)
			{
				if (obj.Id == temp)
				{
					obj.Id = count;
					return true;
				}
				return false;
			});
		assert(chechIter != m_GameObjectManager->m_GameObjects.end());

		Children* children = m_GameObjectManager->QueryComponent<Children>(count);
		if (children)
		{
			for (auto& child : children->ChildrenID)
			{
				Parent* parent = m_GameObjectManager->QueryComponent<Parent>(child);
				if (parent->ParentID == temp)
				{
					parent->ParentID = count;
				}
			}
		}

		Parent* parent = m_GameObjectManager->QueryComponent<Parent>(count);
		if (parent)
		{
			Children* children = m_GameObjectManager->QueryComponent<Children>(parent->ParentID);
			auto checkIter = std::find_if(children->ChildrenID.begin(), children->ChildrenID.end(), [temp, count](uint32_t& id)
				{
					if (id == temp)
					{
						id = count;
						return true;
					}
					return false;
				});
			assert(checkIter != children->ChildrenID.end());
		}

		count++;
	}

	m_GameObjectManager->m_NextGameObjectIndex = count;
}

void TransformSystem::ProcessTransform(Transform* transform)
{
	VSM::Matrix LocalRotMatrix = VSM::Matrix::CreateFromQuaternion(transform->LocalRotation);
	VSM::Matrix LocalScaleMatrix = VSM::Matrix::CreateScale(transform->LocalScale);
	VSM::Matrix LocalTranslationMatrix = VSM::Matrix::CreateTranslation(transform->LocalPosition);

	transform->LocalMatrix = LocalScaleMatrix * LocalRotMatrix * LocalTranslationMatrix;

	if (transform->Sibling<Parent>(m_GameObjectManager))
	{
		uint32_t parentID = transform->Sibling<Parent>(m_GameObjectManager)->ParentID;
		transform->WorldMatrix = transform->LocalMatrix * m_GameObjectManager->QueryComponent<Transform>(parentID)->WorldMatrix;
	}
	else
	{
		transform->WorldMatrix = transform->LocalMatrix;
	}

	transform->WorldMatrix.Decompose(transform->WorldScale, transform->WorldRotation, transform->WorldPosition);
}

void TransformSystem::GenerateQuaternion(Transform* transform)
{
	//VSM::Vector3 radianEuler = transform->LocalEuler * (Very::SimpleMath::XM_PI / 180.0f);
	//transform->LocalRotation = VSM::Quaternion::CreateFromYawPitchRoll(radianEuler.y, radianEuler.x, radianEuler.z);
}


bool ParentChildrenSystem::CheckParent(uint32_t parent, uint32_t child)
{
	Parent* iterator = m_GameObjectManager->QueryComponent<Parent>(child);

	while (iterator != nullptr)
	{
		if (!m_GameObjectManager->QueryComponent<Children>(iterator->ParentID))
		{
			iterator = m_GameObjectManager->QueryComponent<Parent>(iterator->ParentID);
			continue;
		}
		if (iterator->ParentID == parent)
		{
			return true;
		}
		iterator = m_GameObjectManager->QueryComponent<Parent>(iterator->ParentID);
	}

	return false;
}

void ParentChildrenSystem::AddChild(GameObject* parent, GameObject* child)
{
	AddChild(parent->Id, child->Id);
}

void ParentChildrenSystem::AddChild(uint32_t parent, uint32_t child)
{
	// 동일 객체인지 검사
	if (parent == child)
	{
		return;
	}

	// 이미 부모인지 검사 
	if (CheckParent(child, parent))
	{
		return;
	}

	Transform* childTransform = m_GameObjectManager->QueryComponent<Transform>(child);

	if (Parent* parentOfChild = m_GameObjectManager->QueryComponent<Parent>(child); parentOfChild)
	{
		// 예외처리를 더 해 줘야한다...
		if (parentOfChild->ParentID == parent)
		{
			// 같은 부모에 대해 그냥 리턴
			return;
		}
		// 다른 부모에 대한 코드
		else
		{
			// 기존 부모의 자식 목록에서 제거
			auto children = m_GameObjectManager->QueryComponent<Children>(parentOfChild->ParentID);
			children->ChildrenID.remove_if(
				[child](uint32_t id)
				{
					return id == child;
				}
			);

			if (children->ChildrenID.empty())
			{
				//m_GameObjectManager->RemoveComponent(children);
				EventManager::OnDeleteComponent(children);
			}

			parentOfChild->ParentID = parent;

			childTransform->LocalPosition = childTransform->WorldPosition;
			childTransform->LocalRotation = childTransform->WorldRotation;
			childTransform->LocalScale = childTransform->WorldScale;
		}
	}
	else
	{
		m_GameObjectManager->AddComponent<Parent>(child)->ParentID = parent;
	}

	auto children = m_GameObjectManager->QueryComponent<Children>(parent);
	if (children)
	{
		children->ChildrenID.push_back(child);
	}
	else
	{
		m_GameObjectManager->AddComponent<Children>(parent)->ChildrenID.push_back(child);
	}

	Transform* parentTransform = m_GameObjectManager->QueryComponent<Transform>(parent);

	// 부모의 월드 행렬을 기준으로 자식의 로컬 행렬을 계산
	VSM::Matrix parentWorldMatrix = parentTransform->WorldMatrix;
	VSM::Matrix childWorldMatrix = childTransform->WorldMatrix;

	VSM::Matrix childLocalMatrix = childWorldMatrix * parentWorldMatrix.Invert();

	childLocalMatrix.Decompose(childTransform->LocalScale, childTransform->LocalRotation, childTransform->LocalPosition);
}

void ParentChildrenSystem::RemoveChild(uint32_t parent, uint32_t child)
{
	if (Parent* parentOfChild = m_GameObjectManager->QueryComponent<Parent>(child); parentOfChild)
	{
		if (parentOfChild->ParentID == parent)
		{
			auto children = m_GameObjectManager->QueryComponent<Children>(parent);
			children->ChildrenID.remove_if(
				[child](uint32_t id)
				{
					return id == child;
				}
			);

			if (children->ChildrenID.empty())
			{
				//m_GameObjectManager->RemoveComponent(children);
				EventManager::OnDeleteComponent(children);
			}

			//m_GameObjectManager->RemoveComponent(parentOfChild);
			EventManager::OnDeleteComponent(parentOfChild);


			Transform* childTransform = m_GameObjectManager->QueryComponent<Transform>(child);
			childTransform->LocalPosition = childTransform->WorldPosition;
			childTransform->LocalRotation = childTransform->WorldRotation;
			childTransform->LocalScale = childTransform->WorldScale;
		}
	}
}

void ParentChildrenSystem::RemoveParent(uint32_t childID)
{
	if (Parent* parent = m_GameObjectManager->QueryComponent<Parent>(childID); parent)
	{
		RemoveChild(parent->ParentID, childID);
	}
}
