#pragma once
#include <Component.h>

struct JumpParameter :
	public Component
{
	GENERATE_BODY(JumpParameter)
	GENERATE_PROPERTY(JumpParameter, JumpHeight, JumpTimeUp, GravityMultiplier)

	float JumpHeight;		// ������ (ĭ��)

	float JumpTimeUp;		// �����ð�
	float GravityMultiplier;		// �����ð�

	// ���� Ƚ���� �þ���� �ִ�.

};

