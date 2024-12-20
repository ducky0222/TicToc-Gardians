#include "pch.h"
#include "TimeManager.h"

TimeManager::TimeManager()
{
	// 현재 카운트
	QueryPerformanceCounter(&m_PrevCount);

	// 초당 카운트 횟수
	QueryPerformanceFrequency(&m_Frequency);
}

void TimeManager::Update()
{
	QueryPerformanceCounter(&m_CurCount);
	m_DeltaTime = static_cast<double>(m_CurCount.QuadPart - m_PrevCount.QuadPart) / static_cast<double>(m_Frequency.QuadPart);
	m_PrevCount = m_CurCount;
	   
	m_FPS++;

	if (m_ElapsedTime > 1.0) {
		m_ElapsedTime -= 1.0;
		m_LastFPS = m_FPS;
		m_FPS = 0;

		HWND hwnd = GetForegroundWindow();
		if (hwnd) {
			// FPS를 현재 활성 윈도우의 타이틀바에 출력
			std::wstring title = L"FPS: " + std::to_wstring(m_LastFPS);
			auto tt = SetWindowText(hwnd, title.c_str());
		}
	}
}
