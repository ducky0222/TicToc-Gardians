#include "pch.h"
#include "EditorViewport.h"
#include "../ABGraphics/RenderManager.h"
#include "HierarchySystem.h"
#include "GameObjectManager.h"
#include "Transform.h"
#include "Input.h"
#include "MenuBar.h"

#include <imgui_internal.h>

EditorViewport::EditorViewport(GameObjectManager* goManger) :
	m_GameObjectManager(goManger)
{
	// 	m_Camera.FieldOfView = 45.0f * (XM_PI / 180.0f);
	// 	m_Camera.Aspect = 1.0f;
	// 	m_Camera.NearClipPlane = 0.1f;
	// 	m_Camera.FarClipPlane = 5000.0f;
	auto renderCam = RenderManager::GetInstance().GetCamera();
	renderCam->SetLens(FieldOfViewValue, AspectValue, NearClipPlaneValue, FarClipPlaneValue);
	renderCam->SetPosition(0.f, 3.f, -6.f);

	auto shadowCam = RenderManager::GetInstance().GetShadowCamera();
	shadowCam->SetLens(FieldOfViewValue, AspectValue, 1, 3000);
	shadowCam->LookAt({ 0, 25, 25 }, { 0, 0,0 }, { 0, 1, 0 });
}

void EditorViewport::OnImGuiRender()
{
	// 	m_Camera.CameraMove(0.01f);
	// 	m_Camera.CameraRotate(0.01f);


	ImGuiWindowFlags wflags = 0;
	//wflags |= ImGuiWindowFlags_NoTitleBar;
	//wflags |= ImGuiWindowFlags_NoScrollbar;
	wflags |= ImGuiWindowFlags_NoScrollWithMouse;
	wflags |= ImGuiWindowFlags_NoBackground;
	wflags |= ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse;

	ImGui::Begin("Viewport", 0, wflags);


	if (MenuBar::IsPlaying)
	{
		RenderPlayingMode();
	}
	else
	{
		RenderEditorMode();
	}

	ImGui::End();
}

void EditorViewport::RenderCamMove(float deltaTime)
{

	auto renderCam = RenderManager::GetInstance().GetCamera();

	if (Input::GetKey(KEY::W))
	{
		renderCam->TranslateLook(deltaTime);
	}
	if (Input::GetKey(KEY::S))
	{
		renderCam->TranslateLook(-deltaTime);
	}
	if (Input::GetKey(KEY::A))
	{
		renderCam->TranslateRight(-deltaTime);
	}
	if (Input::GetKey(KEY::D))
	{
		renderCam->TranslateRight(deltaTime);
	}
	if (Input::GetKey(KEY::Q))
	{
		renderCam->TranslateUp(-deltaTime);
	}
	if (Input::GetKey(KEY::E))
	{
		renderCam->TranslateUp(deltaTime);
	}

	if (Input::GetKey(KEY::Z))
	{
		OrthoSize += 1 * deltaTime;
	}

	if (Input::GetKey(KEY::X))
	{
		OrthoSize -= 1 * deltaTime;
	}
}

void EditorViewport::RenderCamRotate(float deltaTime)
{
	auto renderCam = RenderManager::GetInstance().GetCamera();
	Vector2 mousePos = Input::GetMousePos();

	if (Input::GetKey(KEY::RBTN))
	{
		DirectX::XMFLOAT2 offset = { mousePos.x - m_PrevCur.x, mousePos.y - m_PrevCur.y };
		if (offset.x > 0 || offset.x < 0 || offset.y < 0 || offset.y > 0)
		{
			renderCam->RotatePitch(offset.y * 0.005f);
			renderCam->RotateAxis({ 0,1,0 }, offset.x * 0.005f);
		}
	}

	m_PrevCur = mousePos;

}

void EditorViewport::RenderEditorMode()
{
	float speed = 0.01f;

	if (Input::GetKey(KEY::LSHIFT))
	{
		speed *= 10.f;
	}

	RenderCamMove(speed);
	RenderCamRotate(0.01f);


	auto current = ImGui::GetCurrentContext();
	auto pos = current->CurrentWindow->Pos;
	auto size = current->CurrentWindow->Size;
	auto idealSize = ImVec2(size.x, size.y - 3);
	auto maxpos = ImVec2(pos.x + idealSize.x, pos.y + idealSize.y);
	auto borderSize = current->CurrentWindow->WindowBorderSize;


	auto renderCam = RenderManager::GetInstance().GetCamera();

	if (prevSize.x != idealSize.x || prevSize.y != idealSize.y)
	{
		auto ratio = idealSize.x / idealSize.y;
		AspectValue = ratio;
		if (IsOrthographic)
		{
			renderCam->SetLensOrtho(OrthoSize, ratio * OrthoSize, NearClipPlaneValue, FarClipPlaneValue);
		}
		else
		{
			renderCam->SetLens(FieldOfViewValue, AspectValue, NearClipPlaneValue, FarClipPlaneValue);
		}
	}

	//auto lookat2 = XMMatrixLookAtLH({ 0, 0, -10, 0 }, { 0, 0, 0, 0 }, { 0, 1, 0, 0 });

	auto myTex = RenderManager::GetInstance().GetBackBufferSRV();

	ViewPortPos = { pos.x, pos.y };
	ViewPortSize = { idealSize.x, idealSize.y };

	ImGui::GetWindowDrawList()->AddImage(
		(void*)myTex,
		ImVec2(pos.x, pos.y + 22),
		ImVec2(maxpos.x, maxpos.y)
	);

	prevSize = size;


	renderCam->UpdateViewMatrix();

	ImGuizmo::SetOrthographic(IsOrthographic);	// 카메라 오소그래픽이랑 맞게
	ImGuizmo::SetDrawlist();	// 먼지 모름 
	ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, ImGui::GetWindowSize().x, ImGui::GetWindowSize().y);
	Matrix view = renderCam->GetView();
	Matrix proj = renderCam->GetProj();


	ImGuiWindowFlags window_flags = ImGuiWindowFlags_None;
	window_flags |= ImGuiWindowFlags_MenuBar;
	ImGuiChildFlags child_flags = ImGuiWindowFlags_None;
	child_flags |= ImGuiChildFlags_Border;
	child_flags |= ImGuiChildFlags_AutoResizeX;
	child_flags |= ImGuiChildFlags_AutoResizeY;
	child_flags |= ImGuiChildFlags_FrameStyle;

	ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
	ImGui::BeginChild("Tools", ImVec2{ 0,0 }, child_flags, window_flags);

	if (ImGui::BeginMenuBar())
	{
		//ImGui::Button("Handle");

		ImGui::EndMenuBar();

		if (ImGui::Button("Perspective"))
		{
			IsOrthographic = false;
		}

		if (ImGui::Button("Orthographic"))
		{
			IsOrthographic = true;
		}

		ImGui::Separator();

		if (ImGui::Button("Snap"))
		{
			ImGui::OpenPopup("SnapPopUp");
		}

		if (ImGui::BeginPopup("SnapPopUp"))
		{
			ImGui::DragFloat("Move", &TranslationSnapValue.x);
			TranslationSnapValue.y = TranslationSnapValue.z = TranslationSnapValue.x;
			ImGui::DragFloat("Rotate", &RotationSnapValue.x);
			RotationSnapValue.y = RotationSnapValue.z = RotationSnapValue.x;
			ImGui::DragFloat("Scale", &ScaleSnapValue.x);
			ScaleSnapValue.y = ScaleSnapValue.z = ScaleSnapValue.x;

			ImGui::EndPopup();
		}

		ImGui::Separator();

		if (ImGui::RadioButton("Translate", Operation == ImGuizmo::OPERATION::TRANSLATE))
		{
			Operation = ImGuizmo::OPERATION::TRANSLATE;
			CurrentModeSnap = &TranslationSnapValue;
		}
		else if (ImGui::RadioButton("Rotate", Operation == ImGuizmo::OPERATION::ROTATE))
		{
			Operation = ImGuizmo::OPERATION::ROTATE;
			CurrentModeSnap = &RotationSnapValue;
		}
		else if (ImGui::RadioButton("Scale", Operation == ImGuizmo::OPERATION::SCALE))
		{
			Operation = ImGuizmo::OPERATION::SCALE;
			CurrentModeSnap = &ScaleSnapValue;
		}

		ImGui::Separator();

		if (ImGui::RadioButton("Local", Mode == ImGuizmo::MODE::LOCAL))
		{
			Mode = ImGuizmo::MODE::LOCAL;
		}
		else if (ImGui::RadioButton("World", Mode == ImGuizmo::MODE::WORLD))
		{
			Mode = ImGuizmo::MODE::WORLD;
		}
	}
	ImGui::EndChild();
	ImGui::PopStyleVar();


	Transform* transform = m_GameObjectManager->QueryComponent<Transform>(HierarchySystem::SelectedNumber);
	if (transform)
	{
		float* snapValue = 0;

		if (Input::GetKey(KEY::CTRL))
		{
			snapValue = reinterpret_cast<float*>(CurrentModeSnap);
		}

		VSM::Matrix m = transform->WorldMatrix;
		ImGuizmo::Manipulate(
			&view.m[0][0],
			&proj.m[0][0],
			Operation,
			Mode,
			&m.m[0][0], 0, snapValue
		);


		if (ImGuizmo::IsUsing())
		{
			auto parent = m_GameObjectManager->QueryComponent<Parent>(HierarchySystem::SelectedNumber);
			if (parent)
			{
				auto parentTransform = m_GameObjectManager->QueryComponent<Transform>(parent->ParentID);
				m = m * parentTransform->WorldMatrix.Invert();
			}
			VSM::Vector3 matrixTranslation, matrixScale;
			VSM::Quaternion matrixRotation;
			m.Decompose(matrixScale, matrixRotation, matrixTranslation);

			switch (Operation)
			{
			case ImGuizmo::OPERATION::TRANSLATE:
				transform->LocalPosition = matrixTranslation;
				break;

			case ImGuizmo::OPERATION::ROTATE:
				transform->LocalRotation = matrixRotation;
				break;

			case ImGuizmo::OPERATION::SCALE:
				transform->LocalScale = matrixScale;
				break;

			default:
				break;
			}
		}

		if (HierarchySystem::IsItemDoubleClicked)
		{
			auto scale = transform->WorldScale;
			auto avg = (std::fabs(scale.x) + std::fabs(scale.y) + std::fabs(scale.z)) / 3;
			static float t = 0;
			renderCam->SetPosition(Vector3::Lerp(
				renderCam->GetPosition(),
				transform->WorldPosition + static_cast<VSM::Vector3>(-renderCam->GetLook()) * 10 * avg, t)
			);
			t += 0.01f;


			if (t > 1.0f)
			{
				HierarchySystem::IsItemDoubleClicked = false;
				t = 0;
			}
		}
	}
}

void EditorViewport::RenderPlayingMode()
{
	auto current = ImGui::GetCurrentContext();
	auto pos = ImVec2{ current->CurrentWindow->Pos.x, current->CurrentWindow->Pos.y + 25.f };
	auto size = ImVec2{ current->CurrentWindow->Size.x, current->CurrentWindow->Size.y - 25.f };
	auto idealSize = ImVec2(size.x, size.y);
	auto maxpos = ImVec2(pos.x + idealSize.x, pos.y + idealSize.y);
	auto borderSize = current->CurrentWindow->WindowBorderSize;


	auto myTex = RenderManager::GetInstance().GetBackBufferSRV();

	//1920 1080
	// IdealSize
	auto myViewPortratio = 1920.f / 1080.f;
	auto inGameViewPortratio = idealSize.x / idealSize.y;

	ImVec2 realPos = {};
	ImVec2 realSize = {};
	if (myViewPortratio > inGameViewPortratio)
	{
		// 내 가로가 더 긴것
		// 뷰포트의 위아래가 남는다.
		realPos.x = pos.x;

		realSize.x = idealSize.x;
		realSize.y = realSize.x / myViewPortratio;

		realPos.y = pos.y + (idealSize.y - realSize.y) / 2;


	}
	else if (myViewPortratio <= inGameViewPortratio)
	{
		// 뷰포트의 가로가 더 긴것
		// 뷰포트의 좌우가 남는다. 
		realPos.y = pos.y;

		realSize.y = idealSize.y;
		realSize.x = realSize.y * myViewPortratio;

		realPos.x = pos.x + (idealSize.x - realSize.x) / 2;
	}
	auto realMaxpos = ImVec2(realPos.x + realSize.x, realPos.y + realSize.y);

	RenderManager::GetInstance().SetEditorViewPort({ realSize.x, realSize.y }, { realPos.x, realPos.y });



	ImGui::GetWindowDrawList()->AddImage(
		(void*)myTex,
		ImVec2(realPos.x, realPos.y),
		ImVec2(realMaxpos.x, realMaxpos.y)
	);
}
