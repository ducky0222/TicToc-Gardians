#include "pch.h"
#include "TickTockGuardians.h"
#include "../ABGraphics/RenderManager.h"

/// Manager classes
#include "SystemManager.h"
#include "GameObjectManager.h"
#include "EventManager.h"
#include "TimeManager.h"
#include "Input.h"
#include "../SoundEngine/ISoundEngine.h"

// Component
#include "TickTockComponentCommon.h"

/// System classes 
#include "TickTockSystemCommon.h"
#include "DimensionCrackSystem.h"
#include "LandingPointSystem.h"
#include "InGameManagerSystem.h"
#include "TransformSystem.h"
#include "../SoundEngine/ISoundEngine.h"
#include "ButtonSystem.h"
#include "CharacterBarrierSystem.h"

#pragma comment(lib, "../lib/PhysXCommon_64.lib")
#pragma comment(lib, "../lib/PhysXCooking_64.lib")
#pragma comment(lib, "../lib/PhysXFoundation_64.lib")

TickTockGuardians::TickTockGuardians(HINSTANCE hInst, std::string_view title, int width, int height, bool bImGuiInit)
	: GameProcess(hInst, title, width, height, bImGuiInit)
{
	// TODO : 나중에 모델 전부 가져오는 코드 추가할것.
	//RenderManager::GetInstance().CreateSkinnedModel()
	RenderManager::GetInstance().CreateModel("TileA", "../Resources/Model/TileA.fbx");
	RenderManager::GetInstance().CreateModel("TileB", "../Resources/Model/TileB.fbx");
	RenderManager::GetInstance().CreateModel("TileC", "../Resources/Model/TileC.fbx");
	RenderManager::GetInstance().CreateModel("TileD", "../Resources/Model/TileD.fbx");
	RenderManager::GetInstance().CreateModel("TileE", "../Resources/Model/TileE.fbx");
	RenderManager::GetInstance().CreateModel("TileF", "../Resources/Model/TileF.fbx");
	RenderManager::GetInstance().CreateModel("BeaverBox", "../Resources/StaticModel/Obj_Box_Beaver.fbx");
	RenderManager::GetInstance().CreateModel("BoxGenerator", "../Resources/StaticModel/Obj_Box_BoxGenaratorBone.fbx");
	
	loadResource(eModelType::Basic, "../Resources/StaticModel");
	loadResource(eModelType::Skinned, "../Resources/SkinnedModel");
	loadResource(eModelType::UI, "../Resources/UI2D");
	loadResource(eModelType::UI3D, "../Resources/UI3D");
 	loadResource(eModelType::UI3D, "../Resources/SpriteEffect");
 	loadResource(eModelType::Animation, "../Resources/Animation");
 	loadResource(eModelType::Background, "../Resources/Background");

	m_SystemManager->AddSystem<BoxSystem>();
	m_SystemManager->AddSystem<ButtonSystem>();
	m_SystemManager->AddSystem<CharacterSystem>();
	m_SystemManager->AddSystem<ReplayerSystem>();
	m_SystemManager->AddSystem<TickTockAnimationSystem>();
	m_SystemManager->AddSystem<TickTockSceneSystem>();
	//m_SystemManager->AddSystem<SceneSystem>();
	m_SystemManager->AddSystem<ConveyorBeltSystem>();
	m_SystemManager->AddSystem<LiftSystem>();
	m_SystemManager->AddSystem<BoxGeneratorSystem>();

	// test system
	m_SystemManager->AddSystem<MousePickingSystem>();
	m_SystemManager->AddSystem<DimensionCrackSystem>();
	m_SystemManager->AddSystem<LandingPointSystem>();

	m_SystemManager->AddSystem<InGameManagerSystem>();
	m_SystemManager->AddSystem<CharacterBarrierSystem>();

	EventManager::Subscribe("OnPause", CreateListenerInfo(&TickTockGuardians::OnPause));
	EventManager::Subscribe("OnResume", CreateListenerInfo(&TickTockGuardians::OnResume));
	EventManager::Subscribe("OnPlaySound", CreateListenerInfo(&TickTockGuardians::OnPlaySound));


	namespace fs = std::filesystem;

	// BGM 탐색
	for (const auto& entry : fs::recursive_directory_iterator(std::filesystem::path{"../Resources/Sound/BGM/"})) {
		if (entry.is_regular_file() && entry.path().extension() == ".wav") {
			auto fileName = entry.path().string();
			auto fileKey = entry.path().filename().replace_extension().string();
			Sound::GetInstance()->Load2DSound(fileName, fileKey, ISoundEngine::SoundType::BGM);
		}
	}

	// EFFECT 탐색
	for (const auto& entry : fs::recursive_directory_iterator(std::filesystem::path{ "../Resources/Sound/EFFECT/" })) {
		if (entry.is_regular_file() && entry.path().extension() == ".wav") {
			auto fileName = entry.path().string();
			auto fileKey = entry.path().filename().replace_extension().string();
			Sound::GetInstance()->Load2DSound(fileName, fileKey, ISoundEngine::SoundType::EFFECT);
		}
	}
}

TickTockGuardians::~TickTockGuardians()
{
}

void TickTockGuardians::Update()
{
	if (m_IsPause)
	{
		m_Time->Update();
		m_DeltaTime = m_Time->GetDeltaTime();

		m_Input->Update();


		TransformSystem(m_GameObjects.get()).Update(m_DeltaTime);
		MousePickingSystem(m_GameObjects.get()).Update(m_DeltaTime);
		InGameManagerSystem(m_GameObjects.get()).Update(m_DeltaTime);
		TickTockSceneSystem(m_GameObjects.get()).Update(m_DeltaTime);

		EventManager::Update(m_DeltaTime);
	}
	else
	{
		GameProcess::Update();
	}
}

void TickTockGuardians::loadResource(eModelType modelType, std::filesystem::path searchFolder)
{
	if (modelType == eModelType::Geometry)
	{
		// 지오메트리 모델은 받지 않음
		assert(modelType != eModelType::Geometry);
		throw;
	}

	for (auto& modelPath : std::filesystem::directory_iterator(searchFolder))
	{
		const std::string& fileName = modelPath.path().filename().string();
		const std::string& filePath = modelPath.path().string();
		RenderManager::GetInstance().Create(modelType, fileName, filePath);
	}
}

void TickTockGuardians::OnPause(std::any data)
{
	m_IsPause = true;
}

void TickTockGuardians::OnResume(std::any data)
{
	m_IsPause = false;
}

void TickTockGuardians::OnPlaySound(std::any data)
{
	auto soundKey = std::any_cast<std::string>(data);

	Sound::GetInstance()->Play(0, soundKey);
}
