#pragma once
#include "Component.h"

// 씬 시스템에서 특정 엔티티 참조를 갖기 위한 컴포넌트
// 씬 안에서 EntityUUID가 중복될 경우 터짐

class EntityIdentity : public Component
{
	GENERATE_BODY(EntityIdentity)
	GENERATE_PROPERTY(EntityIdentity, EntityUUID)

public:
	std::string EntityUUID;
};

