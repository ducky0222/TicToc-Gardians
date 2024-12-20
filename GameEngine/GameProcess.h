#pragma once
#include "EventListener.h"

class Graphics;
class Renderer;
class TimeManager;
class ComponentPoolManager;
class GameObjectManager;
class SystemManager;
class Input;

// 테스트
struct Transform;

class GameProcess : public EventListener
{
public:
	GameProcess(HINSTANCE hInst, std::string_view title, int width, int height, bool bImGuiInit);
	virtual ~GameProcess();

	void Loop();


protected:
	virtual void Update();
	virtual void Render();


	// Test
	virtual void OnImGuiRender();


	void OnPlayButton(std::any data);
	void OnStopButton(std::any data);
	void OnStart(std::any data);
	void OnLoadScene(std::any data);
	void OnSetSpeedWeight(std::any data);
	

	void OnDestroy(std::any data);
	const std::string m_Title;
	HWND m_hWnd = nullptr;
	bool m_bImGuiRender = true;
	float m_SpeedWeight = 1.0f;
	float m_DeltaTime = 0.f;

	// system
	//std::unique_ptr<Graphics> m_Graphics;
	std::unique_ptr<TimeManager> m_Time;
	std::unique_ptr<ComponentPoolManager> m_ComponentPools;
	std::unique_ptr<GameObjectManager> m_GameObjects;

	std::unique_ptr<SystemManager> m_SystemManager;

	Input* m_Input = nullptr;
};
