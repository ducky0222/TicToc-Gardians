#pragma once
#include "Component.h"

// UI 2D용도
class TransformAnim2D : public Component
{
	GENERATE_BODY(TransformAnim2D)
		GENERATE_PROPERTY(TransformAnim2D,
			bIsActive, bIsReset, bIsRecursive, ProcessingTime,
			InitPosition, InitSize, InitRotation,
			EndPosition, EndSize, EndRotation)

public:
	bool bIsActive{ false }; // 활성화 여부
	bool bIsReset{ false }; // TimePos 초기화 용도
	bool bIsRecursive{ false }; // 반복 여부
	float ProcessingTime{ 1.f }; // 처리 요구 시간

	VSM::Vector2 InitSize;
	float InitRotation;
	VSM::Vector2 InitPosition; // 시작 정보 데이터

	VSM::Vector2 EndSize;
	float EndRotation;
	VSM::Vector2 EndPosition; // 종료 정보 데이터

	// 로직 데이터
	float TimePos;
};

// 그외의 모든 컴포넌트 용도
class TransformAnim3D : public Component
{
	GENERATE_BODY(TransformAnim3D)
		GENERATE_PROPERTY(TransformAnim3D,
			bIsActive, bIsReset, bIsRecursive, ProcessingTime,
			InitPosition, InitSize, InitRotation,
			EndPosition, EndSize, EndRotation)

public:
	bool bIsActive{ false }; // 활성화 여부
	bool bIsReset{ false }; // TimePos 초기화 용도
	bool bIsRecursive{ false }; // 반복 여부
	float ProcessingTime{ 1.f }; // 처리 요구 시간

	VSM::Vector3 InitSize;
	VSM::Quaternion InitRotation;
	VSM::Vector3 InitPosition; // 시작 정보 데이터

	VSM::Vector3 EndSize;
	VSM::Quaternion EndRotation;
	VSM::Vector3 EndPosition; // 종료 정보 데이터

	// 로직 데이터
	float TimePos;
};


// UI 2D용도
class NeoTransformAnim2D : public Component
{
	GENERATE_BODY(NeoTransformAnim2D)
		GENERATE_PROPERTY(NeoTransformAnim2D,
			bIsActive, bIsReset, bIsRecursive, bIsReverse, ProcessingTime,
			InitPosition, InitSize, InitRotation,
			EndPosition, EndSize, EndRotation)

public:
	bool bIsActive{ false }; // 활성화 여부
	bool bIsReset{ false }; // TimePos 초기화 용도
	bool bIsRecursive{ false }; // 반복 여부
	bool bIsReverse{ false }; // 거꾸로 애니메이션
	float ProcessingTime{ 1.f }; // 처리 요구 시간

	VSM::Vector2 InitSize;
	float InitRotation;
	VSM::Vector2 InitPosition; // 시작 정보 데이터

	VSM::Vector2 EndSize;
	float EndRotation;
	VSM::Vector2 EndPosition; // 종료 정보 데이터

	// 로직 데이터
	float TimePos;
};

// 그외의 모든 컴포넌트 용도
class NeoTransformAnim3D : public Component
{
	GENERATE_BODY(NeoTransformAnim3D)
		GENERATE_PROPERTY(NeoTransformAnim3D,
			bIsActive, bIsReset, bIsRecursive, bIsReverse, ProcessingTime,
			InitPosition, InitSize, InitRotation,
			EndPosition, EndSize, EndRotation)

public:
	bool bIsActive{ false }; // 활성화 여부
	bool bIsReset{ false }; // TimePos 초기화 용도
	bool bIsRecursive{ false }; // 반복 여부
	bool bIsReverse{ false }; // 거꾸로 애니메이션
	float ProcessingTime{ 1.f }; // 처리 요구 시간

	VSM::Vector3 InitSize;
	VSM::Quaternion InitRotation;
	VSM::Vector3 InitPosition; // 시작 정보 데이터

	VSM::Vector3 EndSize;
	VSM::Quaternion EndRotation;
	VSM::Vector3 EndPosition; // 종료 정보 데이터

	// 로직 데이터
	float TimePos;
};

