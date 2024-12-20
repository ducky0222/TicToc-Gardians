#include "pch.h"
#include "MovementSystem.h"

#include "Movement.h"
#include "Transform.h"

void MovementSystem::Update(float deltaTime)
{
	for (Movement& movement : ComponentItr<Movement>())
	{
		auto transform = movement.Sibling<Transform>(m_GameObjectManager);
		if (transform)
			transform->LocalPosition += movement.Velocity * deltaTime;

		if (movement.Velocity.Length() > 0)
		{
			VSM::Vector3 normalizedVelocity;
			movement.Velocity.Normalize(normalizedVelocity);
			movement.Velocity -= normalizedVelocity * deltaTime;
		}
		else
		{
			movement.Velocity = VSM::Vector3::Zero;
		}
	}
}
