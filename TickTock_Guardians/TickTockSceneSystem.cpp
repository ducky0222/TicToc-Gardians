#include "pch.h"
#include "TickTockSceneSystem.h"
#include "TickTockSceneCommon.h"
#include "TickTockComponentCommon.h"

#include "ComponentCommon.h"
#include "Input.h"

TickTockSceneSystem::TickTockSceneSystem(GameObjectManager* gameObjectManager)
	: SceneSystem(gameObjectManager)
{
	mScenes.insert({ eSceneState::Title, std::make_shared<TitleScene>(m_GameObjectManager) });

	mScenes.insert({ eSceneState::LevelSelect, std::make_shared<LevelSelectScene>(m_GameObjectManager) });

	mScenes.insert({ eSceneState::CharacterSelect, std::make_shared<CharacterSelect>(m_GameObjectManager) });

	// stage
	mScenes.insert({ eSceneState::MapSearchScene, std::make_shared<MapSearchScene>(m_GameObjectManager) });
	mScenes.insert({ eSceneState::FirstPlayer, std::make_shared<FirstPlayer>(m_GameObjectManager) });
	mScenes.insert({ eSceneState::SecondPlayer, std::make_shared<SecondPlayer>(m_GameObjectManager) });
	mScenes.insert({ eSceneState::ThirdPlayer, std::make_shared<ThirdPlayer>(m_GameObjectManager) });
	mScenes.insert({ eSceneState::CheckMissionClear, std::make_shared<CheckMissionClear>(m_GameObjectManager) });
	mScenes.insert({ eSceneState::StageOver, std::make_shared<StageOver>(m_GameObjectManager) });


	//// tutorial
	mScenes.insert({ eSceneState::TutorialPlayer, std::make_shared<TutorialPlayer>(m_GameObjectManager) });
	mScenes.insert({ eSceneState::CheckTutorialClear, std::make_shared<CheckTutorialClear>(m_GameObjectManager) });

	EventManager::Subscribe("OnMapSearchSceneTransition", EventListener::CreateListenerInfo(&TickTockSceneSystem::OnMapSearchSceneTransition));
}

void TickTockSceneSystem::Initialize()
{
	Finalize();

	for (SceneComponent& scene : ComponentItr<SceneComponent>())
	{
		if (scene.SceneName.find("Title") != std::string::npos)
		{
			mCurrentScene = mScenes.find(eSceneState::Title)->second;
			mCurrentScene->Enter();
		}
		else if (scene.SceneName.find("LevelSelect") != std::string::npos)
		{
			mCurrentScene = mScenes.find(eSceneState::LevelSelect)->second;
			mCurrentScene->Enter();
		}
		else if (scene.SceneName.find("Stage") != std::string::npos)
		{
			if (scene.SceneName == "NormalStage01.scene" || 
				scene.SceneName == "NormalStage02.scene" || 
				scene.SceneName == "NormalStage03.scene"|| scene.SceneName == "TutorialStage01.scene")
			{
				mCurrentScene = mScenes.find(eSceneState::TutorialPlayer)->second;
				mCurrentScene->Enter();
			}
			else
			{
				if (IsIWantGoToSearchScene)
				{
					mCurrentScene = mScenes.find(eSceneState::MapSearchScene)->second;
					mCurrentScene->Enter();
					IsIWantGoToSearchScene = false;
				}
				else
				{
					mCurrentScene = mScenes.find(eSceneState::FirstPlayer)->second;
					mCurrentScene->Enter();
				}
			}
		}
		else if (scene.SceneName.find("CharacterSelect") != std::string::npos)
		{
			mCurrentScene = mScenes.find(eSceneState::CharacterSelect)->second;
			mCurrentScene->Enter();
		}
		
	}
}

void TickTockSceneSystem::Start(uint32_t gameObjectId)
{
}

void TickTockSceneSystem::Finish(uint32_t gameObjectId)
{
}

void TickTockSceneSystem::Finalize()
{
	if (mCurrentScene != nullptr)
	{
		mCurrentScene->Exit();
	}

	mCurrentScene = nullptr;
}

void TickTockSceneSystem::Update(float deltaTime)
{
	if (mCurrentScene != nullptr)
	{
		mCurrentScene->Update(deltaTime);

		eSceneState next = mCurrentScene->HandleTransition();

		if (next != mCurrentScene->GetGameState())
		{
			auto find = mScenes.find(next);
			assert(find != mScenes.end());

			mCurrentScene->Exit();
			mCurrentScene = (find->second);
			mCurrentScene->Enter();
		}
	}
}

void TickTockSceneSystem::OnMapSearchSceneTransition(std::any data)
{
	IsIWantGoToSearchScene = true;
	EventManager::PublishImmediate("OnBindSceneName", data);
	EventManager::ScheduleEvent("OnSceneTransition", data);

}

