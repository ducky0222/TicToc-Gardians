#include "pch.h"

#include "AnimationSystem.h"
#include "ComponentCommon.h"
#include "EventManager.h"
#include "ISoundEngine.h"
#include "SerializeSystem.h"

#include "../ABGraphics/RenderManager.h"

AnimationSystem::AnimationSystem(GameObjectManager* gameObjectManager)
	: System(gameObjectManager)
{
	// 이벤트 등록
	EventManager::Subscribe("OnRegisterTransition", CreateListenerInfo(&AnimationSystem::OnRegisterTransition));
	EventManager::Subscribe("OnTransition", CreateListenerInfo(&AnimationSystem::OnTransition));
	EventManager::Subscribe("OnSpriteEffectCreate", CreateListenerInfo(&AnimationSystem::OnSpriteEffectCreate));
	EventManager::Subscribe("OnSpriteEffectDelete", CreateListenerInfo(&AnimationSystem::OnSpriteEffectDelete));

	// 유효성 검사용 데이터 추출
	for (auto& modelPath : std::filesystem::directory_iterator("../Resources/Animation"))
	{
		const std::string& fileName = modelPath.path().filename().string();
		mAnimNames.insert(fileName);
	}
	for (auto& prefabPath : std::filesystem::directory_iterator("../Data/Prefab/"))
	{
		const std::string& fileName = prefabPath.path().filename().string();
		mPrefabNames.insert(fileName);
	}
	for (auto& prefabPath : std::filesystem::directory_iterator("../Resources/CutScene/"))
	{
		const std::string& fileName = prefabPath.path().filename().string();
		const std::string& filePath = prefabPath.path().string();
		mCutSceneNamePaths.insert({ fileName, filePath });
	}

	std::random_device rd;
	mRandomGen = std::mt19937(rd());
	mFloatUniformDistribution = std::uniform_real_distribution<float>(-DISTRIBUTION_HALF_RANGE, DISTRIBUTION_HALF_RANGE);

	// 예시용 트랜지션 이벤트 등록, 나중에 인게임으로 옮겨야 함
	//{
	//	// beaver_Idle
	//	RegisterTransitionData registerData;
	//	registerData.AnimName = "beaver_idle.fbx";
	//	registerData.Transitions.reserve(4);
	//	registerData.Transitions.push_back({ "cat_jump_2.fbx",  [](std::any data) { return GetAsyncKeyState('W') & 0x8000; } });
	//	registerData.Transitions.push_back({ "cat_jump_1.fbx",  [](std::any data) { return GetAsyncKeyState('S') & 0x8000; } });
	//	EventManager::PublishImmediate("OnRegisterTransition", registerData);
	//}
	//{
	//	// beaver_run
	//	RegisterTransitionData registerData;
	//	registerData.AnimName = "cat_jump_2.fbx";
	//	registerData.Transitions.reserve(4);
	//	registerData.Transitions.push_back({ "beaver_idle.fbx",  [](std::any data) { return GetAsyncKeyState('E') & 0x8000; } });
	//	registerData.Transitions.push_back({ "cat_jump_1.fbx",  [](std::any data) { return GetAsyncKeyState('D') & 0x8000; } });
	//	EventManager::PublishImmediate("OnRegisterTransition", registerData);
	//}
	//{
	//	// beaver_jump
	//	RegisterTransitionData registerData;
	//	registerData.AnimName = "cat_jump_1.fbx";
	//	registerData.Transitions.reserve(4);
	//	registerData.Transitions.push_back({ "beaver_idle.fbx",  [](std::any data) { return GetAsyncKeyState('R') & 0x8000; } });
	//	registerData.Transitions.push_back({ "cat_jump_2.fbx",  [](std::any data) { return GetAsyncKeyState('F') & 0x8000; } });
	//	EventManager::PublishImmediate("OnRegisterTransition", registerData);
	//}
}

void AnimationSystem::Update(float deltaTime)
{
	processSkinningAnimation(deltaTime);
	processUIAnimation(deltaTime);
	processTransformAnimation(deltaTime);
	processCutScene(deltaTime);
	processSpriteAnimation(deltaTime);
	processTimerController(deltaTime);
}

void AnimationSystem::Initialize()
{
	initInfos();
}

void AnimationSystem::OnRegisterTransition(std::any data)
{
	RegisterTransitionData registerData = std::any_cast<RegisterTransitionData>(data);

	auto onTranstion = [this, registerData](std::any any)
	{
		TransitionData transitionData = std::any_cast<TransitionData>(any);

		for (auto& transition : registerData.Transitions)
		{
			if (transition.second(transitionData.Data))
			{
				*transitionData.OutNextAnimName = transition.first;
				assert(mAnimNames.find(*transitionData.OutNextAnimName) != mAnimNames.end());
				*transitionData.OutbIsTransition = true;

				break;
			}
		}
	};

	registerEvent(registerData.AnimName, onTranstion);
}

void AnimationSystem::OnTransition(std::any data)
{
	TransitionData transtionData = std::any_cast<TransitionData>(data);
	auto find = mTransitionCallbacks.find(transtionData.AnimName);

	if (find != mTransitionCallbacks.end())
	{
		find->second(transtionData);
	}
}
void AnimationSystem::OnSpriteEffectCreate(std::any data)
{
	SpriteEffectCreateData SECdata = std::any_cast<SpriteEffectCreateData>(data);
	SerializeSystem sys{ m_GameObjectManager };

	auto find = mPrefabNames.find(SECdata.EffectPrefabName);

	if (find != mPrefabNames.end())
	{
		SpriteEffectDeleteData SEDData;
		SEDData.uuid = typeid(this).name();
		SEDData.DeleteId = sys.Instantiate("../Data/Prefab/" + SECdata.EffectPrefabName, SECdata.Position + SECdata.Offset, Quaternion::Identity);

		assert(SECdata.EffectDuration >= 0.f);
		EventManager::ScheduleEvent("OnSpriteEffectDelete", SEDData, SECdata.EffectDuration);
	}
}

void AnimationSystem::OnSpriteEffectDelete(std::any data)
{
	SpriteEffectDeleteData SEDData = std::any_cast<SpriteEffectDeleteData>(data);

	if (SEDData.uuid == typeid(this).name())
	{
		EventManager::OnDestroy(SEDData.DeleteId);
	}
}

bool AnimationSystem::handleTransition(AnimationController& controller)
{
	std::string outNextAnimName = "";
	bool outbIsTransition = false;

	TransitionData transitionData;
	transitionData.AnimName = controller.CurrentFileName;
	transitionData.OutNextAnimName = &outNextAnimName;
	transitionData.OutbIsTransition = &outbIsTransition;

	EventManager::PublishImmediate("OnTransition", transitionData);

	// 트랜지션 enter
	if (outbIsTransition)
	{
		controller.bIsTransitionState = true;
		controller.BeforeFineName = controller.CurrentFileName;
		controller.CurrentFileName = outNextAnimName;
		return true;
	}

	return false;
}

void AnimationSystem::registerEvent(std::string animName, std::function<void(std::any)> func)
{
	auto find = mAnimNames.find(animName);

	if (find != mAnimNames.end())
	{
		mTransitionCallbacks.insert({ animName, func });
	}
}

void AnimationSystem::registerEventAll(std::function<void(std::any)> func)
{
	for (const auto& aninName : mAnimNames)
	{
		mTransitionCallbacks.insert({ aninName, func });
	}
}

void AnimationSystem::initInfos()
{
	for (AnimationClip& clip : ComponentItr<AnimationClip>())
	{
		if (!clip.bIsInit)
		{
			clip.bIsInit = true;

			clip.ClipInfos.insert({ clip.FileName0,{ &clip.SpriteEffectKey0, &clip.FrameInterval0, &clip.bIsRecursive0 } });
			clip.ClipInfos.insert({ clip.FileName1,{ &clip.SpriteEffectKey1, &clip.FrameInterval1, &clip.bIsRecursive1 } });
			clip.ClipInfos.insert({ clip.FileName2,{ &clip.SpriteEffectKey2, &clip.FrameInterval2, &clip.bIsRecursive2 } });
			clip.ClipInfos.insert({ clip.FileName3,{ &clip.SpriteEffectKey3, &clip.FrameInterval3, &clip.bIsRecursive3 } });
			clip.ClipInfos.insert({ clip.FileName4,{ &clip.SpriteEffectKey4, &clip.FrameInterval4, &clip.bIsRecursive4 } });
			clip.ClipInfos.insert({ clip.FileName5,{ &clip.SpriteEffectKey5, &clip.FrameInterval5, &clip.bIsRecursive5 } });
		}
	}
	for (NeoNeoAnimationClip& clip : ComponentItr<NeoNeoAnimationClip>())
	{
		ClipInformation* clipPtr = &clip.Clip0;

		for (size_t i = 0; i < 8; ++i)
		{
			clip.ClipInfos.insert({ clipPtr->FileName, clipPtr++ });
		}
	}

	for (NeoAnimationClip& clip : ComponentItr<NeoAnimationClip>())
	{
		ClipInformation* clipPtr = &clip.Clip0;

		for (size_t i = 0; i < 8; ++i)
		{
			clip.ClipInfos.insert({ clipPtr->FileName, clipPtr++ });
		}
	}

	for (SpriteEffect& effect : ComponentItr<SpriteEffect>())
	{
		if (!effect.bIsInit)
		{
			effect.SpriteEffectInfos.insert({ effect.KeyName0, { &effect.EffectPrefabName0, &effect.Offset0, &effect.RandomOffset0, &effect.Count0, &effect.EffectDuration0, &effect.RandomEffectDuration0, &effect.RandomEffectStart0 } });
			effect.SpriteEffectInfos.insert({ effect.KeyName1, { &effect.EffectPrefabName1, &effect.Offset1, &effect.RandomOffset1, &effect.Count1, &effect.EffectDuration1, &effect.RandomEffectDuration1, &effect.RandomEffectStart1 } });
			effect.SpriteEffectInfos.insert({ effect.KeyName2, { &effect.EffectPrefabName2, &effect.Offset2, &effect.RandomOffset2, &effect.Count2, &effect.EffectDuration2, &effect.RandomEffectDuration2, &effect.RandomEffectStart2 } });
		}
	}

	for (CutSceneClip& clip : ComponentItr<CutSceneClip>())
	{
		std::vector<std::string> filePaths;
		filePaths.reserve(clip.FrameCount);

		if (std::filesystem::directory_entry{ clip.DirectoryPath }.exists())
		{
			for (auto& curSceneTexture : std::filesystem::directory_iterator{ clip.DirectoryPath })
			{
				const std::string& filePath = curSceneTexture.path().string();

				filePaths.push_back(filePath);
			}
		}

		RenderManager::GetInstance().CreateCutSceneModel(clip.Key, filePaths);
	}
}

void AnimationSystem::processSkinningAnimation(float deltaTime)
{
	// 스키닝 애니메이션 처리
	for (AnimationController& controller : ComponentItr<AnimationController>())
	{
		if (controller.Sibling<OnOff>(m_GameObjectManager)->IsOn == false)
		{
			continue;
		}

		// 초기화 관리
		if (!controller.bIsInit)
		{
			controller.bIsInit = true;
			controller.CurrentFileName = controller.EntryFileName;
		}

		// 애니메이션 시간 누적
		if (controller.bIsActive)
		{
			controller.TimePos += deltaTime * controller.AnimSpeedWeight;
		}

		bool checkClipEnterEvent{ false };
		bool checkClipExitEvent{ false };
		bool checkClipOnFrameEvent{ false };
		ClipInformation clipInfo;

		handleTransitionState(controller, &checkClipEnterEvent, &checkClipExitEvent, deltaTime);
		handleOnTranstionState(controller, &checkClipOnFrameEvent, deltaTime, &clipInfo);


		if (clipInfo.SoundKey == "SFX_Footstep")
		{
			// 난수 생성기 초기화
			static std::random_device rd;  // 난수 생성을 위한 하드웨어 장치
			static std::mt19937 gen(rd()); // 난수 생성 알고리즘 (Mersenne Twister)
			static std::uniform_int_distribution<> distrib(1, 8); // 범위 1~8의 균일 분포

			// 난수 생성 및 SoundKey 업데이트
			int randomNum = distrib(gen); // 1~8 사이의 난수 생성
			clipInfo.SoundKey = "SFX_Footstep-00" + std::to_string(randomNum);
		}

		auto playSound = [&](auto clip)
		{
			if ((checkClipEnterEvent && clip->bOnFrameEnter)
				|| (checkClipOnFrameEvent && clip->bOnFrame)
				|| (checkClipExitEvent && clip->bOnFrameExit))
			{
				Sound::GetInstance()->Play(controller.GameObjectId, clipInfo.SoundKey);
			}
		};

		auto handleSpriteEffect = [&](auto clip)
		{
			Transform* transform = controller.Sibling<Transform>(m_GameObjectManager);
			SpriteEffect* spriteEffect = controller.Sibling<SpriteEffect>(m_GameObjectManager);

			if (spriteEffect != nullptr)
			{
				auto spriteEffectFind = spriteEffect->SpriteEffectInfos.find(clipInfo.SpriteEffectKey);

				if (spriteEffectFind != spriteEffect->SpriteEffectInfos.end())
				{
					SpriteEffectCreateData SECData;
					SECData.Position = transform->WorldPosition;
					SECData.EffectPrefabName = *spriteEffectFind->second.EffectPrefabName;

					// 스프라이트 이펙트 이벤트 처리
					if ((checkClipEnterEvent && clip->bOnFrameEnter)
						|| (checkClipOnFrameEvent && clip->bOnFrame)
						|| (checkClipExitEvent && clip->bOnFrameExit))
					{
						createSpriteEffect(SECData, spriteEffectFind->second);
					}
				}
			}
		};

		auto clip = controller.Sibling<AnimationClip>(m_GameObjectManager);
		if (clip != nullptr)
		{
			playSound(clip);
			handleSpriteEffect(clip);
		}

		auto neoClip = controller.Sibling<NeoAnimationClip>(m_GameObjectManager);
		if (neoClip != nullptr)
		{
			playSound(neoClip);
			handleSpriteEffect(neoClip);
		}

		auto neoNeoClip = controller.Sibling<NeoNeoAnimationClip>(m_GameObjectManager);
		if (neoNeoClip != nullptr)
		{
			playSound(neoNeoClip);
			handleSpriteEffect(neoNeoClip);
		}
	}
}

void AnimationSystem::processUIAnimation(float deltaTime)
{
	// UI 애니메이션 처리
	for (UIController& controller : ComponentItr<UIController>())
	{
		if (controller.Sibling<OnOff>(m_GameObjectManager)->IsOn == false)
		{
			continue;
		}

		if (controller.bIsReset)
		{
			controller.bIsReset = false;
			controller.TimePos = 0.f;
		}

		if (controller.bIsActive)
		{
			controller.TimePos += deltaTime;
		}

		VSM::Vector2 texTranslate = { 0, 0 };
		VSM::Vector2 texSize = controller.ClipSize / controller.SpriteSize;

		if (controller.bUseSpriteAnimation)
		{
			unsigned int index = static_cast<unsigned int>(controller.TimePos / controller.FrameInterval);
			texTranslate = (controller.SpriteAnimInterval * (float)index) / controller.SpriteSize;
		}

		if (controller.bUseUVAnimation)
		{
			texTranslate += controller.UVSpeed * controller.TimePos;
		}

		float alpha = controller.InitAlpha;
		if (controller.bUseAlphaAnimation)
		{
			alpha += controller.AlphaSpeed * controller.TimePos;
		}

		texTranslate.x = static_cast<float>(fmod(texTranslate.x, 1));
		texTranslate.y = static_cast<float>(fmod(texTranslate.y, 1));
		alpha = static_cast<float>(fmax(alpha, 0));
		alpha = static_cast<float>(fmin(alpha, 1));

		UI2DRenderer* ui2D = controller.Sibling<UI2DRenderer>(m_GameObjectManager);
		UI3DRenderer* ui3D = controller.Sibling<UI3DRenderer>(m_GameObjectManager);

		if (ui2D != nullptr)
		{
			ui2D->TexSize = texSize;
			ui2D->TexTranslate = texTranslate;
			ui2D->Alpha = alpha;
		}
		if (ui3D != nullptr)
		{
			ui3D->TexSize = texSize;
			ui3D->TexTranslate = texTranslate;
			ui3D->Alpha = alpha;
		}

		MeshRenderer* meshRenderer = controller.Sibling<MeshRenderer>(m_GameObjectManager);

		if (meshRenderer != nullptr)
		{
			meshRenderer->TexSize = texSize;
			meshRenderer->TexTranslate = texTranslate;
			meshRenderer->Alpha = alpha;
		}
	}
}

void AnimationSystem::processTransformAnimation(float deltaTime)
{
	for (TransformAnim2D& aniComp : ComponentItr<TransformAnim2D>())
	{
		if (aniComp.Sibling<OnOff>(m_GameObjectManager)->IsOn == false)
		{
			continue;
		}

		if (aniComp.bIsActive)
		{
			aniComp.TimePos += deltaTime;
		}

		if (aniComp.bIsReset)
		{
			aniComp.TimePos = 0.f;
			aniComp.bIsReset = false;
		}

		float delta = aniComp.bIsRecursive
			? fmod(aniComp.TimePos, aniComp.ProcessingTime)
			: min(aniComp.TimePos, aniComp.ProcessingTime);

		// 해당 컴포넌트는 ui2D랜더러를 위한 컴포넌트이므로 반드시 존재해야 한다.
		UI2DRenderer* ui2DRenderer = aniComp.Sibling<UI2DRenderer>(m_GameObjectManager);
		assert(ui2DRenderer != nullptr);

		float deltaRatio = delta / aniComp.ProcessingTime;
		ui2DRenderer->Size = DirectX::SimpleMath::Vector2::Lerp(aniComp.InitSize, aniComp.EndSize, deltaRatio);
		ui2DRenderer->Rotation = lerp(aniComp.InitRotation, aniComp.EndRotation, deltaRatio);
		ui2DRenderer->Position = DirectX::SimpleMath::Vector2::Lerp(aniComp.InitPosition, aniComp.EndPosition, deltaRatio);
	}

	for (TransformAnim3D& aniComp : ComponentItr<TransformAnim3D>())
	{
		if (aniComp.Sibling<OnOff>(m_GameObjectManager)->IsOn == false)
		{
			continue;
		}

		if (aniComp.bIsActive)
		{
			aniComp.TimePos += deltaTime;
		}

		if (aniComp.bIsReset)
		{
			aniComp.TimePos = 0.f;
			aniComp.bIsReset = false;
		}

		float delta = aniComp.bIsRecursive
			? fmod(aniComp.TimePos, aniComp.ProcessingTime)
			: min(aniComp.TimePos, aniComp.ProcessingTime);

		// 해당 컴포넌트는 ui2D랜더러를 위한 컴포넌트이므로 반드시 존재해야 한다.
		Transform* transform = aniComp.Sibling<Transform>(m_GameObjectManager);
		assert(transform != nullptr);

		float deltaRatio = delta / aniComp.ProcessingTime;
		transform->LocalScale = DirectX::SimpleMath::Vector3::Lerp(aniComp.InitSize, aniComp.EndSize, deltaRatio);
		transform->LocalRotation = DirectX::SimpleMath::Quaternion::Slerp(aniComp.InitRotation, aniComp.EndRotation, deltaRatio);
		transform->LocalPosition = DirectX::SimpleMath::Vector3::Lerp(aniComp.InitPosition, aniComp.EndPosition, deltaRatio);
	}

	for (NeoTransformAnim2D& aniComp : ComponentItr<NeoTransformAnim2D>())
	{
		if (aniComp.Sibling<OnOff>(m_GameObjectManager)->IsOn == false)
		{
			continue;
		}

		if (aniComp.bIsActive)
		{
			if (aniComp.bIsReverse)
			{
				aniComp.TimePos = min(aniComp.TimePos, aniComp.ProcessingTime);
				aniComp.TimePos -= deltaTime;
			}
			else
			{
				aniComp.TimePos = max(aniComp.TimePos, 0.f);
				aniComp.TimePos += deltaTime;
			}
		}

		if (aniComp.bIsReset)
		{
			if (aniComp.bIsReverse)
			{
				aniComp.TimePos = aniComp.ProcessingTime;
			}
			else
			{
				aniComp.TimePos = 0.f;
			}

			aniComp.bIsReset = false;
		}

		float delta = aniComp.TimePos;

		if (aniComp.bIsRecursive)
		{
			if (aniComp.bIsReverse)
			{
				delta = fmod(aniComp.TimePos, aniComp.ProcessingTime);
				if (delta < 0)
				{
					delta += aniComp.ProcessingTime;
				}
			}
			else
			{
				delta = fmod(aniComp.TimePos, aniComp.ProcessingTime);
			}
		}
		else
		{
			if (aniComp.bIsReverse)
			{
				delta = max(aniComp.TimePos, 0.f);
			}
			else
			{
				delta = min(aniComp.TimePos, aniComp.ProcessingTime);
			}
		}

		// 해당 컴포넌트는 ui2D랜더러를 위한 컴포넌트이므로 반드시 존재해야 한다.
		UI2DRenderer* ui2DRenderer = aniComp.Sibling<UI2DRenderer>(m_GameObjectManager);
		assert(ui2DRenderer != nullptr);

		float deltaRatio = delta / aniComp.ProcessingTime;
		ui2DRenderer->Size = DirectX::SimpleMath::Vector2::Lerp(aniComp.InitSize, aniComp.EndSize, deltaRatio);
		ui2DRenderer->Rotation = lerp(aniComp.InitRotation, aniComp.EndRotation, deltaRatio);
		ui2DRenderer->Position = DirectX::SimpleMath::Vector2::Lerp(aniComp.InitPosition, aniComp.EndPosition, deltaRatio);
	}

	for (NeoTransformAnim3D& aniComp : ComponentItr<NeoTransformAnim3D>())
	{
		if (aniComp.Sibling<OnOff>(m_GameObjectManager)->IsOn == false)
		{
			continue;
		}

		if (aniComp.bIsActive)
		{
			if (aniComp.bIsReverse)
			{
				aniComp.TimePos = min(aniComp.TimePos, aniComp.ProcessingTime);
				aniComp.TimePos -= deltaTime;
			}
			else
			{
				aniComp.TimePos = max(aniComp.TimePos, 0.f);
				aniComp.TimePos += deltaTime;
			}
		}

		if (aniComp.bIsReset)
		{
			if (aniComp.bIsReverse)
			{
				aniComp.TimePos = aniComp.ProcessingTime;
			}
			else
			{
				aniComp.TimePos = 0.f;
			}

			aniComp.bIsReset = false;
		}

		float delta = aniComp.TimePos;

		if (aniComp.bIsRecursive)
		{
			if (aniComp.bIsReverse)
			{
				delta = fmod(aniComp.TimePos, aniComp.ProcessingTime);
				if (delta < 0)
				{
					delta += aniComp.ProcessingTime;
				}
			}
			else
			{
				delta = fmod(aniComp.TimePos, aniComp.ProcessingTime);
			}
		}
		else
		{
			if (aniComp.bIsReverse)
			{
				delta = max(aniComp.TimePos, 0.f);
			}
			else
			{
				delta = min(aniComp.TimePos, aniComp.ProcessingTime);
			}
		}

		Transform* transform = aniComp.Sibling<Transform>(m_GameObjectManager);
		assert(transform != nullptr);

		float deltaRatio = delta / aniComp.ProcessingTime;
		transform->LocalScale = DirectX::SimpleMath::Vector3::Lerp(aniComp.InitSize, aniComp.EndSize, deltaRatio);
		transform->LocalRotation = DirectX::SimpleMath::Quaternion::Slerp(aniComp.InitRotation, aniComp.EndRotation, deltaRatio);
		transform->LocalPosition = DirectX::SimpleMath::Vector3::Lerp(aniComp.InitPosition, aniComp.EndPosition, deltaRatio);
	}
}

void AnimationSystem::processCutScene(float deltaTime)
{
	for (CutSceneController& controller : ComponentItr<CutSceneController>())
	{
		if (controller.Sibling<OnOff>(m_GameObjectManager)->IsOn == false)
		{
			continue;
		}

		if (controller.bIsActive)
		{
			controller.TimePos += deltaTime;
		}

		if (controller.bIsReset)
		{
			controller.TimePos = 0.f;
			controller.bIsReset = false;
		}

		unsigned int index = (unsigned int)(controller.TimePos / controller.FrameInterval);
		controller.FrameIndex = controller.bIsRecursive
			? index % controller.FrameCount
			: min(index, controller.FrameCount - 1);
	}
}

void AnimationSystem::processSpriteAnimation(float deltaTime)
{
	for (SpriteController& controller : ComponentItr<SpriteController>())
	{
		if (controller.Sibling<OnOff>(m_GameObjectManager)->IsOn == false)
		{
			continue;
		}

		if (controller.bIsReset)
		{
			controller.bIsReset = false;
			controller.TimePos = 0.f;
			controller.bIsFinishTraversal = false;
		}

		if (controller.bIsActive)
		{
			controller.TimePos += deltaTime;
		}

		unsigned int index = static_cast<unsigned int>(controller.TimePos / controller.FrameInterval);

		if (controller.bIsRecursive)
		{
			index %= controller.SpriteCount;
		}
		else
		{
			index = min(index, controller.SpriteCount - 1);

			if (index >= controller.SpriteCount - 1)
			{
				controller.bIsFinishTraversal = true;
			}
		}

		VSM::Vector2 texTranslate = (controller.ClipInterval * (float)index) / controller.SpriteSize;
		VSM::Vector2 texSize = controller.ClipSize / controller.SpriteSize;

		UI2DRenderer* ui2D = controller.Sibling<UI2DRenderer>(m_GameObjectManager);
		UI3DRenderer* ui3D = controller.Sibling<UI3DRenderer>(m_GameObjectManager);

		if (ui2D != nullptr)
		{
			ui2D->TexSize = texSize;
			ui2D->TexTranslate = texTranslate;
		}
		if (ui3D != nullptr)
		{
			ui3D->TexSize = texSize;
			ui3D->TexTranslate = texTranslate;
		}

		MeshRenderer* meshRenderer = controller.Sibling<MeshRenderer>(m_GameObjectManager);

		if (meshRenderer != nullptr)
		{
			meshRenderer->TexSize = texSize;
			meshRenderer->TexTranslate = texTranslate;
		}
	}
}

void AnimationSystem::processTimerController(float deltaTime)
{
	for (Timer& controller : ComponentItr<Timer>())
	{
		if (controller.Sibling<OnOff>(m_GameObjectManager)->IsOn == false)
		{
			continue;
		}

		if (controller.bIsReset)
		{
			controller.bIsReset = false;
			controller.TimePos = controller.InitTimePos;
		}

		if (controller.bIsActive)
		{
			if (!controller.bIsReverse)
			{
				controller.TimePos += deltaTime;
			}
			else
			{
				controller.TimePos -= deltaTime;
				controller.TimePos = fmax(0, controller.TimePos);
			}
		}

		int index = static_cast<unsigned int>(controller.TimePos / controller.FrameInterval);
		index %= 10;

		VSM::Vector2 texTranslate = (controller.ClipInterval * (float)index) / controller.SpriteSize;
		VSM::Vector2 texSize = controller.ClipSize / controller.SpriteSize;

		UI2DRenderer* ui2D = controller.Sibling<UI2DRenderer>(m_GameObjectManager);

		if (ui2D != nullptr)
		{
			ui2D->TexSize = texSize;
			ui2D->TexTranslate = texTranslate;
		}

		MeshRenderer* meshRenderer = controller.Sibling<MeshRenderer>(m_GameObjectManager);

		if (meshRenderer != nullptr)
		{
			meshRenderer->TexSize = texSize;
			meshRenderer->TexTranslate = texTranslate;
		}
	}
}

void AnimationSystem::processUVAnimation(float deltaTime)
{

}

void AnimationSystem::processAlphaAnimation(float deltaTime)
{

}

void AnimationSystem::createSpriteEffect(AnimationSystem::SpriteEffectCreateData SECData, const SpriteEffect::SpriteEffectInfo& spriteInfo)
{
	for (size_t i = 0; i < *(spriteInfo.Count); ++i)
	{
		float randomFloat0 = mFloatUniformDistribution(mRandomGen);
		float randomFloat1 = mFloatUniformDistribution(mRandomGen);
		float randomFloat2 = mFloatUniformDistribution(mRandomGen);
		float randomFloat3 = mFloatUniformDistribution(mRandomGen);

		float offsetX = fmod(randomFloat0, spriteInfo.RandomOffset->x * 0.5f);
		float offsetY = fmod(randomFloat1, spriteInfo.RandomOffset->y * 0.5f);
		float offsetZ = fmod(randomFloat2, spriteInfo.RandomOffset->z * 0.5f);
		float randomDuration = fmod(rand(), *spriteInfo.RandomEffectDuration * 0.5f);

		SECData.Offset = *(spriteInfo.Offset) + VSM::Vector3{ offsetX, offsetY ,offsetZ };
		SECData.EffectDuration = *spriteInfo.EffectDuration + randomDuration;

		float randomStart = fmod(randomFloat3, *spriteInfo.RandRandomEffectStart);
		randomStart = fmax(0.01f, randomStart);
		EventManager::ScheduleEvent("OnSpriteEffectCreate", SECData, randomStart);
	}
}

void AnimationSystem::handleTransitionState(AnimationController& controller, bool* outEnterTransition, bool* outbExitTransition, float deltaTime)
{
	// 트랜지션 관리
	if (controller.bUseTransition)
	{
		//트랜지션 상태일시
		if (controller.bIsTransitionState)
		{
			controller.TransitionTimePos += deltaTime;

			// 트랜지션 exit
			if (controller.TransitionTimePos > controller.TransitionTime)
			{
				controller.TimePos = controller.TransitionTimePos;
				controller.TransitionTimePos = 0.f;
				controller.bIsTransitionState = false;
				*outbExitTransition = true;
			}
		}
		// 트랜지션 상태가 아닐 시
		else
		{
			*outEnterTransition = handleTransition(controller);
		}
	}
}

void AnimationSystem::handleOnTranstionState(AnimationController& controller, bool* outOnFrameEvent, float deltatime, ClipInformation* outClipInfo)
{
	AnimationClip* clip = controller.Sibling<AnimationClip>(m_GameObjectManager);
	if (clip != nullptr)
	{
		auto currentClip = clip->ClipInfos.find(controller.CurrentFileName);

		if (currentClip != clip->ClipInfos.end())
		{
			outClipInfo->FileName = "";
			outClipInfo->SpriteEffectKey = *currentClip->second.SpriteEffectKey;
			outClipInfo->FrameInterval = *currentClip->second.FrameInterval;
			outClipInfo->bIsRecursive = *currentClip->second.bIsRecursive;
			outClipInfo->SoundKey = "";
		}
	}

	NeoAnimationClip* neoClip = controller.Sibling<NeoAnimationClip>(m_GameObjectManager);

	if (neoClip != nullptr)
	{
		auto currentClip = neoClip->ClipInfos.find(controller.CurrentFileName);

		if (currentClip != neoClip->ClipInfos.end())
		{
			outClipInfo->FileName = currentClip->second->FileName;
			outClipInfo->SpriteEffectKey = currentClip->second->SpriteEffectKey;
			outClipInfo->FrameInterval = currentClip->second->FrameInterval;
			outClipInfo->bIsRecursive = currentClip->second->bIsRecursive;
			outClipInfo->SoundKey = currentClip->second->SoundKey;
		}
	}

	NeoNeoAnimationClip* nneoClip = controller.Sibling<NeoNeoAnimationClip>(m_GameObjectManager);

	if (nneoClip != nullptr)
	{
		auto currentClip = nneoClip->ClipInfos.find(controller.CurrentFileName);

		if (currentClip != nneoClip->ClipInfos.end())
		{
			outClipInfo->FileName = currentClip->second->FileName;
			outClipInfo->SpriteEffectKey = currentClip->second->SpriteEffectKey;
			outClipInfo->FrameInterval = currentClip->second->FrameInterval;
			outClipInfo->bIsRecursive = currentClip->second->bIsRecursive;
			outClipInfo->SoundKey = currentClip->second->SoundKey;
		}
	}

	// OnClipEvent 체크
	bool bCheckOnClipEvent = false;
	{
		const auto& animations = RenderManager::GetInstance().GetAnimations();
		auto animFind = animations.find(controller.CurrentFileName);

		assert(animFind != animations.end());
		if (animFind != animations.end())
		{
			controller.CurrentFrameIndex = animFind->second->GetAnimClip(0).GetFrameIndex(controller.TimePos, outClipInfo->bIsRecursive);
		}

		if (!controller.ThrowFrameEvent)
		{
			controller.ThrowFrameEvent = bCheckOnClipEvent = controller.CurrentFrameIndex % outClipInfo->FrameInterval == 0;
		}
		else
		{
			controller.ThrowFrameEvent = controller.CurrentFrameIndex % outClipInfo->FrameInterval != 0;
		}
	}

	*outOnFrameEvent = bCheckOnClipEvent;
}
