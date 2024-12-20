#include "pch.h"
#include "GameProcess.h"

#include "TimeManager.h"
#include "Input.h"

// Graphics
#include "../ABGraphics/RenderManager.h"
#include "../ABGraphics/DebugRenderManager.h"

// temp
//#include "Graphics.h"
#include "../Reflection/TypeInfo.h"

// test
#include "Transform.h"

#include "ComponentPoolManager.h"
#include "SystemManager.h"
#include "ISoundEngine.h"
#include "HysxEngine.h"

#include "TransformSystem.h"
#include "EventManager.h"
#include "EventTestSystem.h"
#include "GameObjectManager.h"
#include "RenderSystem.h"
//#include "PlayableSystem.h"
#include "CameraSystem.h"
#include "OnOffSystem.h"
#include "PhysicsSystem.h"
#include "SerializeSystem.h"




LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

#ifdef LOG_CONSOLE
#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console")
#endif

bool g_InitComplete = false;

GameProcess::GameProcess(HINSTANCE hInst, std::string_view title, int width, int height, bool bImGuiInit)
	: m_Title(title), m_bImGuiRender(bImGuiInit)
{
	EventManager::Subscribe("OnPlayButton", CreateListenerInfo(&GameProcess::OnPlayButton));
	EventManager::Subscribe("OnStopButton", CreateListenerInfo(&GameProcess::OnStopButton));
	EventManager::Subscribe("OnStart", CreateListenerInfo(&GameProcess::OnStart));
	EventManager::Subscribe("OnLoadScene", CreateListenerInfo(&GameProcess::OnLoadScene));
	EventManager::Subscribe("OnSetSpeedWeight", CreateListenerInfo(&GameProcess::OnSetSpeedWeight));
	EventManager::Subscribe("OnDestroy", CreateListenerInfo(&GameProcess::OnDestroy));

#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_CrtSetBreakAlloc(515);
#endif


	auto wTitle = std::wstring(title.begin(), title.end());

	// 윈도우 클래스 구조체 정의
	WNDCLASS wndclass{};
	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = WndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInst;
	wndclass.hIcon = LoadIcon(hInst, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = wTitle.c_str();

	RegisterClass(&wndclass);

	// Editor mode
	if (m_bImGuiRender)
	{
		RECT rcClient = { 0, 0, (LONG)width, (LONG)height };
		AdjustWindowRect(&rcClient, WS_OVERLAPPEDWINDOW, FALSE);

		m_hWnd = CreateWindowW(
			wTitle.c_str(),
			wTitle.c_str(),
			WS_OVERLAPPEDWINDOW,
			100, 100, rcClient.right - rcClient.left, rcClient.bottom - rcClient.top,
			NULL, NULL, hInst, NULL);
	}
	// Launcher mode
	else
	{
		RECT rcClient = { 0, 0, (LONG)width, (LONG)height };
		AdjustWindowRect(&rcClient, WS_POPUP | WS_MINIMIZEBOX | WS_MAXIMIZEBOX, FALSE);

		m_hWnd = CreateWindowW(
			wTitle.c_str(),
			wTitle.c_str(),
			WS_POPUP | WS_MINIMIZEBOX | WS_MAXIMIZEBOX,
			0, 0, rcClient.right - rcClient.left, rcClient.bottom - rcClient.top,
			NULL, NULL, hInst, NULL);
	}

	assert(m_hWnd);

	m_Time = std::make_unique<TimeManager>();
	m_ComponentPools = std::make_unique<ComponentPoolManager>();
	m_GameObjects = std::make_unique<GameObjectManager>();

	/// 렌더매니저 초기화
	RenderManager::GetInstance().Init(m_hWnd, width, height, bImGuiInit);
	DebugRenderManager::Get().Init(RenderManager::GetInstance().GetDevice(), RenderManager::GetInstance().GetContext(), RenderManager::GetInstance().GetCamera());

	/// 시스템매니저 초기화
	m_SystemManager = std::make_unique<SystemManager>();
	m_SystemManager->Initialize(m_GameObjects.get());
	m_SystemManager->AddSystem<TransformSystem>();

	m_SystemManager->AddSystem<RenderSystem>();
	m_SystemManager->AddSystem<ParentChildrenSystem>();
	m_SystemManager->AddSystem<CameraSystem>();

	// test
	m_SystemManager->AddSystem<EventTestSystem>();
	m_SystemManager->AddSystem<OnOffSystem>();
	//m_SystemManager->AddSystem<PlayableSystem>();

	// physics test
	m_SystemManager->AddSystem<PhysicsSystem>();

	/// 사운드매니저 초기화
	Sound::GetInstance()->Initialize();

	/// 인풋매니저 초기화
	m_Input = &Input::GetInstance();
	m_Input->Initialize();

	/// 물리엔진 초기화
	Physics::GetInstance()->Initialize();

	/// 다 초기화 되고 윈도우 만들기
	ShowWindow(m_hWnd, SW_SHOWNORMAL);
	UpdateWindow(m_hWnd);

	g_InitComplete = true;
}

GameProcess::~GameProcess()
{
	Sound::Destroy();
	DebugRenderManager::Get().Destroy();
	RenderManager::GetInstance().Destroy();
	Physics::Destroy();

	for (auto val : TypeInfo::ComponentTypes | std::views::values)
	{
		delete val;
	}
}

void GameProcess::Loop()
{
	MSG msg;

	while (true)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT) break;

			TranslateMessage(&msg);
			DispatchMessage(&msg);

		}
		else
		{
			Update();
			EventManager::Update(m_DeltaTime);

			RenderManager::GetInstance().BeginRender();
			Render();
			OnImGuiRender();

			RenderManager::GetInstance().Excute();
			RenderManager::GetInstance().EndRender();

		}
	}
}

void GameProcess::Update()
{
	m_Time->Update();
	m_DeltaTime = m_Time->GetDeltaTime();
	m_DeltaTime *= m_SpeedWeight;

//#ifdef _DEBUG
//	if (m_DeltaTime > 1.f / 60.f)
//		m_DeltaTime = 1.f / 60.f;
//#endif

	m_Input->Update();
	Sound::GetInstance()->Update();

	m_SystemManager->Update(m_DeltaTime);
	m_SystemManager->FixedUpdate(m_DeltaTime);
}

void GameProcess::Render()
{
	m_SystemManager->Render();
}

void GameProcess::OnImGuiRender()
{
}

void GameProcess::OnPlayButton(std::any data)
{
	m_SystemManager->Initialize();
}

void GameProcess::OnStopButton(std::any data)
{
	m_SystemManager->Finalize();
}

void GameProcess::OnStart(std::any data)
{
	m_SystemManager->Start(std::any_cast<uint32_t>(data));
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
		return 0;

	switch (message)
	{
	case WM_SIZE:
	{
		if (!g_InitComplete)
			break;

		auto x = LOWORD(lParam);
		auto y = HIWORD(lParam);

		x < 16 ? x = 16 : x;
		y < 9 ? y = 9 : y;

		RenderManager::GetInstance().OnResize(x, y);
		break;
	}
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

void GameProcess::OnLoadScene(std::any data)
{
	auto sceneName = std::any_cast<std::string>(data);
	SerializeSystem sSys{ m_GameObjects.get() };
	string path = "../Data/Scene/" + sceneName;
	if (sSys.DeserializeScene(nullptr, path))
	{
		m_SystemManager->Initialize();
	}
	else
	{
		return;
	}
}

void GameProcess::OnSetSpeedWeight(std::any data)
{
	float weight = std::any_cast<float>(data);

	m_SpeedWeight = weight;
}

void GameProcess::OnDestroy(std::any data)
{
	auto id = std::any_cast<uint32_t>(data);
	m_SystemManager->Finish(id);
	m_GameObjects->OnReleaseGameObject(id);
}