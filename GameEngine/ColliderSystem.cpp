#include "pch.h"
#include "ColliderSystem.h"

#include "Collider.h"
#include "OnOff.h"

void ColliderSystem::Update(float deltaTime)
{
	auto itr = ComponentItr<BoxCollider>();
	for (BoxCollider& bc : itr)
	{
		if(bc.Sibling<OnOff>(m_GameObjectManager)->IsOn == false) continue;

		for (BoxCollider& bc2 : itr)
		{
			if (&bc == &bc2) continue;
			if (bc2.Sibling<OnOff>(m_GameObjectManager)->IsOn == false) continue;

			auto collEvent = bc.Sibling<CollideEvent>(m_GameObjectManager);
			auto collEvent2 = bc2.Sibling<CollideEvent>(m_GameObjectManager);

			if (IsCollide(bc, bc2))
			{
				if (collEvent)
					collEvent->IsCollide = true;
				if (collEvent2)
					collEvent2->IsCollide = true;
			}
			else
			{
				if (collEvent)
					collEvent->IsCollide = false;
				if (collEvent2)
					collEvent2->IsCollide = false;
			}

		}
	}
}

bool ColliderSystem::IsCollide(const BoxCollider& bc1, const BoxCollider& bc2)
{
	DirectX::BoundingOrientedBox lhs;
	lhs.Center = bc1.Center;
	lhs.Extents = bc1.Size;
	lhs.Orientation = bc1.Orientation;

	DirectX::BoundingOrientedBox rhs;
	rhs.Center = bc2.Center;
	rhs.Extents = bc2.Size;
	rhs.Orientation = bc2.Orientation;

	return lhs.Intersects(rhs);
}

void ColliderEventSystem::Update(float deltaTime)
{
	for (CollideEvent& ce : ComponentItr<CollideEvent>())
	{
		ce.IsEnter = false;
		ce.IsStay = false;
		ce.IsExit = false;

		if (ce.IsCollide == true)
		{
			if (ce.IsPrevCollide == true)
			{
				ce.IsStay = false;
			}
			else
			{

			}
		}

		ce.IsPrevCollide = ce.IsCollide;
	}
}
