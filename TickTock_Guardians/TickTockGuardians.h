#pragma once
#include <GameProcess.h>

#include "../ABGraphics/eModelType.h"

class TickTockGuardians : public GameProcess
{
public:
	TickTockGuardians(HINSTANCE hInst, std::string_view title, int width, int height, bool bImGuiInit);

	~TickTockGuardians();

	void Update() override;
private:

	void loadResource(eModelType modelType, std::filesystem::path searchFolder);

	void OnPause(std::any data);
	void OnResume(std::any data);
	void OnPlaySound(std::any data);

private:
	bool m_IsPause = false;
};
