#include "pch.h"
#include "Inspector.h"

#include "HierarchySystem.h"
#include "GameObjectManager.h"

// test
#include "Transform.h"
#include "SerializeSystem.h"
#include "imgui_stdlib.h"
#include "TransformSystem.h"
#include "PhysicsEnums.h"
#include "Tag.h"
#include "NeoAnimationClip.h"

Inspector::Inspector(GameObjectManager* goManger)
	: m_GameObjectManager(goManger)
{
}

void Inspector::OnImGuiRender()
{
	ImGui::Begin("Inspector");

	const auto objectNum = HierarchySystem::SelectedNumber;

	if (objectNum != 0)
	{
		// 하이어라키창에서 오브젝트가 삭제당했을때 예외가 발생해야한다. || 오브젝트가 없을때 예외가 발생해야한다.
		if (!m_GameObjectManager->GetGameObject(objectNum))
		{
			ImGui::End();
			return;
		}
		ImGuiIO& io = ImGui::GetIO(); (void)io;


		ImGui::PushID(static_cast<int>(objectNum));

		auto& objectName = m_GameObjectManager->GetGameObject(objectNum)->Name;
		ImGui::InputText("Name", &objectName);

		ImGui::Separator();

		// ID 및 Prefab 버튼
		{
			// ID 텍스트와 버튼의 폭을 계산합니다.
			float textWidth = ImGui::CalcTextSize("GameObject ID : %d").x;
			float buttonWidth = ImGui::CalcTextSize("Save Prefab").x;

			// 창의 너비를 가져옵니다.
			float windowWidth = ImGui::GetWindowWidth();

			// 가운데 정렬을 위한 간격을 계산합니다.
			float centerOffset = (windowWidth - (textWidth + buttonWidth)) * 0.5f;

			// GameObject ID 텍스트의 Y 위치를 아래로 조절합니다.
			float textHeight = ImGui::GetTextLineHeight();
			auto prevY = ImGui::GetCursorPosY();
			ImGui::SetCursorPosX(centerOffset);
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + textHeight * 0.25f);

			ImGui::Text("GameObject ID : %d", objectNum);

			ImGui::SameLine();

			ImGui::SetCursorPosY(prevY);
			if (ImGui::Button("Save Prefab"))
			{
				SerializeSystem sys(m_GameObjectManager);
				if (!sys.SerializePrefab(objectNum, "../Data/Prefab/" + objectName + ".prefab"))
					ImGui::OpenPopup("Save/Load Error");
			}
		}

		if (ImGui::BeginPopupModal("Save/Load Error", NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize))
		{
			// 화면 정 가운데에 모달 고정
			ImVec2 centerPos = ImVec2((ImGui::GetIO().DisplaySize.x - ImGui::GetWindowSize().x) * 0.5f, (ImGui::GetIO().DisplaySize.y - ImGui::GetWindowSize().y) * 0.5f);
			ImGui::SetWindowPos(centerPos);

			// 텍스트 가운데 정렬
			ImGui::Text("Error: Unable to Save/Load file %s", objectName.c_str());

			// 버튼 가운데 정렬
			float buttonWidth = 120.0f; // 버튼의 폭 조절
			ImVec2 buttonPos = ImVec2((ImGui::GetWindowSize().x - buttonWidth) * 0.5f, ImGui::GetCursorPosY());
			ImGui::SetCursorPos(buttonPos);

			if (ImGui::Button("OK", ImVec2(buttonWidth, 0)))
			{
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}

		ImGui::Separator();

		const auto& map = m_GameObjectManager->GetAttachedComponents(objectNum);


		ImGui::InputText("##Inspector", &inspectorInput);

		string inspectorInputTemp;

		std::transform(inspectorInput.begin(), inspectorInput.end(), std::back_inserter(inspectorInputTemp), ::tolower);

		for (const auto& val : *map | std::views::values)
		{
			auto& typeInfo = val->GetTypeInfo();
			auto valName = typeInfo.GetName();
			auto& properties = typeInfo.GetProperties();

			string valNameString = valName;
			string inspectortemp;
			std::transform(valNameString.begin(), valNameString.end(), std::back_inserter(inspectortemp), ::tolower);

			if (inspectortemp.find(inspectorInputTemp) == string::npos)
			{
				continue;
			}

			ImGui::PushID(valNameString.c_str());
			if (ImGui::TreeNodeEx(valName, ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanFullWidth))
			{
				if (ImGui::IsMouseClicked(1) && ImGui::IsItemHovered())
				{
					ImGui::OpenPopup("ComponentPopup");
					rightClickComponent = val;
				}

				for (auto& p : properties)
				{
					RenderMember(p, val);
				}

				ImGui::TreePop();
			}
			ImGui::PopID();


			ImGui::Separator();

		}

		if (ImGui::IsMouseClicked(1) && !ImGui::IsAnyItemHovered())
		{
			rightClickComponent = nullptr;
		}

		if (rightClickComponent && ImGui::BeginPopupContextWindow("ComponentPopup"))
		{
			if (ImGui::MenuItem("Delete Component"))
			{
				//m_GameObjectManager->RemoveComponent(rightClickComponent);
				EventManager::OnDeleteComponent(rightClickComponent);
				rightClickComponent = nullptr;
			}
			ImGui::EndPopup();
		}

		ImGui::PopID();

		// Add Component Button
		{
			float buttonWidth = ImGui::CalcTextSize("\tAdd Component\t").x;
			float xPos = (ImGui::GetWindowSize().x - buttonWidth) * 0.5f;
			float yPos = ImGui::GetCursorPosY() + 20.0f;
			ImGui::SetCursorPos({ xPos, yPos });


			if (ImGui::Button("\tAdd Component\t"))
			{
				ImGui::OpenPopup("AddComponentPopup");
			}

			
			ImGui::SetNextWindowSize({ 250.f, 330.f });
			if (ImGui::BeginPopup("AddComponentPopup"))
			{
				if (!input.empty())
				{
					if (ImGui::Button("x"))
					{
						input.clear();
					}
				}
				else
				{
					if (ImGui::Button(">"))
					{

					}
				}
				ImGui::SameLine();

				ImGui::InputText("##AddComponent", &input);

			
				string inputTemp;

				std::transform(input.begin(), input.end(), std::back_inserter(inputTemp), ::tolower);

				input.empty() ? ImGui::SeparatorText("Component") : ImGui::SeparatorText("Search");

				for (auto& [name, component] : TypeInfo::GetComponentMap())
				{
					if (name == "Parent" or name == "Children")
					{
						continue;
					}

					string temp;
					std::transform(name.begin(), name.end(), std::back_inserter(temp), ::tolower);

					if (temp.find(inputTemp) == string::npos)
					{
						continue;
					}
					ImGui::PushID(name.c_str());
					if (ImGui::Button(name.c_str(), ImVec2(200.f, 0)))
					{
						component->AddThis(m_GameObjectManager, objectNum);
					}
					ImGui::PopID();
				}

				ImGui::EndPopup();
			}
		}
	}

	ImGui::End();
}

template <typename T>
void MatchGuiByType(const Property* property, Component* component)
{
	if constexpr (std::is_same_v<T, void*>) {
		ImGui::Text("Unreflected Type");
		return;
	}

	const auto name = property->GetName();
	T& val = property->Get<T>(component);

	if constexpr (std::is_same_v<T, VSM::Vector3>) {
		ImGui::DragFloat3(name, &val.x);
	}
	else if constexpr (std::is_same_v<T, VSM::Vector2>) {
		ImGui::DragFloat2(name, &val.x);
	}
	else if constexpr (std::is_same_v<T, VSM::Quaternion>) {
		Vector3 toEuler = val.ToEuler() / (Very::SimpleMath::XM_PI / 180.0f);
		ImGui::DragFloat3(name, &toEuler.x);
		val = Quaternion::CreateFromYawPitchRoll(toEuler * (Very::SimpleMath::XM_PI / 180.0f));
	}
	else if constexpr (std::is_same_v<T, VSM::Boolean3>) {
		ImGui::PushID(name);
		ImGui::Checkbox("x", &val.x);
		ImGui::SameLine();
		ImGui::Checkbox("y", &val.y);
		ImGui::SameLine();
		ImGui::Checkbox("z", &val.z);
		ImGui::SameLine();
		ImGui::Text(name);
		ImGui::PopID();

	}
	else if constexpr (std::is_same_v<T, int>) {
		ImGui::DragInt(name, &val);
	}
	else if constexpr (std::is_same_v<T, bool>) {
		ImGui::Checkbox(name, &val);
	}
	else if constexpr (std::is_same_v<T, double>) {
		ImGui::DragScalar(name, ImGuiDataType_Double, &val);
	}
	else if constexpr (std::is_same_v<T, float>) {
		ImGui::DragFloat(name, &val);
	}
	else if constexpr (std::is_same_v<T, unsigned int>) {
		ImGui::DragScalar(name, ImGuiDataType_U32, &val);
	}
	else if constexpr (std::is_same_v<T, std::string>)
	{
		ImGui::InputText(name, &val);
	}
	else if constexpr (std::is_same_v<T, std::list<uint32_t>>)
	{
		std::vector<string> list;
		for (auto i : val)
		{
			list.push_back(std::to_string(i));
		}
		int selection = -1;
		ImGui::ListBox(name, &selection,
			[](void* userdata, int idx)->const char*
			{
				return (*(std::vector<string>*)userdata)[idx].c_str();
			},
			&list, val.size(), val.size() >= 4 ? 4 : val.size());
	}
	else if constexpr (std::is_same_v<T, ePxLayer>)
	{
		static const char* items[] = { "TILE", "TRIGGER_TILE", "CHARACTER", "NO_COLLISION"};
		int enumVal = log2(static_cast<int>(val));
		ImGui::Combo(name, &enumVal, items, IM_ARRAYSIZE(items));
		val = static_cast<ePxLayer>(1 << enumVal);
	}
	else if constexpr (std::is_same_v<T, ePxMaterial>)
	{
		static const char* items[] = { "FLOOR_MATERIAL", "PLAYER_MATERIAL", "BOX_MATERIAL" };
		auto enumVal = static_cast<int>(val);
		ImGui::Combo(name, &enumVal, items, IM_ARRAYSIZE(items));
		val = static_cast<ePxMaterial>(enumVal);
	}
	else if constexpr (std::is_same_v<T, eActorType>)
	{
		static const char* items[] = { "STATIC_ACTORTYPE", "DYNAMIC_ACTORTYPE" };
		auto enumVal = static_cast<int>(val);
		ImGui::Combo(name, &enumVal, items, IM_ARRAYSIZE(items));
		val = static_cast<eActorType>(enumVal);
	}
	else if constexpr (std::is_same_v<T, eColliderShape>)
	{
		static const char* items[] = { "BOX_SHAPE", "CAPSULE_SHAPE", "CYLINDER_SHAPE" };
		auto enumVal = static_cast<int>(val);
		ImGui::Combo(name, &enumVal, items, IM_ARRAYSIZE(items));
		val = static_cast<eColliderShape>(enumVal);
	}
	else if constexpr (std::is_same_v<T, eCollisionShape>)
	{
		static const char* items[] = { "BOX_SHAPE", "CAPSULE_SHAPE", "CYLINDER_SHAPE", "GIMMICK_BOX_SHAPE" };
		auto enumVal = static_cast<int>(val);
		ImGui::Combo(name, &enumVal, items, IM_ARRAYSIZE(items));
		val = static_cast<eCollisionShape>(enumVal);
	}
	else if constexpr (std::is_same_v<T, TagType>)
	{
		static const char* items[] = { "Cat", "Rabbit", "Beaver", "StartingPoint", "DimensionCrack", "Box", "Button", "Lift", "ConveyorBelt", "BoxGenerator", "SelectCat", "SelectRabbit", "SelectBeaver" ,"BoxUI_LeftDown","BoxUI_LeftUp","BoxUI_RightDown","BoxUI_RightUp","Transparent_Wall","BoxUI_Landing"};
		auto enumVal = static_cast<int>(val);
		ImGui::Combo(name, &enumVal, items, IM_ARRAYSIZE(items));
		val = static_cast<TagType>(enumVal);
	}

	else if constexpr (std::is_same_v<T, eBeltDir>)
	{
		static const char* items[] = { "SOUTHEAST", "SOUTHWEST", "NORTHEAST", "NORTHWEST" };
		auto enumVal = static_cast<int>(val);
		ImGui::Combo(name, &enumVal, items, IM_ARRAYSIZE(items));
		val = static_cast<eBeltDir>(enumVal);
	}

	else if constexpr (std::is_same_v<T, ClipInformation>)
	{
		ImGui::Text(name);
		std::string temp = name;
		temp += ".FileName";
		ImGui::InputText(temp.c_str(), &val.FileName);
		temp = name; temp += ".SpriteEffectKey";
		ImGui::InputText(temp.c_str(), &val.SpriteEffectKey);
		temp = name; temp += ".FrameInterval";
		ImGui::DragScalar(temp.c_str(), ImGuiDataType_U32, &val.FrameInterval);
		temp = name; temp += ".bIsRecursive";
		ImGui::Checkbox(temp.c_str(), &val.bIsRecursive);
		temp = name; temp += ".SoundKey";
		ImGui::InputText(temp.c_str(), &val.SoundKey);
	}
}



void Inspector::RenderMember(const Property* property, Component* component)
{
	const auto typeHash = property->GetTypeInfo().GetHash();

	// 사용자 정의 구조체
	if (typeHash == VSM::Vector3::StaticTypeInfo().GetHash())
	{
		MatchGuiByType<VSM::Vector3>(property, component);
	}
	else if (typeHash == VSM::Vector2::StaticTypeInfo().GetHash())
	{
		MatchGuiByType<VSM::Vector2>(property, component);
	}
	else if (typeHash == VSM::Quaternion::StaticTypeInfo().GetHash())
	{
		MatchGuiByType<VSM::Quaternion>(property, component);
	}
	else if (typeHash == VSM::Boolean3::StaticTypeInfo().GetHash())
	{
		MatchGuiByType<VSM::Boolean3>(property, component);
	}
	else if (typeHash == ClipInformation::StaticTypeInfo().GetHash())
	{
		MatchGuiByType<ClipInformation>(property, component);
	}
	// c++ 기본 타입
	else
	{
		auto typeName = property->GetTypeInfo().GetFullName();

		if (std::strcmp(typeName, "int") == 0)
		{
			MatchGuiByType<int>(property, component);
		}
		else if (std::strcmp(typeName, "bool") == 0)
		{
			MatchGuiByType<bool>(property, component);
		}
		else if (std::strcmp(typeName, "double") == 0)
		{
			MatchGuiByType<double>(property, component);
		}
		else if (std::strcmp(typeName, "float") == 0)
		{
			MatchGuiByType<float>(property, component);
		}
		else if (std::strcmp(typeName, "unsigned int") == 0)
		{
			MatchGuiByType<unsigned int>(property, component);
		}
		else if (std::strcmp(typeName, "class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> >") == 0)
		{
			MatchGuiByType<std::string>(property, component);
		}
		else if (std::strcmp(typeName, "class std::list<unsigned int,class std::allocator<unsigned int> >") == 0)
		{
			MatchGuiByType<std::list<uint32_t>>(property, component);
		}
		else if (std::strcmp(typeName, "enum ePxLayer") == 0)
		{
			MatchGuiByType<ePxLayer>(property, component);
		}
		else if (std::strcmp(typeName, "enum ePxMaterial") == 0)
		{
			MatchGuiByType<ePxMaterial>(property, component);
		}
		else if (std::strcmp(typeName, "enum eActorType") == 0)
		{
			MatchGuiByType<eActorType>(property, component);
		}
		else if (std::strcmp(typeName, "enum eColliderShape") == 0)
		{
			MatchGuiByType<eColliderShape>(property, component);
		}
		else if (std::strcmp(typeName, "enum eCollisionShape") == 0)
		{
			MatchGuiByType<eCollisionShape>(property, component);
		}
		else if (std::strcmp(typeName, "enum TagType") == 0)
		{
			MatchGuiByType<TagType>(property, component);
		}
		else if (std::strcmp(typeName, "enum eBeltDir") == 0)
		{
			MatchGuiByType<eBeltDir>(property, component);
		}
		else
		{
			MatchGuiByType<void*>(property, component);
		}

	}
}