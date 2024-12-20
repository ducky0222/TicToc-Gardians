#include "pch.h"
#include "ReplayerSystem.h"

#include "Character.h"
#include "Input.h"
#include "Replayer.h"
#include "RigidBody.h"
//#include "TraceSizeSetter.h"
#include "Transform.h"
#include "OnOff.h"
#include <algorithm>

#include "SkinnedMeshRenderer.h"
#include "TransformSystem.h"

ReplayerSystem::ReplayerSystem(GameObjectManager* goManager)
	: System(goManager)
{
	EventManager::Subscribe("OnTraceDirection", CreateListenerInfo(&ReplayerSystem::OnTraceDirection));
	EventManager::Subscribe("OnStartMoving", CreateListenerInfo(&ReplayerSystem::OnStartMoving));
	EventManager::Subscribe("OnStartReplay", CreateListenerInfo(&ReplayerSystem::OnStartReplay));
	EventManager::Subscribe("OnSkipTracing", CreateListenerInfo(&ReplayerSystem::OnSkipTracing));

	EventManager::Subscribe("OnUndo", CreateListenerInfo(&ReplayerSystem::OnUndo));
	EventManager::Subscribe("OnStopReplay", CreateListenerInfo(&ReplayerSystem::OnStopReplay));
}
 
void ReplayerSystem::FixedUpdate(float deltaTime)
{
	for (Replayer& replayer : ComponentItr<Replayer>())
	{
		if (!replayer.Sibling<OnOff>(m_GameObjectManager)->IsOn) continue;

		// 리플레이 가능 상태 확인
		if (!replayer.IsReplaying)
			continue;

		if (!replayer.IsFinishReplaying && replayer.ReplayIndex >= replayer.Tracer.size())
		{
			replayer.IsFinishReplaying = true;
			replayer.IsReplaying = false;

			// 투명화
			if (const auto skinned = m_GameObjectManager->QueryComponent<SkinnedMeshRenderer>(replayer.GameObjectId))
			{
				skinned->Alpha = 1.0f;
			}

			EventManager::ScheduleEvent("OnFinishReplay", replayer.GameObjectId);
			continue;
		}

		auto rigidbody = replayer.Sibling<RigidBody>(m_GameObjectManager);
		assert(rigidbody);

		auto character = replayer.Sibling<Character>(m_GameObjectManager);
		assert(character);


		if (replayer.Tracer[replayer.ReplayIndex][ACTION::LEFT])
		{
			character->Direction += VSM::Vector3(-0.99f, 0, 0.99f);
		}
		if (replayer.Tracer[replayer.ReplayIndex][ACTION::RIGHT])
		{
			character->Direction += VSM::Vector3(0.99f, 0, -0.99f);
		}
		if (replayer.Tracer[replayer.ReplayIndex][ACTION::UP])
		{
			character->Direction += VSM::Vector3(1, 0, 1);
		}
		if (replayer.Tracer[replayer.ReplayIndex][ACTION::DOWN])
		{
			character->Direction += VSM::Vector3(-1, 0, -1);
		}

		character->Direction.Normalize();

		if (replayer.Tracer[replayer.ReplayIndex][ACTION::JUMP])
		{
			EventManager::PublishImmediate("OnJump", replayer.GameObjectId);
		}

		if (replayer.Tracer[replayer.ReplayIndex][ACTION::SKIP])
		{
			replayer.IsWaiting = true;
		}
		if (replayer.Tracer[replayer.ReplayIndex][ACTION::BOX])
		{
			EventManager::PublishImmediate("OnSpawnBox", replayer.GameObjectId);
		}



		replayer.ReplayIndex++;
	}
}

void ReplayerSystem::OnTraceDirection(std::any data)
{
	auto [id, direction] = std::any_cast<std::pair<uint32_t, std::bitset<8>>>(data);

	auto replayerComponent = m_GameObjectManager->QueryComponent<Replayer>(id);

	if (!replayerComponent or !replayerComponent->IsTracing)
		return;

	// 입력된 키 값 저장
	//if (replayerComponent->Tracer.size())
		replayerComponent->Tracer[replayerComponent->TraceIndex++] = direction;

	// 모든 저장이 완료 되었을 경우 종료
	if (replayerComponent->TraceIndex >= replayerComponent->TraceSize)
	{
		replayerComponent->IsTracing = false;

		EventManager::PublishImmediate("OnFinishTracing", id);
	}
}

void ReplayerSystem::OnUndo(std::any data)
{
	auto id = std::any_cast<uint32_t>(data);

	auto replayer = m_GameObjectManager->QueryComponent<Replayer>(id);

	if (!replayer)
		return;

	ParentChildrenSystem(m_GameObjectManager).RemoveParent(id);
	//replayer->Sibling<Transform>(m_GameObjectManager)->LocalPosition = replayer->StartPosition;
	//replayer->Sibling<Transform>(m_GameObjectManager)->LocalRotation = replayer->StartRotation;

	replayer->IsReplaying = false;
	replayer->IsTracing = false;
	replayer->IsFinishReplaying = false;
	replayer->IsWaiting = false;

	std::ranges::fill(replayer->Tracer, std::bitset<8>(0));
}

void ReplayerSystem::OnStopReplay(std::any data)
{
	auto id = std::any_cast<uint32_t>(data);

	auto replayer = m_GameObjectManager->QueryComponent<Replayer>(id);

	if (!replayer)
		return;

	auto transform = replayer->Sibling<Transform>(m_GameObjectManager);
	auto character = replayer->Sibling<Character>(m_GameObjectManager);
	auto rigidbody = replayer->Sibling<RigidBody>(m_GameObjectManager);

	if (!character or !rigidbody)
		return;

	replayer->IsReplaying = false;

	rigidbody->Velocity = VSM::Vector3::Zero;
	rigidbody->Force = VSM::Vector3::Zero;

	transform->LocalPosition = replayer->StartPosition;
	transform->LocalRotation = replayer->StartRotation;

	character->IsMovable = false;
	character->Direction = VSM::Vector3::Zero;
	character->DirectionBitset = 0;
}


void ReplayerSystem::OnStartReplay(std::any data)
{
	auto id = std::any_cast<uint32_t>(data);

	auto replayer = m_GameObjectManager->QueryComponent<Replayer>(id);

	if (!replayer)
		return;

	auto transform = replayer->Sibling<Transform>(m_GameObjectManager);
	auto rigidbody = replayer->Sibling<RigidBody>(m_GameObjectManager);
	auto character = replayer->Sibling<Character>(m_GameObjectManager);
	
	if (!character)
		return;

	if (!rigidbody)
		return;

	auto parent = replayer->Sibling<Parent>(m_GameObjectManager);
	if (parent)
	{
		ParentChildrenSystem(m_GameObjectManager).RemoveParent(id);
	}

	// 투명화
	if (const auto skinned = m_GameObjectManager->QueryComponent<SkinnedMeshRenderer>(id))
	{
		skinned->Alpha = 0.7f;
	}

	// 위치 변경
	transform->LocalPosition = replayer->StartPosition;
	transform->LocalRotation = replayer->StartRotation;

	// 시작 가능하도록 변경
	replayer->IsReplaying = true;
	replayer->ReplayIndex = 0;
	replayer->IsFinishReplaying = false;

	// 캐릭터 초기화
	character->IsMovable = true;

	// 속도 초기화
	rigidbody->Velocity = VSM::Vector3::Zero;
	rigidbody->Force = VSM::Vector3::Zero;
}

void ReplayerSystem::OnStartMoving(std::any data)
{
	auto id = std::any_cast<uint32_t>(data);

	auto replayerComponent = m_GameObjectManager->QueryComponent<Replayer>(id);

	if (!replayerComponent)
		return;

	auto transformComponent = replayerComponent->Sibling<Transform>(m_GameObjectManager);

	// 초기 위치, 회전 값 저장
	replayerComponent->StartPosition = transformComponent->LocalPosition;
	replayerComponent->StartRotation = transformComponent->LocalRotation;

	// 초기화
	replayerComponent->Tracer.resize(replayerComponent->TraceSize);
	replayerComponent->ReplayIndex = 0;
	replayerComponent->TraceIndex = 0;
	replayerComponent->IsReplaying = false;
	replayerComponent->IsTracing = true;
}

void ReplayerSystem::OnSkipTracing(std::any data)
{
	auto id = std::any_cast<uint32_t>(data);

	auto replayerComponent = m_GameObjectManager->QueryComponent<Replayer>(id);

	if (!replayerComponent)
		return;

	auto iter = replayerComponent->Tracer.begin();
	iter += replayerComponent->TraceIndex;
	std::transform(
		iter,
		replayerComponent->Tracer.end(),
		iter,
		[](std::bitset<8> bitset) { return bitset.set(static_cast<uint32_t>(ACTION::SKIP)); });

	replayerComponent->IsTracing = false;
	replayerComponent->TraceIndex = replayerComponent->TraceSize;

	EventManager::ScheduleEvent("OnFinishTracing", id);
}