#pragma once
#include "Component.h"

// UI 2D�뵵
class TransformAnim2D : public Component
{
	GENERATE_BODY(TransformAnim2D)
		GENERATE_PROPERTY(TransformAnim2D,
			bIsActive, bIsReset, bIsRecursive, ProcessingTime,
			InitPosition, InitSize, InitRotation,
			EndPosition, EndSize, EndRotation)

public:
	bool bIsActive{ false }; // Ȱ��ȭ ����
	bool bIsReset{ false }; // TimePos �ʱ�ȭ �뵵
	bool bIsRecursive{ false }; // �ݺ� ����
	float ProcessingTime{ 1.f }; // ó�� �䱸 �ð�

	VSM::Vector2 InitSize;
	float InitRotation;
	VSM::Vector2 InitPosition; // ���� ���� ������

	VSM::Vector2 EndSize;
	float EndRotation;
	VSM::Vector2 EndPosition; // ���� ���� ������

	// ���� ������
	float TimePos;
};

// �׿��� ��� ������Ʈ �뵵
class TransformAnim3D : public Component
{
	GENERATE_BODY(TransformAnim3D)
		GENERATE_PROPERTY(TransformAnim3D,
			bIsActive, bIsReset, bIsRecursive, ProcessingTime,
			InitPosition, InitSize, InitRotation,
			EndPosition, EndSize, EndRotation)

public:
	bool bIsActive{ false }; // Ȱ��ȭ ����
	bool bIsReset{ false }; // TimePos �ʱ�ȭ �뵵
	bool bIsRecursive{ false }; // �ݺ� ����
	float ProcessingTime{ 1.f }; // ó�� �䱸 �ð�

	VSM::Vector3 InitSize;
	VSM::Quaternion InitRotation;
	VSM::Vector3 InitPosition; // ���� ���� ������

	VSM::Vector3 EndSize;
	VSM::Quaternion EndRotation;
	VSM::Vector3 EndPosition; // ���� ���� ������

	// ���� ������
	float TimePos;
};


// UI 2D�뵵
class NeoTransformAnim2D : public Component
{
	GENERATE_BODY(NeoTransformAnim2D)
		GENERATE_PROPERTY(NeoTransformAnim2D,
			bIsActive, bIsReset, bIsRecursive, bIsReverse, ProcessingTime,
			InitPosition, InitSize, InitRotation,
			EndPosition, EndSize, EndRotation)

public:
	bool bIsActive{ false }; // Ȱ��ȭ ����
	bool bIsReset{ false }; // TimePos �ʱ�ȭ �뵵
	bool bIsRecursive{ false }; // �ݺ� ����
	bool bIsReverse{ false }; // �Ųٷ� �ִϸ��̼�
	float ProcessingTime{ 1.f }; // ó�� �䱸 �ð�

	VSM::Vector2 InitSize;
	float InitRotation;
	VSM::Vector2 InitPosition; // ���� ���� ������

	VSM::Vector2 EndSize;
	float EndRotation;
	VSM::Vector2 EndPosition; // ���� ���� ������

	// ���� ������
	float TimePos;
};

// �׿��� ��� ������Ʈ �뵵
class NeoTransformAnim3D : public Component
{
	GENERATE_BODY(NeoTransformAnim3D)
		GENERATE_PROPERTY(NeoTransformAnim3D,
			bIsActive, bIsReset, bIsRecursive, bIsReverse, ProcessingTime,
			InitPosition, InitSize, InitRotation,
			EndPosition, EndSize, EndRotation)

public:
	bool bIsActive{ false }; // Ȱ��ȭ ����
	bool bIsReset{ false }; // TimePos �ʱ�ȭ �뵵
	bool bIsRecursive{ false }; // �ݺ� ����
	bool bIsReverse{ false }; // �Ųٷ� �ִϸ��̼�
	float ProcessingTime{ 1.f }; // ó�� �䱸 �ð�

	VSM::Vector3 InitSize;
	VSM::Quaternion InitRotation;
	VSM::Vector3 InitPosition; // ���� ���� ������

	VSM::Vector3 EndSize;
	VSM::Quaternion EndRotation;
	VSM::Vector3 EndPosition; // ���� ���� ������

	// ���� ������
	float TimePos;
};

