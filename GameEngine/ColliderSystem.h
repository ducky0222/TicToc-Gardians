#pragma once
#include "System.h"


/// <summary>
/// �ݶ��̴��� ��¶�� ���⼭ ���� ���ƾ��Ѵ�. 
/// onCollisionEnter � ���⼭ ���� �Ҽ� �����Ͱ���. 
/// </summary>

// ���� ȫ��ȯ ���� struct�� �Ǿ� ����
struct BoxCollider;

class ColliderSystem :
	public System, public IUpdatable
{
public:
	using System::System;

	void Update(float deltaTime) override;

private:
	bool IsCollide(const BoxCollider& bc1, const BoxCollider& bc2);
};

class ColliderEventSystem : public System, public IUpdatable
{
public:
	using System::System;

	void Update(float deltaTime) override;
};