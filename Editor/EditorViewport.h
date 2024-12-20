#pragma once
#include "IWindow.h"
#include "EditorCamera.h"

class GameObjectManager;

class EditorViewport : public IWindow
{
public:
	EditorViewport(GameObjectManager* goManger);
	~EditorViewport() override = default;

	void OnImGuiRender() override;


private:
	void RenderCamMove(float deltaTime);
	void RenderCamRotate(float deltaTime);
	void RenderEditorMode();
	void RenderPlayingMode();

public:
	Vector2 ViewPortSize = Vector2(0, 0);
	Vector2 ViewPortPos = Vector2(0, 0);

	Vector3 TranslationSnapValue = Vector3(1.0f);
	Vector3 RotationSnapValue = Vector3(15.0f);
	Vector3 ScaleSnapValue = Vector3(0.1f);

	Vector3* CurrentModeSnap = &TranslationSnapValue;
	ImGuizmo::OPERATION Operation = ImGuizmo::TRANSLATE;
	ImGuizmo::MODE Mode = ImGuizmo::MODE::LOCAL;


	float FieldOfViewValue = 45.0f * (XM_PI / 180.0f);
	float AspectValue = 1.0f;
	float NearClipPlaneValue = 0.1f;
	float FarClipPlaneValue = 10000.0f;

	float OrthoSize = 10.0f;

	bool IsOrthographic = false;

	GameObjectManager* m_GameObjectManager;
	//EditorCamera m_Camera;
	ImVec2 prevSize = {};


	VSM::Vector2 m_PrevCur = { 0, 0 };
};
