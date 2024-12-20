#pragma once
#include "Component.h"

// �� �ý��ۿ��� Ư�� ��ƼƼ ������ ���� ���� ������Ʈ
// �� �ȿ��� EntityUUID�� �ߺ��� ��� ����

class EntityIdentity : public Component
{
	GENERATE_BODY(EntityIdentity)
	GENERATE_PROPERTY(EntityIdentity, EntityUUID)

public:
	std::string EntityUUID;
};

