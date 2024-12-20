#pragma once
#include "Audio.h"
#include "../TickTock_Guardians/TickTockGuardians.h"

class LauncherTTG : public TickTockGuardians
{
public:
	LauncherTTG(HINSTANCE hInst);
	~LauncherTTG() override = default;
};
