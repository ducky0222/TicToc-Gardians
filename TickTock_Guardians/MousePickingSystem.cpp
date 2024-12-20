#include "pch.h"
#include "MousePickingSystem.h"
#include "TickTockComponentCommon.h"
#include "../ABGraphics/RenderManager.h"
#include "../ABGraphics/DebugRenderManager.h"
#include "ComponentCommon.h"
#include "Input.h"
#include "OnOff.h"
#include "CameraComponent.h"
#include "../SoundEngine/ISoundEngine.h"

MousePickingSystem::MousePickingSystem(GameObjectManager* gameObjectManager)
	: System(gameObjectManager)
{
}

void renderDebugDraw()
{

}
// 동작을 위해서 여기에 정의하지만 이 이벤트는 여기서 처리되면 안됨.
void MousePickingSystem::Update(float deltaTime)
{
	Vector2 mousePos = Input::GetMousePos();

	int screenX = mousePos.x;
	int screenY = mousePos.y;
	RenderManager::GetInstance().ConvertMousePosWindowsToViewPort(&screenX, &screenY);

	HandleMouseOverEvent(screenX, screenY);
	HandleMouseOutEvent(screenX, screenY);

	if (Input::GetKeyDown(KEY::LBTN))
	{
		//Sound::GetInstance()->Play(0, "SFX_UI_Select_Click");
		HandleMouseDownEvent(screenX, screenY);
	}
	else if (Input::GetKeyUp(KEY::LBTN))
	{
		HandleMouseUpEvent(screenX, screenY);
	}

	// debug Render
	//for (SelectCharacter& selectCharacter : ComponentItr<SelectCharacter>())
	//{
	//	if (selectCharacter.Sibling<OnOff>(m_GameObjectManager)->IsOn == false)
	//		continue;

	//	SkinnedMeshRenderer* mesh = selectCharacter.Sibling<SkinnedMeshRenderer>(m_GameObjectManager);

	//	if (mesh == nullptr)
	//	{
	//		continue;
	//	}

	//	BoundingBox boundingBox;

	//	if (RenderManager::GetInstance().GetBoundingBox(eModelType::Skinned, mesh->FileName, &boundingBox))
	//	{
	//		const Transform* transform = selectCharacter.Sibling<Transform>(m_GameObjectManager);
	//		boundingBox.Transform(boundingBox, transform->WorldMatrix);


	//		// MousePickingSystem.cpp
	//		if (selectCharacter.bIsSelected)
	//		{
	//			DebugRenderManager::Get().Submit(boundingBox);
	//			DebugRenderManager::Ring ring;
	//			ring.Pos = boundingBox.Center;
	//			ring.Pos += {0, 2.5, 0};
	//			ring.XAxis = { 5, 0, 0 };
	//			ring.YAxis = { 0, 0, 5 };
	//			ring.Color = { 0, 1, 1 ,1 };
	//			DebugRenderManager::Get().Submit(ring);
	//			ring.Pos -= {0, 7.5, 0};
	//			DebugRenderManager::Get().Submit(ring);

	//			BoundingSphere sphere;
	//			DirectX::BoundingSphere::CreateFromBoundingBox(sphere, boundingBox);
	//			DebugRenderManager::Get().Submit(sphere);
	//		}
	//		else
	//		{
	//			DebugRenderManager::Get().Submit(boundingBox, { 1, 0, 0, 1 });

	//		}
	//	}
	//}
}

void MousePickingSystem::HandleMouseDownEvent(int screenX, int screenY)
{
	//bool bIsOrtho = false;
	//for (CameraComponent& camera : ComponentItr<CameraComponent>())
	//{
	//	if (camera.Sibling<OnOff>(m_GameObjectManager)->IsOn == false)
	//		continue;
	//
	//	camera.Orthographic ? (bIsOrtho = true) : (bIsOrtho = false);
	//}

	// // 캐릭터 피킹 처리
	// DirectX::SimpleMath::Ray pickingRayInWorld = RenderManager::GetInstance().GetPickingRayInWorldViewPort(screenX, screenY, bIsOrtho);
	// 
	// for (SelectCharacter& selectCharacter : ComponentItr<SelectCharacter>())
	// {
	// 	if (selectCharacter.Sibling<OnOff>(m_GameObjectManager)->IsOn == false)
	// 		continue;
	// 
	// 	SkinnedMeshRenderer* mesh = selectCharacter.Sibling<SkinnedMeshRenderer>(m_GameObjectManager);
	// 
	// 	if (mesh == nullptr)
	// 	{
	// 		continue;
	// 	}
	// 
	// 	BoundingBox boundingBox;
	// 
	// 	if (RenderManager::GetInstance().GetBoundingBox(eModelType::Skinned, mesh->FileName, &boundingBox))
	// 	{
	// 		const Transform* transform = selectCharacter.Sibling<Transform>(m_GameObjectManager);
	// 		pickingRayInWorld.direction.Normalize();
	// 		boundingBox.Transform(boundingBox, transform->WorldMatrix);
	// 
	// 		float min;
	// 		selectCharacter.bIsClicked = pickingRayInWorld.Intersects(boundingBox, min);
	// 	}
	// }

	const Vector2& viewPortSize = RenderManager::GetInstance().GetEditorViewPortSize();
	const Vector2& uiCanvasSIze = RenderManager::GetInstance().GetUICanvasSize();
	const Vector2 uiClickPosition = Vector2(screenX, screenY) / viewPortSize * uiCanvasSIze;

	// UI 피킹 처리
	for (UI2DRenderer& ui : ComponentItr<UI2DRenderer>())
	{
		if (ui.Sibling<OnOff>(m_GameObjectManager)->IsOn == false)
			continue;

		Vector2 bottomRight = ui.Position + ui.Size;
		Button2D* button2D = ui.Sibling<Button2D>(m_GameObjectManager);
		NeoButton2D* neobutton2D = ui.Sibling<NeoButton2D>(m_GameObjectManager);

		if (button2D != nullptr)
		{
			button2D->bIsClicked = ui.Position.x < uiClickPosition.x
				&& ui.Position.y < uiClickPosition.y
				&& bottomRight.x > uiClickPosition.x
				&& bottomRight.y > uiClickPosition.y;
		}
		else if (neobutton2D != nullptr)
		{
			neobutton2D->bIsClicked = ui.Position.x < uiClickPosition.x
				&& ui.Position.y < uiClickPosition.y
				&& bottomRight.x > uiClickPosition.x
				&& bottomRight.y > uiClickPosition.y;
		}
	}
}

void MousePickingSystem::HandleMouseUpEvent(int screenX, int screenY)
{
	// bool bIsOrtho = false;
	// for (CameraComponent& camera : ComponentItr<CameraComponent>())
	// {
	// 	if (camera.Sibling<OnOff>(m_GameObjectManager)->IsOn == false)
	// 		continue;
	// 
	// 	camera.Orthographic ? (bIsOrtho = true) : (bIsOrtho = false);
	// }
	// 
	// // 처리되지 않은 데이터 중 가장 높은 데이터 식별
	// int selectIndex = -1;
	// for (SelectCharacter& selectCharacter : ComponentItr<SelectCharacter>())
	// {
	// 	if (selectCharacter.bIsSelected)
	// 	{
	// 		selectIndex = max(selectIndex, selectCharacter.SelectIndex);
	// 	}
	// }
	// 
	// // 캐릭터 피킹 처리
	// DirectX::SimpleMath::Ray pickingRayInWorld = RenderManager::GetInstance().GetPickingRayInWorldViewPort(screenX, screenY, false);
	// 
	// for (SelectCharacter& selectCharacter : ComponentItr<SelectCharacter>())
	// {
	// 	if (selectCharacter.Sibling<OnOff>(m_GameObjectManager)->IsOn == false)
	// 		continue;
	// 	SkinnedMeshRenderer* mesh = selectCharacter.Sibling<SkinnedMeshRenderer>(m_GameObjectManager);
	// 
	// 	if (mesh == nullptr)
	// 	{
	// 		continue;
	// 	}
	// 
	// 	BoundingBox boundingBox;
	// 
	// 	if (RenderManager::GetInstance().GetBoundingBox(eModelType::Skinned, mesh->FileName, &boundingBox))
	// 	{
	// 		const Transform* transform = selectCharacter.Sibling<Transform>(m_GameObjectManager);
	// 		pickingRayInWorld.direction.Normalize();
	// 		boundingBox.Transform(boundingBox, transform->WorldMatrix);
	// 
	// 		float min;
	// 
	// 		if (pickingRayInWorld.Intersects(boundingBox, min) && selectCharacter.bIsClicked)
	// 		{
	// 			if (!selectCharacter.bIsSelected)
	// 			{
	// 				selectCharacter.bIsSelected = true;
	// 				selectCharacter.SelectIndex = selectIndex + 1;
	// 			}
	// 			else if (selectCharacter.SelectIndex == selectIndex)
	// 			{
	// 				selectCharacter.bIsSelected = false;
	// 				selectCharacter.SelectIndex = -1;
	// 			}
	// 		}
	// 	}
	// 
	// 	selectCharacter.bIsClicked = false;
	// }

	const Vector2& viewPortSize = RenderManager::GetInstance().GetEditorViewPortSize();
	const Vector2& uiCanvasSIze = RenderManager::GetInstance().GetUICanvasSize();
	const Vector2 uiClickPosition = Vector2(screenX, screenY) / viewPortSize * uiCanvasSIze;

	// UI 피킹 처리
	for (UI2DRenderer& ui : ComponentItr<UI2DRenderer>())
	{
		if (ui.Sibling<OnOff>(m_GameObjectManager)->IsOn == false)
			continue;

		Vector2 bottomRight = ui.Position + ui.Size;
		Button2D* button2D = ui.Sibling<Button2D>(m_GameObjectManager);
		NeoButton2D* neobutton2D = ui.Sibling<NeoButton2D>(m_GameObjectManager);

		if (ui.Position.x < uiClickPosition.x
			&& ui.Position.y < uiClickPosition.y
			&& bottomRight.x > uiClickPosition.x
			&& bottomRight.y > uiClickPosition.y)
		{
			if (button2D != nullptr && button2D->bIsClicked)
			{
				EventManager::ScheduleEvent(button2D->ButtonEvent, button2D->UserData);
				button2D->bIsClicked = false;
			}
			else if (neobutton2D != nullptr && neobutton2D->bIsClicked)
			{
				EventManager::ScheduleEvent(neobutton2D->ButtonClickEvent, neobutton2D->ClickUserData);
				neobutton2D->bIsClicked = false;
			}

		}
	}
}

void MousePickingSystem::HandleMouseOverEvent(int screenX, int screenY)
{
	const Vector2& viewPortSize = RenderManager::GetInstance().GetEditorViewPortSize();
	const Vector2& uiCanvasSIze = RenderManager::GetInstance().GetUICanvasSize();
	const Vector2 uiClickPosition = Vector2(screenX, screenY) / viewPortSize * uiCanvasSIze;

	for (UI2DRenderer& ui : ComponentItr<UI2DRenderer>())
	{
		if (ui.Sibling<OnOff>(m_GameObjectManager)->IsOn == false)
			continue;

		Vector2 bottomRight = ui.Position + ui.Size;
		NeoButton2D* button2D = ui.Sibling<NeoButton2D>(m_GameObjectManager);

		if (button2D == nullptr)
		{
			continue;
		}

		if (ui.Position.x < uiClickPosition.x
			&& ui.Position.y < uiClickPosition.y
			&& bottomRight.x > uiClickPosition.x
			&& bottomRight.y > uiClickPosition.y
			&& !button2D->bIsOvered)
		{
			EventManager::ScheduleEvent(button2D->ButtonOverEvent, button2D->OverUserData);
			button2D->bIsOvered = true;
		}
	}
}

void MousePickingSystem::HandleMouseOutEvent(int screenX, int screenY)
{
	const Vector2& viewPortSize = RenderManager::GetInstance().GetEditorViewPortSize();
	const Vector2& uiCanvasSIze = RenderManager::GetInstance().GetUICanvasSize();
	const Vector2 uiClickPosition = Vector2(screenX, screenY) / viewPortSize * uiCanvasSIze;

	for (UI2DRenderer& ui : ComponentItr<UI2DRenderer>())
	{
		if (ui.Sibling<OnOff>(m_GameObjectManager)->IsOn == false)
			continue;

		Vector2 bottomRight = ui.Position + ui.Size;
		NeoButton2D* button2D = ui.Sibling<NeoButton2D>(m_GameObjectManager);

		if (button2D == nullptr)
		{
			continue;
		}

		if (!(ui.Position.x < uiClickPosition.x
			&& ui.Position.y < uiClickPosition.y
			&& bottomRight.x > uiClickPosition.x
			&& bottomRight.y > uiClickPosition.y)
			&& button2D->bIsOvered)
		{
			EventManager::ScheduleEvent(button2D->ButtonOutEvent, button2D->OutUserData);
			button2D->bIsOvered = false;
		}
	}
}