#pragma once

#include "Processor.h"
#include "../ABGraphics/Camera.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

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
		enum { TILE_WIDTH = 2 };
		enum { TILE_DEPTH = 2 };
		enum { TILE_SIZE = 40 };

		::Camera* mCamera{};
		int asd{};
		char asdz{};
		string asdf{};

		/// 화면으로 본 상하좌우
		Vector3 m_screenFront{};
		Vector3 m_screenBack{};
		Vector3 m_screenRight{};
		Vector3 m_screenLeft{};
	};
}