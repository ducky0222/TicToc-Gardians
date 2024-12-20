#include "pch.h"
#include "MenuBar.h"
#include "GameObjectManager.h"
#include "ResetSystem.h"
#include "ImGuiFileDialog.h"
#include "SerializeSystem.h"
#include "TransformSystem.h"
#include "Scene.h"
#include <imgui_internal.h>
#include "../ABGraphics/RenderManager.h"
#include "../SoundEngine/ISoundEngine.h"

MenuBar::MenuBar(GameObjectManager* goManager) :
	m_GameObjectManager(goManager)
{
}

void MenuBar::OnImGuiRender()
{
	SerializeSystem sSys{ m_GameObjectManager };

	auto io = ImGui::GetIO(); 
	if (io.KeysDown[ImGuiKey_LeftCtrl] && io.KeysDown[ImGuiKey_S])
	{
		NewSceneFlag = false;
		SaveCurrentScene();
	}


	ImGui::BeginMainMenuBar();
	if (ImGui::BeginMenu("Menu"))
	{
		RenderMenuFilie();
		ImGui::EndMenu();
	}

	ImGui::SameLine();
	if (ImGui::BeginMenu("Help"))
	{
		AboutMenu();

		ImGui::EndMenu();
	}

	RenderAboutPopup();


	// 중간에 버튼 하나를 배치할것이다.
	ImGui::SameLine(ImGui::GetWindowWidth() / 2 - 63);
	if (IsPlaying)
	{
		if (ImGui::Button("Stop", ImVec2(60, 0)))
		{
			// GameProcess Event
			EventManager::PublishImmediate("OnStopButton");

			auto renderCam = RenderManager::GetInstance().GetCamera();
			renderCam->LookAt(PrevCameraMatrix.Translation(), PrevCameraMatrix.Translation() + PrevCameraMatrix.Backward(), PrevCameraMatrix.Up());
			ResetSystem(m_GameObjectManager).Reset();
			sSys.DeserializeScene(nullptr, "../temp/temp.scene");
			IsPlaying = false;

			Sound::GetInstance()->CleanChannel();
		}
	}
	else
	{
		if (ImGui::Button("Play", ImVec2(60, 0)))
		{
			auto renderCam = RenderManager::GetInstance().GetCamera();
			PrevCameraMatrix = renderCam->GetView().Invert();
			Scene scene("temp");
			sSys.SerializeScene(&scene, "../temp/");
			IsPlaying = true;

			// GameProcess Event
			EventManager::PublishImmediate("OnPlayButton");
		}
	}

	ImGui::SameLine(ImGui::GetWindowWidth() / 2 + 3);

	if (IsPause)
	{
		if (ImGui::Button("Resume", ImVec2(60, 0)))
		{
			IsPause = false;
		}
	}
	else
	{
		if (ImGui::Button("Pause", ImVec2(60, 0)))
		{
			IsPause = true;
		}
	}



	ImGui::SameLine(ImGui::GetWindowWidth() - 200);
	ImGui::Text("Scene : %s", CurrentSceneName.c_str());

	ImGui::EndMainMenuBar();


	AlertSaveScene();



	if (ImGuiFileDialog::Instance()->Display("ChooseSceneDlgKey")) {
		if (ImGuiFileDialog::Instance()->IsOk()) { // action if OK
			std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
			std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();
			// action
			sSys.DeserializeScene(nullptr, filePathName);
			//pcSys.Start();

			CurrentSceneName = ImGuiFileDialog::Instance()->GetCurrentFileName();
			size_t lastindex = CurrentSceneName.find_last_of(".");
			CurrentSceneName = CurrentSceneName.substr(0, lastindex);
			CurrentScenePath = filePath + "/";

			m_RecentQueue.push_back(filePathName);
			if (m_RecentQueue.size() > 5)
			{
				m_RecentQueue.pop_front();
			}
		}

		// close
		ImGuiFileDialog::Instance()->Close();
	}

	if (ImGuiFileDialog::Instance()->Display("SaveAsDlgKey")) {
		if (ImGuiFileDialog::Instance()->IsOk()) { // action if OK
			std::string currentFileName = ImGuiFileDialog::Instance()->GetCurrentFileName();
			std::string currentFileNameWithoutExt;
			size_t lastindex = currentFileName.find_last_of(".");
			currentFileNameWithoutExt = currentFileName.substr(0, lastindex);
			std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();
			filePath += "/";
			// action
			Scene scene(currentFileNameWithoutExt.c_str());
			sSys.SerializeScene(&scene, filePath.c_str());



			if (NewSceneFlag)
			{
				CurrentSceneName = "";
				CurrentScenePath = "";
				ResetSystem(m_GameObjectManager).Reset();
				NewSceneFlag = false;
			}
			else
			{
				CurrentSceneName = currentFileNameWithoutExt;
				CurrentScenePath = filePath;
			}
		}

		// close
		ImGuiFileDialog::Instance()->Close();
	}

}

void MenuBar::RenderMenuFilie()
{

	ImGuiID popup_id = ImHashStr("Do you want to save your changes?");

	//ImGui::MenuItem("(demo menu)", NULL, false, false);
	if (ImGui::MenuItem("New Scene"))
	{
		NewSceneFlag = true;

		if (DirtyFlag)
		{
			ImGui::PushOverrideID(popup_id);
			ImGui::OpenPopup("Do you want to save your changes?");
			ImGui::PopID();
		}
	}
	if (ImGui::MenuItem("Open", "Ctrl+O"))
	{

		if (DirtyFlag)
		{
			ImGui::PushOverrideID(popup_id);
			ImGui::OpenPopup("Do you want to save your changes?");
			ImGui::PopID();
		}

		IGFD::FileDialogConfig config;
		config.path = "../Data/Scene";
		config.flags = ImGuiFileDialogFlags_Modal;
		ImGuiFileDialog::Instance()->OpenDialog("ChooseSceneDlgKey", "Choose Scene", ".scene", config);

	}
	if (ImGui::BeginMenu("Open Recent"))
	{
		auto iter = m_RecentQueue.rbegin();
		while (iter != m_RecentQueue.rend())
		{
			if (ImGui::MenuItem(iter->c_str()))
			{
				if (DirtyFlag)
				{
					ImGui::PushOverrideID(popup_id);
					ImGui::OpenPopup("Do you want to save your changes?");
					ImGui::PopID();
				}

				SerializeSystem sSys{ m_GameObjectManager };
				ParentChildrenSystem pcSys{ m_GameObjectManager };

				std::string filePath = *iter;
				sSys.DeserializeScene(nullptr, filePath);

				size_t separator = filePath.find_last_of("/");
				CurrentScenePath = filePath.substr(0, separator + 1);
				CurrentSceneName = filePath.substr(separator + 1);
				size_t lastindex = CurrentSceneName.find_last_of(".");
				CurrentSceneName = CurrentSceneName.substr(0, lastindex);
				break;
			}
			iter++;
		}

		ImGui::EndMenu();
	}
	if (ImGui::MenuItem("Save", "Ctrl+S"))
	{
		NewSceneFlag = false;

		SaveCurrentScene();
	}
	if (ImGui::MenuItem("Save As.."))
	{
		NewSceneFlag = false;

		IGFD::FileDialogConfig config;
		config.path = "../Data/Scene";
		config.flags = ImGuiFileDialogFlags_Modal | ImGuiFileDialogFlags_ConfirmOverwrite;
		ImGuiFileDialog::Instance()->OpenDialog("SaveAsDlgKey", "Save As...", ".scene", config);
	}
	if (ImGui::MenuItem("Optimize"))
	{
		TransformSystem(m_GameObjectManager).Optimize();
	}

	ImGui::Separator();
	if (ImGui::BeginMenu("Options"))
	{
		static bool enabled = true;
		ImGui::MenuItem("Enabled", "", &enabled);
		ImGui::BeginChild("child", ImVec2(0, 60), ImGuiChildFlags_Border);
		for (int i = 0; i < 10; i++)
			ImGui::Text("Scrolling Text %d", i);
		ImGui::EndChild();
		static float f = 0.5f;
		static int n = 0;
		ImGui::SliderFloat("Value", &f, 0.0f, 1.0f);
		ImGui::InputFloat("Input", &f, 0.1f);
		ImGui::Combo("Combo", &n, "Yes\0No\0Maybe\0\0");
		ImGui::EndMenu();
	}

	if (ImGui::BeginMenu("Colors"))
	{
		float sz = ImGui::GetTextLineHeight();
		for (int i = 0; i < ImGuiCol_COUNT; i++)
		{
			const char* name = ImGui::GetStyleColorName((ImGuiCol)i);
			ImVec2 p = ImGui::GetCursorScreenPos();
			ImGui::GetWindowDrawList()->AddRectFilled(p, ImVec2(p.x + sz, p.y + sz), ImGui::GetColorU32((ImGuiCol)i));
			ImGui::Dummy(ImVec2(sz, sz));
			ImGui::SameLine();
			ImGui::MenuItem(name);
		}
		ImGui::EndMenu();
	}


	if (ImGui::MenuItem("Checked", NULL, true)) {}
	ImGui::Separator();
	if (ImGui::MenuItem("Quit", "Alt+F4")) {}




}

void MenuBar::SaveCurrentScene()
{
	if (CurrentSceneName.empty())
	{
		IGFD::FileDialogConfig config;
		config.path = "../Data/Scene";
		config.flags = ImGuiFileDialogFlags_Modal | ImGuiFileDialogFlags_ConfirmOverwrite;
		ImGuiFileDialog::Instance()->OpenDialog("SaveAsDlgKey", "Save As...", ".scene", config);
	}
	else
	{
		SerializeSystem sSys{ m_GameObjectManager };
		Scene scene(CurrentSceneName.c_str());
		sSys.SerializeScene(&scene, CurrentScenePath.c_str());

		if (NewSceneFlag)
		{
			CurrentSceneName = "";
			CurrentScenePath = "";
			ResetSystem(m_GameObjectManager).Reset();
			NewSceneFlag = false;
		}
	}


}

void MenuBar::AlertSaveScene()
{
	ImGuiID popup_id = ImHashStr("Do you want to save your changes?");

	ImGui::PushOverrideID(popup_id);

	//  팝업을 화면 가운데 배치한다.
	ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
	if (ImGui::BeginPopupModal("Do you want to save your changes?", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("Your changes will be lost if you don't save them.");
		ImGui::Separator();
		if (ImGui::Button("Save", ImVec2(100, 0)))
		{
			SaveCurrentScene();
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("Don't Save", ImVec2(100, 0)))
		{
			CurrentSceneName = "";
			CurrentScenePath = "";
			ResetSystem(m_GameObjectManager).Reset();
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel", ImVec2(100, 0)))
		{
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}
	ImGui::PopID();

}

void MenuBar::AboutMenu()
{
	ImGuiID popup_id = ImHashStr("About");


	if (ImGui::MenuItem("About"))
	{
		ImGui::PushOverrideID(popup_id);

		ImGui::OpenPopup("About");

		ImGui::PopID();
	}



}

void MenuBar::RenderAboutPopup()
{
	ImGuiID popup_id = ImHashStr("About");

	ImGui::PushOverrideID(popup_id);

	//  팝업을 화면 가운데 배치한다.
	ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
	ImGuiWindowFlags flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoResize;
	if (ImGui::BeginPopup("About", flags))
	{
		ImGui::Text("Tic-tok Game engine editor Version 1.0");
		ImGui::Separator();
		ImGui::Text("NeoNeoTT Game Enigne Version 1.19");
		ImGui::Text("SweetRedBean Graphics Engine Version 2.3");
		ImGui::Text("Hysx Engine Lip Version 1.20 lip");
		ImGui::Text("");
		ImGui::Text("Developed by Programming 5th 4Q Team AngryBird");
		ImGui::Separator();
		// 이름 가운데 정렬

		RenderTextCenter("Im SangHak");
		RenderTextCenter("Kim JeongWoo");
		RenderTextCenter("Lee DongHyu");
		RenderTextCenter("Hong Jihwan");
		RenderTextCenter("Jeong Changdo");


		ImGui::Separator();
		if (ImGui::Button("Close", ImVec2(100, 0)))
		{
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}
	ImGui::PopID();
}

void MenuBar::RenderTextCenter(const char* text)
{
	float textWidth = ImGui::CalcTextSize(text).x;
	float xPos = (ImGui::GetWindowSize().x - textWidth) * 0.5f;
	ImGui::SetCursorPosX(xPos);
	ImGui::Text(text);
}
