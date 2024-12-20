#pragma once
#include "Component.h"

class SpriteEffect : public Component
{
	GENERATE_BODY(SpriteEffect)
		GENERATE_PROPERTY(SpriteEffect,
			KeyName0, EffectPrefabName0, Offset0, RandomOffset0, Count0, EffectDuration0, RandomEffectDuration0, RandomEffectStart0,
			KeyName1, EffectPrefabName1, Offset1, RandomOffset1, Count1, EffectDuration1, RandomEffectDuration1, RandomEffectStart1,
			KeyName2, EffectPrefabName2, Offset2, RandomOffset2, Count2, EffectDuration2, RandomEffectDuration2, RandomEffectStart2)

public:
	// 에디터용 데이터
	std::string KeyName0;
	std::string EffectPrefabName0{ ".prefab" };
	VSM::Vector3 Offset0;
	VSM::Vector3 RandomOffset0{ 0.001f, 0.001f, 0.001f };
	unsigned int Count0{ 1 };
	float EffectDuration0{ 2.f };
	float RandomEffectDuration0{ 0.001f };
	float RandomEffectStart0{ 0.001f };

	std::string KeyName1;
	std::string EffectPrefabName1{ ".prefab" };
	VSM::Vector3 Offset1;
	VSM::Vector3 RandomOffset1{ 0.001f, 0.001f, 0.001f };
	unsigned int Count1{ 1 };
	float EffectDuration1{ 2.f };
	float RandomEffectDuration1{ 0.001f };
	float RandomEffectStart1{ 0.001f };

	std::string KeyName2;
	std::string EffectPrefabName2{ ".prefab" };
	VSM::Vector3 Offset2;
	VSM::Vector3 RandomOffset2{ 0.001f, 0.001f, 0.001f };
	unsigned int Count2{ 1 };
	float EffectDuration2{ 2.f };
	float RandomEffectDuration2{ 0.001f };
	float RandomEffectStart2{ 0.001f };

	// 로직 데이터
	bool bIsInit{ false };

	struct SpriteEffectInfo
	{
		std::string* EffectPrefabName;
		VSM::Vector3* Offset;
		VSM::Vector3* RandomOffset;
		unsigned int* Count;
		float* EffectDuration;
		float* RandomEffectDuration;
		float* RandRandomEffectStart;
	};

	std::map<std::string, SpriteEffectInfo> SpriteEffectInfos;
};

