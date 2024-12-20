#pragma once
#include "imgui.h"

class IWindow
{
public:
	IWindow() = default;
	virtual ~IWindow() = default;

	virtual void OnImGuiRender() = 0;
};
