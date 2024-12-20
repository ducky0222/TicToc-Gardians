#include "../TickTock_Guardians/pch.h"
#include "LauncherTTG.h"

#include "../ABGraphics/RenderManager.h"
#include "../GameEngine/SerializeSystem.h"

LauncherTTG::LauncherTTG(HINSTANCE hInst)
	: TickTockGuardians(hInst, "TICTOC GUARDIANS",1920, 1080, false)
{
	EventManager::OnLoadScene("TitleHong.scene");
	RenderManager::GetInstance().SetEditorViewPort({ 1920, 1080 }, { 0, 0 });
}
