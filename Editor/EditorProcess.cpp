#include "pch.h"
#include "EditorProcess.h"


#include "Asset.h"
#include "MenuBar.h"
#include "EditorViewport.h"
#include "Hierarchy.h"
#include "Inspector.h"

#include "TimeManager.h"
#include "../ABGraphics/RenderManager.h"
#include "../ABGraphics/GeometryGenerator.h"

#include "RenderSystem.h"
#include "TransformSystem.h"
#include "Input.h"
#include "EventManager.h"
#include "../SoundEngine/ISoundEngine.h"

#pragma comment(lib, "../lib/PhysXCommon_64.lib")
#pragma comment(lib, "../lib/PhysXCooking_64.lib")
#pragma comment(lib, "../lib/PhysXFoundation_64.lib")

EditorProcess::EditorProcess(HINSTANCE hInst, const std::string_view& title, int width, int height)
	: TickTockGuardians(hInst, title, width, height, true)
{
	// imgui폰트를 바꾸고싶다.
	ImGui::GetIO().Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Arial.ttf", 16.0f);

	m_Windows.emplace_back(std::make_unique<MenuBar>(m_GameObjects.get()));

	MenuBar* menuBar = dynamic_cast<MenuBar*>(m_Windows[0].get());

	m_Windows.emplace_back(std::make_unique<Asset>(m_GameObjects.get(), menuBar));
	m_Windows.emplace_back(std::make_unique<EditorViewport>(m_GameObjects.get()));
	m_Windows.emplace_back(std::make_unique<Hierarchy>(m_GameObjects.get()));
	m_Windows.emplace_back(std::make_unique<Inspector>(m_GameObjects.get()));

	GeometryGenerator::MeshData meshdata;
	GeometryGenerator::CreateBox(1, 1, 1, &meshdata);
	RenderManager::GetInstance().CreateGeometryModel("Box", meshdata);

	GeometryGenerator::MeshData meshdata2;
	GeometryGenerator::CreateSphere(1, 20, 20, &meshdata2);
	RenderManager::GetInstance().CreateGeometryModel("Sphere", meshdata2);

	// RenderManager::GetInstance().SetSkymap("../Resources/Texture/CubicFirstCubeMapEnvHDR.dds");
	// RenderManager::GetInstance().SetIBL(
	// 	"../Resources/Texture/CubicFirstCubeMapDiffuseHDR.dds",
	// 	"../Resources/Texture/CubicFirstCubeMapSpecularHDR.dds",
	// 	"../Resources/Texture/CubicFirstCubeMapBrdf.dds");

	TweakDarculaStyle();
}

EditorProcess::~EditorProcess()
{
}

void EditorProcess::TweakDarculaStyle()
{
	auto& colors = ImGui::GetStyle().Colors;
	colors[ImGuiCol_WindowBg] = ImVec4{ 0.1f, 0.1f, 0.13f, 1.0f };
	colors[ImGuiCol_MenuBarBg] = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f };

	// Border
	colors[ImGuiCol_Border] = ImVec4{ 0.44f, 0.37f, 0.61f, 0.29f };
	colors[ImGuiCol_BorderShadow] = ImVec4{ 0.0f, 0.0f, 0.0f, 0.24f };

	// Text
	colors[ImGuiCol_Text] = ImVec4{ 1.0f, 1.0f, 1.0f, 1.0f };
	colors[ImGuiCol_TextDisabled] = ImVec4{ 0.5f, 0.5f, 0.5f, 1.0f };

	// Headers
	colors[ImGuiCol_Header] = ImVec4{ 0.13f, 0.13f, 0.17, 1.0f };
	colors[ImGuiCol_HeaderHovered] = ImVec4{ 0.19f, 0.2f, 0.25f, 1.0f };
	colors[ImGuiCol_HeaderActive] = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f };

	// Buttons
	colors[ImGuiCol_Button] = ImVec4{ 0.13f, 0.13f, 0.17, 1.0f };
	colors[ImGuiCol_ButtonHovered] = ImVec4{ 0.19f, 0.2f, 0.25f, 1.0f };
	colors[ImGuiCol_ButtonActive] = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f };
	colors[ImGuiCol_CheckMark] = ImVec4{ 0.74f, 0.58f, 0.98f, 1.0f };

	// Popups
	colors[ImGuiCol_PopupBg] = ImVec4{ 0.1f, 0.1f, 0.13f, 0.92f };

	// Slider
	colors[ImGuiCol_SliderGrab] = ImVec4{ 0.44f, 0.37f, 0.61f, 0.54f };
	colors[ImGuiCol_SliderGrabActive] = ImVec4{ 0.74f, 0.58f, 0.98f, 0.54f };

	// Frame BG
	colors[ImGuiCol_FrameBg] = ImVec4{ 0.13f, 0.13, 0.17, 1.0f };
	colors[ImGuiCol_FrameBgHovered] = ImVec4{ 0.19f, 0.2f, 0.25f, 1.0f };
	colors[ImGuiCol_FrameBgActive] = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f };

	// Tabs
	colors[ImGuiCol_Tab] = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f };
	colors[ImGuiCol_TabHovered] = ImVec4{ 0.24, 0.24f, 0.32f, 1.0f };
	colors[ImGuiCol_TabActive] = ImVec4{ 0.2f, 0.22f, 0.27f, 1.0f };
	colors[ImGuiCol_TabUnfocused] = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f };
	colors[ImGuiCol_TabUnfocusedActive] = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f };

	// Title
	colors[ImGuiCol_TitleBg] = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f };
	colors[ImGuiCol_TitleBgActive] = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f };
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f };

	// Scrollbar
	colors[ImGuiCol_ScrollbarBg] = ImVec4{ 0.1f, 0.1f, 0.13f, 1.0f };
	colors[ImGuiCol_ScrollbarGrab] = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f };
	colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4{ 0.19f, 0.2f, 0.25f, 1.0f };
	colors[ImGuiCol_ScrollbarGrabActive] = ImVec4{ 0.24f, 0.24f, 0.32f, 1.0f };

	// Seperator
	colors[ImGuiCol_Separator] = ImVec4{ 0.44f, 0.37f, 0.61f, 1.0f };
	colors[ImGuiCol_SeparatorHovered] = ImVec4{ 0.74f, 0.58f, 0.98f, 1.0f };
	colors[ImGuiCol_SeparatorActive] = ImVec4{ 0.84f, 0.58f, 1.0f, 1.0f };

	// Resize Grip
	colors[ImGuiCol_ResizeGrip] = ImVec4{ 0.44f, 0.37f, 0.61f, 0.29f };
	colors[ImGuiCol_ResizeGripHovered] = ImVec4{ 0.74f, 0.58f, 0.98f, 0.29f };
	colors[ImGuiCol_ResizeGripActive] = ImVec4{ 0.84f, 0.58f, 1.0f, 0.29f };

	// Docking
	colors[ImGuiCol_DockingPreview] = ImVec4{ 0.44f, 0.37f, 0.61f, 1.0f };

	auto& style = ImGui::GetStyle();
	style.TabRounding = 4;
	style.ScrollbarRounding = 9;
	style.WindowRounding = 7;
	style.GrabRounding = 3;
	style.FrameRounding = 3;
	style.PopupRounding = 4;
	style.ChildRounding = 4;
}

void EditorProcess::Update()
{
	if (MenuBar::IsPlaying && !MenuBar::IsPause)
	{
		TickTockGuardians::Update();
	}
	else
	{
		m_Time->Update();
		m_DeltaTime = m_Time->GetDeltaTime();

		TransformSystem(m_GameObjects.get()).Update(m_DeltaTime);

		m_Input->Update();

		EventManager::Update(m_DeltaTime);
	}
}

void EditorProcess::Render()
{
	TickTockGuardians::Render();
}

void EditorProcess::OnImGuiRender()
{
	ImGui::DockSpaceOverViewport();


	for (auto& window : m_Windows)
	{
		window->OnImGuiRender();
	}
}
