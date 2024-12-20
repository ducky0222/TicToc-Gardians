#include "pch.h"
#include "HierarchySystem.h"

#include "Transform.h"
#include "imgui.h"
#include "TransformSystem.h"
#include "EventManager.h"

void HierarchySystem::EditorRender()
{
	for (Transform& transform : ComponentItr<Transform>())
	{
		// �θ� ������ ��Ʈ
		if (transform.Sibling<Parent>(m_GameObjectManager) != nullptr)
			continue;

		auto k = m_GameObjectManager->GetGameObjectName(transform.GameObjectId).data();
		RealRender(transform.GameObjectId);
	}

	if (ImGui::IsMouseClicked(1) && !ImGui::IsAnyItemHovered())
	{
		IsObjectRightClicked = false;
	}
}

void HierarchySystem::RealRender(uint32_t id)
{
	// ���� ������Ʈ imgui ������
	bool isNodeOpen = false;
	auto children = m_GameObjectManager->QueryComponent<Children>(id);


	ImGui::PushID(id);

	ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanFullWidth;
	if (children == nullptr)
		node_flags |= ImGuiTreeNodeFlags_Leaf;
	if (SelectedNumber == id)
		node_flags |= ImGuiTreeNodeFlags_Selected;
   	isNodeOpen = ImGui::TreeNodeEx(m_GameObjectManager->GetGameObject(id)->Name.c_str(), node_flags);

	// ����Ŭ���ϸ� �� ���۵ǰ� �ϰ�ʹ�.
	if (ImGui::IsMouseDoubleClicked(0) && ImGui::IsItemHovered())
	{
		IsItemDoubleClicked = true;
	}

	if (ImGui::IsItemClicked(1))
	{
		ImGui::OpenPopup("GameObject Popup");
		RClickedNumber = id;
		IsObjectRightClicked = true;
	}

	if (IsObjectRightClicked && ImGui::BeginPopupContextWindow("GameObject Popup"))
	{
		if (ImGui::MenuItem("Delete"))
		{
			//m_GameObjectManager->ReleaseGameObject(transform.GameObjectId);
			EventManager::OnDeleteGameObject(RClickedNumber);
			RClickedNumber = 0;
			IsObjectRightClicked = false;
		}


		ImGui::EndPopup();
	}



	/// ������ �巡�� �� ���
	ImGuiDragDropFlags target_flags = 0;

	// �巡�� �ҽ��� �Ǵ� ���ӿ�����Ʈ
	if (ImGui::BeginDragDropSource(target_flags))
	{
		//int data = HierarchySystem::S
		ImGui::SetDragDropPayload("GameObject", &id, sizeof(uint32_t));

		ImGui::Text("%s", m_GameObjectManager->GetGameObjectName(id).data());
		ImGui::EndDragDropSource();
	}
	
	// ����� �� ���ӿ�����Ʈ
	if (ImGui::BeginDragDropTarget())
	{
		ImGuiDragDropFlags target_flags = 0;
		//target_flags |= ImGuiDragDropFlags_AcceptBeforeDelivery;    // Don't wait until the delivery (release mouse button on a target) to do something
		//target_flags |= ImGuiDragDropFlags_AcceptNoDrawDefaultRect; // Don't display the yellow rectangle
		//target_flags |= ImGuiDragDropFlags_AcceptPeekOnly;          // Don't wait until the mouse button is released to display the payload

		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("GameObject", target_flags))
		{
			auto move_from = *(const int*)payload->Data;
			auto move_to = id;

			if (move_from != move_to)
			{
				ParentChildrenSystem(m_GameObjectManager).AddChild(move_to, move_from);
			}

		}
		ImGui::EndDragDropTarget();
	}

	ImGui::PopID();



	
	if (ImGui::IsItemClicked())
	{
		SelectedNumber = id;
	}


	if (isNodeOpen)
	{
		if (children == nullptr)
		{
			ImGui::TreePop();
			return;
		}

		// ���� ������Ʈ�� �ڽĵ鿡 ���� ����
		for (auto k : children->ChildrenID)
		{
			RealRender(k);
		}

		// Ʈ�� ��� �ݱ�
		ImGui::TreePop();
	}

}
