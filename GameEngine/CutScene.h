#pragma once
#include "Component.h"

class CutSceneClip : public Component
{
	GENERATE_BODY(CutSceneClip)
		GENERATE_PROPERTY(CutSceneClip,
			Key, FrameCount, DirectoryPath)

public:
	std::string Key; // 로딩에 사용할 키 데이터
	unsigned int FrameCount; // 데이터 로딩 개수
	std::string DirectoryPath;
};

class CutSceneController : public Component
{
	GENERATE_BODY(CutSceneController)
		GENERATE_PROPERTY(CutSceneController,
			bIsActive, bIsReset, bIsRecursive, FrameInterval, FrameIndex, FrameCount)

public:
	bool bIsActive{ false }; // 활성화 여부
	bool bIsReset{ false }; // TimePos 초기화 용도
	bool bIsRecursive{ false }; // 반복 여부
	float FrameInterval{ 0.2f }; // 한 장당 처리 시간
	unsigned int FrameIndex{ 0u }; // 현재 프레임 인덱스
	unsigned int FrameCount{ 1u }; // 처리할 개수

	// 로직 데이터
	float TimePos;
};
