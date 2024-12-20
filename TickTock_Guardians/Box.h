#pragma once

#include "Component.h"

class Box : public Component
{
	GENERATE_BODY(Box)
	GENERATE_PROPERTY(Box, MoveLength, GeneratingTime, ReadyToPush,ConveyorBeltTime, Pushtime, IsOrigin)

	uint32_t NewBoxTarget;
	uint32_t SavedTarget;

	// Reset Data
	bool IsOrigin = false;								// �ʱ� ���� ����
	VSM::Vector3 OriginPosition = VSM::Vector3::Zero;	// �ʱ� ��ġ

	/// �ݶ��̴� Init���� �� bool��
	bool OnMoveWalk;
	/// �����̾� ��Ʈ �̵� �� bool��
	bool IsMoving;
	/// �����̾� ��Ʈ �̵��ð�
	float ConveyorBeltTime;
	/// �ݶ��̴� Init���� �� bool��
	bool OnPushed;
	/// �б� �̵� �� bool��
	bool IsPushing;
	/// Generator�� ���� ����Ǿ��� ��.
	bool OnGenerated = false;
	bool IsGenerating = false;

	bool ButtonTriggerOn = false;
	bool LandingSpriteOn = false;
	/// �̴� �̵��ð�
	float Pushtime;
	/// �б� ���� ���ð�
	float ReadyToPush;
	///��ȯ�ϴµ� �ɸ��� �ð�.
	float GeneratingTime;

	/// ������ �Ÿ�
	float MoveLength;

	/// deltaTime+ �ϴ� �뵵
	float FlowingTime;

	/// ������
	VSM::Vector3 ArrivalPosition;
	/// ������
	VSM::Vector3 DeparturePosition;
};

