#include "pch.h"

#include "RenderSystem.h"
#include "ComponentCommon.h"
#include "Input.h"

#include "../ABGraphics/DebugRenderManager.h"
#include "../ABGraphics/RenderManager.h"
#include "../ABGraphics/MathUtil.h"

RenderSystem::RenderSystem(GameObjectManager* goManager)
	: System(goManager)
{
	// 유효성 검사용 데이터 추출
	for (auto& modelPath : std::filesystem::directory_iterator("../Resources/SkyBox"))
	{
		const std::string& fileName = modelPath.path().filename().string();
		const std::string& filePath = modelPath.path().string();
		mSkyBoxNamePaths.insert({ fileName, filePath });
	}

	
}

void RenderSystem::Render()
{
	if (Input::GetKeyDown(KEY::F1))
	{
		DebugRenderManager::Get().SetActive(true);
	}
	else if (Input::GetKeyDown(KEY::F2))
	{
		DebugRenderManager::Get().SetActive(false);
	}

	for (GeometryRenderer& renderer : ComponentItr<GeometryRenderer>())
	{
		if (renderer.Sibling<OnOff>(m_GameObjectManager)->IsOn == false)
			continue;
		const std::string& key = renderer.KeyName;

		Transform* transform = renderer.Sibling<Transform>(m_GameObjectManager);
		RenderManager::GetInstance().ApplyTransform(eModelType::Geometry, key, transform->WorldMatrix);
		RenderManager::GetInstance().ApplyAlpha(eModelType::Geometry, key, renderer.bUseAlpha, renderer.Alpha);
		RenderManager::GetInstance().Submit(eModelType::Geometry, key);
	}

	for (MeshRenderer& meshRenderer : ComponentItr<MeshRenderer>())
	{
		if (meshRenderer.Sibling<OnOff>(m_GameObjectManager)->IsOn == false)
			continue;

		const std::string& key = meshRenderer.FileName;

		Transform* transform = meshRenderer.Sibling<Transform>(m_GameObjectManager);
		RenderManager::GetInstance().ApplyTransform(eModelType::Basic, key, transform->WorldMatrix);
		RenderManager::GetInstance().ApplyTexTransform(eModelType::Basic, key, meshRenderer.TexSize, meshRenderer.TexTranslate, 0);
		RenderManager::GetInstance().ApplyAlpha(eModelType::Basic, key, meshRenderer.bUseAlpha, meshRenderer.Alpha);
		RenderManager::GetInstance().ApplyModelAnimation(key, meshRenderer.AnimName, meshRenderer.ClipName, meshRenderer.TimePos, meshRenderer.bIsRecursive);
		RenderManager::GetInstance().Submit(eModelType::Basic, key);
	}

	for (SkinnedMeshRenderer& skinnedMeshRenderer : ComponentItr<SkinnedMeshRenderer>())
	{
		if (skinnedMeshRenderer.Sibling<OnOff>(m_GameObjectManager)->IsOn == false)
			continue;
		const std::string& key = skinnedMeshRenderer.FileName;

		Transform* transform = skinnedMeshRenderer.Sibling<Transform>(m_GameObjectManager);
		RenderManager::GetInstance().ApplyTransform(eModelType::Skinned, key, transform->WorldMatrix);
		RenderManager::GetInstance().ApplyTexTransform(eModelType::Skinned, key, skinnedMeshRenderer.TexSize, skinnedMeshRenderer.TexTranslate, 0);
		RenderManager::GetInstance().ApplyAlpha(eModelType::Skinned, key, skinnedMeshRenderer.bUseAlpha, skinnedMeshRenderer.Alpha);

		AnimationController* controller = skinnedMeshRenderer.Sibling<AnimationController>(m_GameObjectManager);

		if (controller != nullptr)
		{
			AnimationClip* clip = skinnedMeshRenderer.Sibling<AnimationClip>(m_GameObjectManager);
			NeoNeoAnimationClip* nneoClip = skinnedMeshRenderer.Sibling<NeoNeoAnimationClip>(m_GameObjectManager);
			NeoAnimationClip* neoClip = skinnedMeshRenderer.Sibling<NeoAnimationClip>(m_GameObjectManager);

			if (!controller->bIsTransitionState)
			{
				if (nneoClip != nullptr)
				{
					auto find = nneoClip->ClipInfos.find(controller->CurrentFileName);

					if (find != nneoClip->ClipInfos.end())
					{
						bool bRecursive = find->second->bIsRecursive;
						RenderManager::GetInstance().ApplySkinnedModelAnimation(key, controller->CurrentFileName, controller->TimePos, bRecursive);
					}
				}
				else if (clip != nullptr)
				{
					auto find = clip->ClipInfos.find(controller->CurrentFileName);

					if (find != clip->ClipInfos.end())
					{
						bool bRecursive = *find->second.bIsRecursive;
						RenderManager::GetInstance().ApplySkinnedModelAnimation(key, controller->CurrentFileName, controller->TimePos, bRecursive);
					}
				}

				else if (neoClip != nullptr)
				{
					auto find = neoClip->ClipInfos.find(controller->CurrentFileName);

					if (find != neoClip->ClipInfos.end())
					{
						bool bRecursive = find->second->bIsRecursive;
						RenderManager::GetInstance().ApplySkinnedModelAnimation(key, controller->CurrentFileName, controller->TimePos, bRecursive);
					}
				}
			}
			else
			{
				if (nneoClip != nullptr)
				{
					auto find = nneoClip->ClipInfos.find(controller->CurrentFileName);
					auto belforeFind = nneoClip->ClipInfos.find(controller->BeforeFineName);

					if (find != nneoClip->ClipInfos.end() && belforeFind != nneoClip->ClipInfos.end())
					{
						float weight = controller->TransitionTimePos / controller->TransitionTime;
						RenderManager::GetInstance().ApplySkinnedModelBlendAnimation(key, controller->BeforeFineName, controller->CurrentFileName, controller->TimePos, controller->TransitionTimePos, weight, belforeFind->second->bIsRecursive, find->second->bIsRecursive);
					}
				}
				else if (clip != nullptr)
				{
					auto find = clip->ClipInfos.find(controller->CurrentFileName);
					auto belforeFind = clip->ClipInfos.find(controller->BeforeFineName);

					if (find != clip->ClipInfos.end() && belforeFind != clip->ClipInfos.end())
					{
						float weight = controller->TransitionTimePos / controller->TransitionTime;
						RenderManager::GetInstance().ApplySkinnedModelBlendAnimation(key, controller->BeforeFineName, controller->CurrentFileName, controller->TimePos, controller->TransitionTimePos, weight, *(belforeFind->second.bIsRecursive), *(find->second.bIsRecursive));
					}
				}

				else if (neoClip != nullptr)
				{
					auto find = neoClip->ClipInfos.find(controller->CurrentFileName);
					auto belforeFind = neoClip->ClipInfos.find(controller->BeforeFineName);

					if (find != neoClip->ClipInfos.end() && belforeFind != neoClip->ClipInfos.end())
					{
						float weight = controller->TransitionTimePos / controller->TransitionTime;
						RenderManager::GetInstance().ApplySkinnedModelBlendAnimation(key, controller->BeforeFineName, controller->CurrentFileName, controller->TimePos, controller->TransitionTimePos, weight, belforeFind->second->bIsRecursive, find->second->bIsRecursive);
					}
				}
			}
		}

		RenderManager::GetInstance().Submit(eModelType::Skinned, key);
	}

	for (UI2DRenderer& renderer : ComponentItr<UI2DRenderer>())
	{
		if (renderer.Sibling<OnOff>(m_GameObjectManager)->IsOn == false)
			continue;
		const std::string& key = renderer.FileName;

		RenderManager::GetInstance().ApplyTransform(eModelType::UI, key, RenderManager::GetInstance().MakeUITransform(renderer.Position, renderer.Size, to_rad(renderer.Rotation)));
		RenderManager::GetInstance().ApplyTexTransform(eModelType::UI, key, renderer.TexSize, renderer.TexTranslate, 0);
		RenderManager::GetInstance().ApplyAlpha(eModelType::UI, key, renderer.bUseAlpha, renderer.Alpha);
		RenderManager::GetInstance().ApplyUILayer(key, renderer.Layer);
		RenderManager::GetInstance().Submit(eModelType::UI, key);
	}

	for (UI3DRenderer& renderer : ComponentItr<UI3DRenderer>())
	{
		if (renderer.Sibling<OnOff>(m_GameObjectManager)->IsOn == false)
			continue;
		const std::string& key = renderer.FileName;

		Transform* transform = renderer.Sibling<Transform>(m_GameObjectManager);
		RenderManager::GetInstance().ApplyUI3DLookVector(key, renderer.LookVector);
		RenderManager::GetInstance().ApplyUI3DPosition(key, transform->WorldPosition);
		RenderManager::GetInstance().ApplyUI3DSize(key, { transform->WorldScale.x, transform->WorldScale.y });
		RenderManager::GetInstance().ApplyTexTransform(eModelType::UI3D, key, renderer.TexSize, renderer.TexTranslate, 0);
		RenderManager::GetInstance().ApplyAlpha(eModelType::UI3D, key, renderer.bUseAlpha, renderer.Alpha);
		RenderManager::GetInstance().ApplyUI3DLayer(key, renderer.Layer);
		RenderManager::GetInstance().Submit(eModelType::UI3D, key);
	}

	for (CutSceneController& controller : ComponentItr<CutSceneController>())
	{
		if (controller.Sibling<OnOff>(m_GameObjectManager)->IsOn == false)
		{
			continue;
		}

		CutSceneClip* clip = controller.Sibling<CutSceneClip>(m_GameObjectManager);

		if (clip == nullptr)
		{
			continue;
		}

		RenderManager::GetInstance().SetCutSceneIndex(clip->Key, controller.FrameIndex, controller.bIsRecursive);
		RenderManager::GetInstance().Submit(eModelType::CutScene, clip->Key);
	}

	for (BackgroundRenderer& renderer : ComponentItr<BackgroundRenderer>())
	{
		if (renderer.Sibling<OnOff>(m_GameObjectManager)->IsOn == false)
		{
			continue;
		}

		RenderManager::GetInstance().Submit(eModelType::Background, renderer.FileName);
	}

	// 스카이박스 런타임 반영 시 패스 유효성 점검 하는 코드 필요함

}

void RenderSystem::Initialize()
{
	assert(ComponentItr<SkyBoxRenderer>().innerVector.size() <= 1);

	if (ComponentItr<SkyBoxRenderer>().innerVector.size() == 0)
	{
		RenderManager::GetInstance().ResetSkymap();
		RenderManager::GetInstance().ResetIBL();

		return;
	}

	for (SkyBoxRenderer& renderer : ComponentItr<SkyBoxRenderer>())
	{
		if (renderer.bUseSkyBox)
		{
			auto find = mSkyBoxNamePaths.find(renderer.SkyBoxName);

			if (find != mSkyBoxNamePaths.end())
			{
				RenderManager::GetInstance().SetSkymap(find->second);
			}
		}
		else
		{
			RenderManager::GetInstance().ResetSkymap();
		}

		if (renderer.bUseIBL)
		{
			auto diffuse = mSkyBoxNamePaths.find(renderer.DiffuseName);
			auto specular = mSkyBoxNamePaths.find(renderer.SpecularName);
			auto brdfLUT = mSkyBoxNamePaths.find(renderer.BRDF_LUTName);

			if (diffuse != mSkyBoxNamePaths.end()
				&& specular != mSkyBoxNamePaths.end()
				&& brdfLUT != mSkyBoxNamePaths.end())
			{
				RenderManager::GetInstance().SetIBL(diffuse->second, specular->second, brdfLUT->second);
			}
		}
		else
		{
			RenderManager::GetInstance().ResetIBL();
		}
	}
}

void RenderSystem::Start(uint32_t gameObjectId)
{

}
void RenderSystem::Finish(uint32_t gameObjectId)
{

}
void RenderSystem::Finalize()
{

}
