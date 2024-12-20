#include "pch.h"
#include "Hierarchy.h"
#include "HierarchySystem.h"
#include "TransformSystem.h"
#include "GameObjectManager.h"
#include "imgui_internal.h"

Hierarchy::Hierarchy(GameObjectManager* goManager)
	: m_GameObjectManager(goManager)
{
}

void Hierarchy::OnImGuiRender()
{
	ImGui::Begin("Hierarchy");

	auto hs = HierarchySystem{ m_GameObjectManager };


	ImGuiIO& io = ImGui::GetIO(); (void)io;
	auto curcontext = ImGui::GetCurrentContext();
	auto currwindow = curcontext->CurrentWindow;
	auto pos = currwindow->Pos;
	auto size = currwindow->Size;
	IsMouseIn = io.MouseClickedPos[0].x > pos.x && io.MouseClickedPos[0].x < pos.x + size.x && io.MouseClickedPos[0].y > pos.y && io.MouseClickedPos[0].y < pos.y + size.y;

	if (io.MouseClicked[0] && IsMouseIn)
	{
		HierarchySystem::SelectedNumber = 0;
	}

	hs.EditorRender();

	if (const ImGuiPayload* payload = ImGui::GetDragDropPayload(); payload && ImGui::IsMouseReleased(0) && Hierarchy::IsMouseIn)
	{
		auto move_from = *(const int*)payload->Data;

		ParentChildrenSystem(m_GameObjectManager).RemoveParent(move_from);
	}

	if (!HierarchySystem::IsObjectRightClicked && ImGui::BeginPopupContextWindow())
	{
		if (ImGui::MenuItem("Create Empty"))
		{
			m_GameObjectManager->CreateGameObject("GameObject");
		}
	
		ImGui::EndPopup();
	}


	ImGui::End();
}
