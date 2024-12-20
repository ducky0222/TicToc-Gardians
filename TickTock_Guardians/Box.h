#pragma once

#include "Component.h"

class Box : public Component
{
	GENERATE_BODY(Box)
	GENERATE_PROPERTY(Box, MoveLength, GeneratingTime, ReadyToPush,ConveyorBeltTime, Pushtime, IsOrigin)

	uint32_t NewBoxTarget;
	uint32_t SavedTarget;

	// Reset Data
	bool IsOrigin = false;								// 초기 존재 여부
	VSM::Vector3 OriginPosition = VSM::Vector3::Zero;	// 초기 위치

	/// 콜라이더 Init했을 때 bool값
	bool OnMoveWalk;
	/// 컨베이어 벨트 이동 중 bool값
	bool IsMoving;
	/// 컨베이어 벨트 이동시간
	float ConveyorBeltTime;
	/// 콜라이더 Init했을 때 bool값
	bool OnPushed;
	/// 밀기 이동 중 bool값
	bool IsPushing;
	/// Generator에 의해 실행되었을 때.
	bool OnGenerated = false;
	bool IsGenerating = false;

	bool ButtonTriggerOn = false;
	bool LandingSpriteOn = false;
	/// 미는 이동시간
	float Pushtime;
	/// 밀기 까지 대기시간
	float ReadyToPush;
	///소환하는데 걸리는 시간.
	float GeneratingTime;

	/// 움직일 거리
	float MoveLength;

	/// deltaTime+ 하는 용도
	float FlowingTime;

	/// 시작점
	VSM::Vector3 ArrivalPosition;
	/// 도착점
	VSM::Vector3 DeparturePosition;
};

