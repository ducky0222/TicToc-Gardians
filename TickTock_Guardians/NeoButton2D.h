#pragma once
#include <Component.h>
// 2D지만 3D에도 붙일 수 있다.!
struct NeoButton2D : public Component
{
public:
	GENERATE_BODY(NeoButton2D)
		GENERATE_PROPERTY(NeoButton2D,
			ButtonClickEvent, ClickUserData,
			ButtonOverEvent, OverUserData,
			ButtonOutEvent, OutUserData)

public:
	std::string ButtonClickEvent;
	std::string ClickUserData;

	std::string ButtonOverEvent;
	std::string OverUserData;

	std::string ButtonOutEvent;
	std::string OutUserData;

	// 로직 데이터
	bool bIsClicked{ false }; // 마우스 다운을 저장해놓고, 마우스 온시에 bIsClicked면 클릭된 거잖아
	bool bIsOvered{ false };
};
