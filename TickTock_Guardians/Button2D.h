#pragma once
#include <Component.h>
struct Button2D : public Component
{
public:
	GENERATE_BODY(Button2D)
	GENERATE_PROPERTY(Button2D, ButtonEvent, UserData)

	std::string ButtonEvent;
	std::string UserData;

// ���� ������
	bool bIsClicked;
};

