#pragma once
#include "Component.h"

class SelectCharacter : public Component
{
	GENERATE_BODY(SelectCharacter)
		GENERATE_PROPERTY(SelectCharacter, bIsSelected, SelectIndex)

public:
	// ������ ������
	bool bIsSelected{ false };
	int SelectIndex{ -1 };

	// ���� ������
	bool bIsClicked; // 3d��ŷ Ŭ�� ������
};

