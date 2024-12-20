#include "pch.h"
#include "CameraSystem.h"
#include "CameraComponent.h"
#include "../ABGraphics/RenderManager.h"
#include "Transform.h"
#include "OnOff.h"


void CameraSystem::Update(float deltaTime)
{
	for (CameraComponent& camera : ComponentItr<CameraComponent>())
	{
		if(!camera.Sibling<OnOff>(m_GameObjectManager)->IsOn) continue;

		auto renderCam = RenderManager::GetInstance().GetCamera();
		if (camera.Orthographic)
		{
			renderCam->SetLensOrtho(camera.OrthoSize, camera.OrthoSize / camera.Aspect, camera.NearClipPlane, camera.FarClipPlane);
		}
		else
		{
			renderCam->SetLens(camera.FieldOfView, camera.Aspect, camera.NearClipPlane, camera.FarClipPlane);
		}

		auto transform = camera.Sibling<Transform>(m_GameObjectManager);
		renderCam->SetPosition(transform->WorldPosition);
		renderCam->LookAt(transform->WorldPosition, transform->WorldPosition + transform->WorldMatrix.Backward(), transform->WorldMatrix.Up());
	}
}

