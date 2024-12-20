#include "pch.h"
#include "PlayerSystem.h"

#include "Player.h"
#include "Input.h"
#include "HysxEngine.h"
#include "HysxEngine.h"

void PlayerSystem::Update(float deltaTime)
{
	for (Player& player : ComponentItr<Player>())
	{
// 		auto movement = player.Sibling<Movement>(m_GameObjectManager);
// 		if (movement)
// 		{
// 			if (Input::GetKey(KEY::RIGHT))
// 			{
// 				movement->Velocity.x = 1.0f;
// 			}
// 			if (Input::GetKey(KEY::LEFT))
// 			{
// 				movement->Velocity.x = -1.0f;
// 			}
// 			if (Input::GetKey(KEY::UP))
// 			{
// 				movement->Velocity.y = 1.0f;
// 			}
// 			if (Input::GetKey(KEY::DOWN))
// 			{
// 				movement->Velocity.y = -1.0f;
// 			}
// 		}
		
		
	}
}
