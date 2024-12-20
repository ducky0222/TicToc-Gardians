#pragma once
#include "Component.h"

class CutSceneClip : public Component
{
	GENERATE_BODY(CutSceneClip)
		GENERATE_PROPERTY(CutSceneClip,
			Key, FrameCount, DirectoryPath)

public:
	std::string Key; // �ε��� ����� Ű ������
	unsigned int FrameCount; // ������ �ε� ����
	std::string DirectoryPath;
};

class CutSceneController : public Component
{
	GENERATE_BODY(CutSceneController)
		GENERATE_PROPERTY(CutSceneController,
			bIsActive, bIsReset, bIsRecursive, FrameInterval, FrameIndex, FrameCount)

public:
	bool bIsActive{ false }; // Ȱ��ȭ ����
	bool bIsReset{ false }; // TimePos �ʱ�ȭ �뵵
	bool bIsRecursive{ false }; // �ݺ� ����
	float FrameInterval{ 0.2f }; // �� ��� ó�� �ð�
	unsigned int FrameIndex{ 0u }; // ���� ������ �ε���
	unsigned int FrameCount{ 1u }; // ó���� ����

	// ���� ������
	float TimePos;
};
