#pragma once
#include <random>
#include <memory>

#include "System.h"
#include "EventListener.h"
#include "AnimationController.h"
#include "SpriteEffect.h"
#include "NeoNeoAnimationClip.h"

class AnimationSystem : public System, public IUpdatable, public IStartable, public EventListener
{
public:
	struct RegisterTransitionData
	{
		std::string AnimName;
		std::vector<std::pair<std::string, std::function<bool(std::any)>>> Transitions;
	};

	struct TransitionData
	{
		// inParam
		std::string AnimName;

		// outParam
		std::string* OutNextAnimName{ nullptr };
		bool* OutbIsTransition{ nullptr };
		std::any Data;
		// 여기에 게임에 따른 트랜지션 데이터를 받아서 처리한다? 아니면 그냥 여기에 콜백함수 등록하게 해서 처리하는 것도 괜찮을듯?
	};

	struct SpriteEffectCreateData
	{
		Vector3 Position;
		Vector3 Offset;
		std::string EffectPrefabName;
		float EffectDuration;
		// std::string SoundName;
	};

	struct SpriteEffectDeleteData
	{
		std::string uuid; // 스프라이트 이펙트 삭제를 제한적으로 하기 위한 변수
		unsigned int DeleteId; // 삭제할 엔티티 Id
	};

public:
	AnimationSystem(GameObjectManager* gameObjectManager);

	void Update(float deltaTime) override;

	virtual void Initialize();
	virtual void Start(uint32_t gameObjectId) {}
	virtual void Finish(uint32_t gameObjectId) {}
	virtual void Finalize() {}

	void OnRegisterTransition(std::any data);
	void OnTransition(std::any data);
	void OnSpriteEffectCreate(std::any data);
	void OnSpriteEffectDelete(std::any data);

protected:
	// 각 애니메이션 시스템은 상속 받아서 요부분을 재정의한다.
	virtual bool handleTransition(AnimationController& controller);

private:
	void registerEvent(std::string aniName, std::function<void(std::any)> func);
	void registerEventAll(std::function<void(std::any)> func);

	void initInfos();
	void processSkinningAnimation(float deltaTime);
	void processUIAnimation(float deltaTime);
	void processTransformAnimation(float deltaTime);
	void processCutScene(float deltaTime);

	void processSpriteAnimation(float deltaTime);
	void processUVAnimation(float deltaTime);
	void processAlphaAnimation(float deltaTime);
	void processTimerController(float deltaTiem);

	void createSpriteEffect(AnimationSystem::SpriteEffectCreateData SECData, const SpriteEffect::SpriteEffectInfo& spriteInfo);
	void handleTransitionState(AnimationController& controller, bool* outEnterTransition, bool* outbExitTransition, float deltaTime);
	void handleOnTranstionState(AnimationController& controller, bool* outOnFrameEvent, float deltatime, ClipInformation* outClipInfo);

private:
	std::set<std::string> mAnimNames;
	std::set<std::string> mPrefabNames;
	std::map<std::string, std::string> mCutSceneNamePaths;
	std::map<std::string, std::function<void(std::any)>> mTransitionCallbacks;
	std::mt19937 mRandomGen;
	std::uniform_real_distribution<float> mFloatUniformDistribution;
	const float DISTRIBUTION_HALF_RANGE = 1234567890;
};

