#pragma once
#pragma once

#include "Processor.h"
#include "../ABGraphics/Camera.h"
#include "../ABGraphics/Animation.h"
#include "../ABGraphics/RenderManager.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

struct ModelInstance
{
	eModelType ModelType;
	std::string Key;
	Matrix Transform;
	bool bUseAlpha;
	float Alpha;
	std::string AnimKey;
	std::string ClipKey;
	float TimePos;
};

namespace entryPoint
{
	using namespace common;

	class Sample final : public Processor
	{
	public:
		Sample(HINSTANCE hInstance, UINT width, UINT height, std::wstring name);
		virtual ~Sample() override;

		bool Init() override;
		void Update(float deltaTime) override;
		void Render() override;

		virtual LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
		{
			if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam))
			{
				return true;
			}

			return Processor::MsgProc(hwnd, msg, wParam, lParam);
		}

	private:
		void camMove(float deltaTime);
		void setShadowTransform();
		void initGeometry();

	private:
		enum { TILE_WIDTH = 5};
		enum { TILE_DEPTH = 5};
		enum { TILE_HEIGHT = 8 };

		std::vector<ModelInstance> mModelInstance;
		std::unique_ptr<SpriteAnimation> mSpriteAnim;
	};
}