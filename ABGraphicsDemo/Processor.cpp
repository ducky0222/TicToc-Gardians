#include "pch.h"

#include <WindowsX.h>
#include <sstream>

#include "Processor.h"
#include "../ABGraphics/RenderManager.h"

namespace common
{
	namespace
	{
		Processor* gd3dApp = 0;
	}

	LRESULT CALLBACK
		MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		return gd3dApp->MsgProc(hwnd, msg, wParam, lParam);
	}

	Processor::Processor(HINSTANCE hInstance, UINT width, UINT height, std::wstring name)
		: mhInstance(hInstance)
		, mhWnd(NULL)
		, mbPaused(false)
		, mbMinimized(false)
		, mbMaximized(false)
		, mbResizing(false)

		, mTitle(name)
		, mWidth(width)
		, mHeight(height)
		, mbEnable4xMsaa(false)
	{
		gd3dApp = this;
	}

	Processor::~Processor()
	{

	}

	int Processor::Run()
	{
		MSG msg = { 0 };

		mTimer.Reset();

		while (msg.message != WM_QUIT)
		{
			if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else
			{
				mTimer.Update();

				if (!mbPaused)
				{
					CalculateFrameStats();
					Update(mTimer.GetDeltaTime());
					Render();
				}
				else
				{
					Sleep(100);
				}
			}
		}

		return (int)msg.wParam;
	}

	bool Processor::Init()
	{
		// init Window
		{
			WNDCLASS wc;
			wc.style = CS_HREDRAW | CS_VREDRAW;
			wc.lpfnWndProc = MainWndProc;
			wc.cbClsExtra = 0;
			wc.cbWndExtra = 0;
			wc.hInstance = mhInstance;
			wc.hIcon = LoadIcon(0, IDI_APPLICATION);
			wc.hCursor = LoadCursor(0, IDC_ARROW);
			wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
			wc.lpszMenuName = 0;
			wc.lpszClassName = L"D3DWndClassName";

			if (!RegisterClass(&wc))
			{
				MessageBox(0, L"RegisterClass Failed.", 0, 0);
				return false;
			}

			RECT windowRect = { 0, 0, static_cast<LONG>(GetWidth()), static_cast<LONG>(GetHeight()) };
			AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, false);

			int width = windowRect.right - windowRect.left;
			int height = windowRect.bottom - windowRect.top;

			mhWnd = CreateWindow(
				L"D3DWndClassName",
				mTitle.c_str(),
				WS_OVERLAPPEDWINDOW,
				CW_USEDEFAULT,
				CW_USEDEFAULT,
				width, height,
				0, 0, mhInstance, 0);

			if (!mhWnd)
			{
				MessageBox(0, L"CreateWindow Failed.", 0, 0);
				return false;
			}

			ShowWindow(mhWnd, SW_SHOW);
			UpdateWindow(mhWnd);
		}

		return true;
	}

	LRESULT Processor::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		switch (msg)
		{
		case WM_ACTIVATE:
			if (LOWORD(wParam) == WA_INACTIVE)
			{
				mbPaused = true;
				mTimer.Pause();
			}
			else
			{
				mbPaused = false;
				mTimer.Start();
			}
			return 0;
		case WM_SIZE:
			mWidth = LOWORD(lParam);
			mHeight = HIWORD(lParam);
			return 0;

		case WM_ENTERSIZEMOVE:
			mbPaused = true;
			mbResizing = true;
			mTimer.Pause();
			return 0;
		case WM_EXITSIZEMOVE:
			mbPaused = false;
			mbResizing = false;
			mTimer.Start();
			return 0;
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
		case WM_MENUCHAR:
			return MAKELRESULT(0, MNC_CLOSE);
		case WM_GETMINMAXINFO:
			((MINMAXINFO*)lParam)->ptMinTrackSize.x = 200;
			((MINMAXINFO*)lParam)->ptMinTrackSize.y = 200;
			return 0;

		case WM_LBUTTONDOWN:
		case WM_MBUTTONDOWN:
		case WM_RBUTTONDOWN:
			OnMouseDown(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			return 0;
		case WM_LBUTTONUP:
		case WM_MBUTTONUP:
		case WM_RBUTTONUP:
			OnMouseUp(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			return 0;
		case WM_MOUSEMOVE:
			OnMouseMove(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			return 0;
		}

		return DefWindowProc(hwnd, msg, wParam, lParam);
	}

	void Processor::CalculateFrameStats()
	{
		// Code computes the average frames per second, and also the 
		// average time it takes to render one frame.  These stats 
		// are appended to the window caption bar.

		static int frameCnt = 0;
		static float timeElapsed = 0.0f;

		frameCnt++;

		// Compute averages over one second period.
		if ((mTimer.GetTotalTime() - timeElapsed) >= 1.0f)
		{
			float fps = (float)frameCnt; // fps = frameCnt / 1
			float mspf = 1000.0f / fps;

			std::wostringstream outs;
			outs.precision(6);
			outs << mTitle << L"    "
				<< L"FPS: " << fps << L"    "
				<< L"Frame Time: " << mspf << L" (ms)";
			SetWindowText(mhWnd, outs.str().c_str());

			// Reset for next average.
			frameCnt = 0;
			timeElapsed += 1.0f;
		}
	}
}