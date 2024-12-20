#pragma once
#include "Component.h"

enum class TagType
{
	Cat,			// 고양이 캐릭터
	Rabbit,			// 토끼 캐릭터
	Beaver,			// 비버 캐릭터

	LandingPoint,	// 시작점
	DimensionCrack,	// 차원의 틈
	Box,			// 박스
	Button,			// 버튼
	Lift,			// 리프트
	ConveyorBelt,	// 컨베이어 벨트
	BoxGenerator,	// 박스 생성기

	SelectCat,		// 선택창 고양이 캐릭터
	SelectRabbit,	// 선택창 토끼 캐릭터
	SelectBeaver,	// 선택창 비버 캐릭터
	BoxUI_LeftDown,
	BoxUI_LeftUp,
	BoxUI_RightDown,
	BoxUI_RightUp,
	Transparent_Wall,
	BoxUI_Landing
	
};

struct Tag : public Component
{
	GENERATE_BODY(Tag)
	GENERATE_PROPERTY(Tag, tag)
			
	TagType tag;
};
