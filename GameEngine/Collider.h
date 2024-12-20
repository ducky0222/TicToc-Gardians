#pragma once
#include "Component.h"

/// <summary>
/// �ݶ��̴��� ��ӹ��� �� ����. ������Ʈ Ǯ T������ �ø�������� ���� ����
/// �̸��� ���� �ȵ�� ���߿� �ٲٵ��� ���� 
/// �ϴ� �ӽ� 
/// </summary>
struct BoxCollider : public Component
{
public:
	GENERATE_BODY(BoxCollider)

		// �߰� 
		VSM::Vector3 Center;
	// ũ�� ( �߰����� ������.) 
	VSM::Vector3 Size;

	VSM::Quaternion Orientation;

};

/// <summary>
/// ��� ��� �̺�Ʈ �Ŵ����� �ٸ� �ɷ� ����°� ���ƺ��δ�. �ٵ� �� ���� ������ �ȳ���. 
/// TODO : ���߿� �����غ���
/// </summary>
struct CollideEvent : public Component
{
public:
	GENERATE_BODY(CollideEvent)

	bool IsCollide = false;
	bool IsPrevCollide = false;

	// ���� �̺�Ʈ
	bool IsEnter = false;
	bool IsStay = false;
	bool IsExit = false;

	// �浹�� ������Ʈ
	uint32_t CollideObjectID;



};
