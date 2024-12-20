#pragma once
#include <Component.h>
// 2D���� 3D���� ���� �� �ִ�.!
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

	// ���� ������
	bool bIsClicked{ false }; // ���콺 �ٿ��� �����س���, ���콺 �½ÿ� bIsClicked�� Ŭ���� ���ݾ�
	bool bIsOvered{ false };
};
