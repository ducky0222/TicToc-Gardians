#include "pch.h"
#include "Asset.h"
#include <filesystem>

#include "GameObjectManager.h"
#include "SerializeSystem.h"
#include "TransformSystem.h"
#include "MenuBar.h"
#include <imgui_internal.h>
#include <imgui_stdlib.h>

Asset::Asset(GameObjectManager* goManager, MenuBar* menuBar) :
	m_GameObjectManager(goManager),
	m_MenuBar(menuBar)
{
	auto currPath = std::filesystem::current_path();

	auto ABPath = currPath.parent_path();
	ABPath.append("Data");

	AssetPath = ABPath.string();
}



void Asset::OnImGuiRender()
{
	ImGui::Begin("Asset");

	// Left : 에셋이나 리소스 등의 디렉토리 계층구조
	{
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_HorizontalScrollbar;
		ImGui::BeginChild("ChildL", ImVec2(ImGui::GetContentRegionAvail().x * 0.3f, 0), ImGuiChildFlags_None, window_flags);
		RenderAssetDirectory(AssetPath);
		ImGui::EndChild();
	}

	ImGui::SameLine();

	// Right : 디렉토리 내부의 파일들을 버튼이나 뭐로 브라우징
	{
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_None;
		//	window_flags |= ImGuiWindowFlags_MenuBar;
		ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
		ImGui::BeginChild("ChildR", ImVec2(0, 0), ImGuiChildFlags_Border, window_flags);
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("Menu"))
			{
				//ShowExampleMenuFile();
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}
		ImGui::InputText("Asset Search", &input);
		ImGui::Separator();

		if (ImGui::BeginTable("split", 4, ImGuiTableFlags_Resizable | ImGuiTableFlags_NoSavedSettings | ImGuiTableFlags_NoBordersInBody))
		{
			RenderSelectedPathFiles();
			ImGui::EndTable();
		}
		ImGui::EndChild();
		ImGui::PopStyleVar();
	}

	ImGui::End();
}

void Asset::RenderAssetDirectory(const std::string& path)
{
	std::filesystem::path p(path);

	if (!std::filesystem::exists(p) || !std::filesystem::is_directory(p))
		return;
	// p 내부에 디렉토리가 있는지 검사
	bool hasSubDir = false;
	for (const auto& entry : std::filesystem::directory_iterator(path))
	{
		if (entry.is_directory())
		{
			hasSubDir = true;
			break;
		}
	}

	ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanFullWidth;
	if (!hasSubDir)
		node_flags |= ImGuiTreeNodeFlags_Leaf;
	if (SelectedPath == path)
		node_flags |= ImGuiTreeNodeFlags_Selected;
	bool nodeOpen = ImGui::TreeNodeEx(p.filename().string().c_str(), node_flags);

	if (ImGui::IsItemClicked())
	{
		SelectedPath = path;
	}

	if (nodeOpen) {
		for (const auto& entry : std::filesystem::directory_iterator(path)) {
			if (entry.is_directory()) {
				RenderAssetDirectory(entry.path().string());
			}
		}
		ImGui::TreePop();
	}
}

void Asset::RenderSelectedPathFiles()
{
	if (SelectedPath.empty())
		return;

	int id = 2938;



	string inputTemp;

	std::transform(input.begin(), input.end(), std::back_inserter(inputTemp), ::tolower);
	//TODO 여기서 할떈 플레이 모드에 따라서 따로 해줘야할듯하다.
	for (const auto& entry : std::filesystem::directory_iterator(SelectedPath))
	{
		if (entry.is_regular_file())
		{

			string name = entry.path().filename().string();
			string temp;
			std::transform(name.begin(), name.end(), std::back_inserter(temp), ::tolower);
			if (temp.find(inputTemp) == string::npos)
			{
				continue;
			}

			ImGui::TableNextColumn();

			ImGui::PushID(name.c_str());
			if (ImGui::Button(entry.path().filename().string().c_str(), ImVec2(-FLT_MIN, 0.0f)))
			{
				SerializeSystem sSys{ m_GameObjectManager };

				if (entry.path().extension() == ".scene")
				{
					if (MenuBar::IsPlaying)
					{
						// GameProcess Event
						EventManager::PublishImmediate("OnStopButton");

						MenuBar::IsPlaying = false;

					}
					string filePath = entry.path().string();
					size_t separator = filePath.find_last_of("\\");
					m_MenuBar->CurrentScenePath = filePath.substr(0, separator + 1);
					m_MenuBar->CurrentSceneName = filePath.substr(separator + 1);
					size_t lastindex = m_MenuBar->CurrentSceneName.find_last_of(".");
					m_MenuBar->CurrentSceneName = m_MenuBar->CurrentSceneName.substr(0, lastindex);
					sSys.DeserializeScene(nullptr, entry.path().string());
				}
				else if (entry.path().extension() == ".prefab" && !MenuBar::IsPlaying)
				{
					sSys.DeserializePrefab(entry.path().string());
				}
				else if(entry.path().extension() == ".prefab" && MenuBar::IsPlaying)
				{
					sSys.Instantiate(entry.path().string());
				}

			}
			ImGui::PopID();
		}
	}
}