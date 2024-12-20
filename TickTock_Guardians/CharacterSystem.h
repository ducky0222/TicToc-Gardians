#pragma once
#include "System.h"
#include "EventListener.h"

struct RigidBody;

class CharacterSystem : public System, public IUpdatable, public IFixedUpdatable, public EventListener
{
public:
	CharacterSystem(GameObjectManager* gameObjectManager);

	void Update(float deltaTime) override;
	void FixedUpdate(float deltaTime) override;

	void OnCollisionEnter(std::any data);
	void OnCollisionExit(std::any data);

	void OnStartMoving(std::any data);
	void OnFinishTracing(std::any data);
	void OnJump(std::any data);
	void OnSpawnBox(std::any data);
	//void OnSpawnRandomStepEffect(std::any data);
	//void OnSpawnLandingEffect(std::any data);

	bool IsGround(uint32_t characterID, uint32_t groundID);
	bool IsGround(RigidBody* character);
	void OnUnlockInput(std::any data);
	void OnReady(std::any data);			// 캐릭터 시작 준비 상태
	void OnStartReplay(std::any data);		// 캐릭터 시작
	void OnUndo(std::any data);				// 캐릭터 언두
	void SetIsSpawningFalse(std::any data);
};
