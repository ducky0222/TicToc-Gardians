#pragma once

#include "Timer.h"

namespace common
{
	class Processor
	{
	public:
		Processor(HINSTANCE hInstance, UINT width, UINT height, std::wstring name);
		virtual ~Processor();

		int Run();

		virtual bool Init();
		virtual void Update(float deltaTime) = 0;
		virtual void Render() = 0;

		virtual LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

		virtual void OnMouseDown(WPARAM btnState, int x, int y) { }
		virtual void OnMouseUp(WPARAM btnState, int x, int y) { }
		virtual void OnMouseMove(WPARAM btnState, int x, int y) { }

		inline HINSTANCE GetInstance() const;
		inline HWND GetHWnd() const;
		inline UINT GetWidth() const;
		inline UINT GetHeight() const;
		inline float GetAspectRatio() const;
		inline const WCHAR* GetTitle() const;

	protected:
		void CalculateFrameStats();

	protected:
		HINSTANCE mhInstance;
		HWND mhWnd;
		bool mbPaused;
		bool mbMinimized;
		bool mbMaximized;
		bool mbResizing;

		Timer mTimer;

		std::wstring mTitle;
		UINT mWidth;
		UINT mHeight;
		bool mbEnable4xMsaa;
	};

	HINSTANCE Processor::GetInstance() const
	{
		return mhInstance;
	}

	HWND Processor::GetHWnd() const
	{
		return mhWnd;
	}

	UINT Processor::GetWidth() const
	{
		return mWidth;
	}

	UINT Processor::GetHeight() const
	{
		return mHeight;
	}

	const WCHAR* Processor::GetTitle() const
	{
		return mTitle.c_str();
	}

	float Processor::GetAspectRatio() const
	{
		return mWidth / static_cast<float>(mHeight);
	}
}
