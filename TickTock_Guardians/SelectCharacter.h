#pragma once
#include "Component.h"

class SelectCharacter : public Component
{
	GENERATE_BODY(SelectCharacter)
		GENERATE_PROPERTY(SelectCharacter, bIsSelected, SelectIndex)

public:
	// 에디터 데이터
	bool bIsSelected{ false };
	int SelectIndex{ -1 };

	// 로직 데이터
	bool bIsClicked; // 3d피킹 클릭 추적용
};

