#pragma once

#include "../GameEngine/GameProcess.h"

class Renderer;
class IPhysicsEngine;
class Camera;

class Process final : public GameProcess
{
public:
	Process(HINSTANCE hInst, std::string_view title, int width, int height, bool bImGuiInit);
	virtual ~Process() override;

	virtual void Update() override;
	virtual void Render(Renderer* renderer) override;

	virtual void OnImGuiRender() override;

private:
	Camera* m_camera = nullptr;

};

