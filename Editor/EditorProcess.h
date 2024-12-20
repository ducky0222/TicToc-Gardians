#pragma once
#include "TickTockGuardians.h"

class IWindow;

class EditorProcess : public TickTockGuardians
{
public:
	EditorProcess(HINSTANCE hInst, const std::string_view& title, int width, int height);
	~EditorProcess() override;

private:
	void TweakDarculaStyle();

	void Update() override;
	void Render() override;
	void OnImGuiRender() override;
	
	std::vector<std::unique_ptr<IWindow>> m_Windows;
};
