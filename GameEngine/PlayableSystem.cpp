#include "pch.h"
#include "PlayableSystem.h"
#include "Character.h"
#include "Input.h"
#include "Transform.h"
#include "../ABGraphics/RenderManager.h"

//test
#include "SerializeSystem.h"


static float stime = 0;
static VSM::Quaternion lastq = VSM::Quaternion::Identity;
static VSM::Quaternion targetq = VSM::Quaternion::Identity;
static float slerpTime = 0;
static bool slerping = false;

void PlayableSystem::Update(float deltaTime)
{
	for (Character& character : ComponentItr<Character>())
	{
		if (Input::GetKeyDown(KEY::I))
		{
			SerializeSystem(m_GameObjectManager).Instantiate("../Data/Prefab/Capsule.prefab", character.GameObjectId, true);
		}

		if(character.i >= 600)
		{
			continue;
		}

		stime += deltaTime;
		Transform* transform = character.Sibling<Transform>(m_GameObjectManager);

		if (character.InitialPosition == Vector3::Zero)
		{
			character.InitialPosition = transform->LocalPosition;
		}


		if (Input::GetKey(KEY::RIGHT))
		{
			character.Direction += VSM::Vector3(0.99f, 0, -0.98f);
			slerping = true;
			character.Bitsets |= 1 << static_cast<unsigned>(KEY::RIGHT);
		}
		if (Input::GetKey(KEY::LEFT))
		{
			character.Direction += VSM::Vector3(-0.98f, 0, 0.99f);
			slerping = true;
			character.Bitsets |= 1 << static_cast<unsigned>(KEY::LEFT);
		}
		if (Input::GetKey(KEY::UP))
		{
			character.Direction += VSM::Vector3(1, 0, 1);
			slerping = true;
			character.Bitsets |= 1 << static_cast<unsigned>(KEY::UP);

		}
		if (Input::GetKey(KEY::DOWN))
		{
			character.Direction += VSM::Vector3(-1, 0, -1);
			slerping = true;
			character.Bitsets |= 1 << static_cast<unsigned>(KEY::DOWN);
		}

		RenderManager::GetInstance().ApplySkinnedModelAnimation("dancing", "dancing", "mixamo.com", stime, true);
	}


}

void PlayableSystem::FixedUpdate(float deltaTime)
{
	for (Character& character : ComponentItr<Character>())
	{
		Transform* transform = character.Sibling<Transform>(m_GameObjectManager);

		character.Direction.Normalize();


		if (character.i >= 600)
		{
			character.IsReplayable = true;
			continue;
		}

		if (slerping)
		{
			targetq = VSM::Quaternion::LookRotation(character.Direction, VSM::Vector3::Up);
			lastq = transform->LocalRotation;

			slerpTime = 0;
		}

		if (slerpTime < 1)
		{
			slerpTime += deltaTime * 15;
			VSM::Quaternion q = VSM::Quaternion::Slerp(lastq, targetq, slerpTime);
			transform->LocalRotation = q;
			slerping = false;
		}
		else
		{
			targetq = transform->LocalRotation;
		}
		transform->LocalPosition += character.Direction* character.Speed * deltaTime;
		character.Direction = Vector3::Zero;



		character.Tracer[character.i] = character.Bitsets;
		character.i++;
		character.Bitsets = 0;
	}

	for (Character& character : ComponentItr<Character>())
	{
		if(character.IsReplayable == true)
		{
			Transform* transform = character.Sibling<Transform>(m_GameObjectManager);

			if (character.InitialPosition != Vector3::Zero)
			{
				transform->LocalPosition = character.InitialPosition;
				character.InitialPosition = Vector3::Zero;
			}


			if(character.replayi >= 600)
			{
				continue;
			}
			if (character.Tracer[character.replayi][static_cast<unsigned>(KEY::RIGHT)])
			{
				character.Direction += VSM::Vector3(0.99f, 0, -0.98f);
			}
			if (character.Tracer[character.replayi][static_cast<unsigned>(KEY::LEFT)])
			{
				character.Direction += VSM::Vector3(-0.98f, 0, 0.99f);
			}
			if (character.Tracer[character.replayi][static_cast<unsigned>(KEY::UP)])
			{
				character.Direction += VSM::Vector3(1, 0, 1);
			}
			if (character.Tracer[character.replayi][static_cast<unsigned>(KEY::DOWN)])
			{
				character.Direction += VSM::Vector3(-1, 0, -1);
			}
			character.Direction.Normalize();

			character.replayi++;
			transform->LocalPosition += character.Direction * character.Speed * deltaTime;
			character.Direction = Vector3::Zero;

		}
	}
}
