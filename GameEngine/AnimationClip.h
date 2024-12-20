#pragma once
#include "Component.h"

class AnimationClip : public Component
{
	GENERATE_BODY(AnimationClip)
		GENERATE_PROPERTY(AnimationClip
			, bOnFrameEnter, bOnFrame, bOnFrameExit
			, FileName0, SpriteEffectKey0, FrameInterval0, bIsRecursive0
			, FileName1, SpriteEffectKey1, FrameInterval1, bIsRecursive1
			, FileName2, SpriteEffectKey2, FrameInterval2, bIsRecursive2
			, FileName3, SpriteEffectKey3, FrameInterval3, bIsRecursive3
			, FileName4, SpriteEffectKey4, FrameInterval4, bIsRecursive4
			, FileName5, SpriteEffectKey5, FrameInterval5, bIsRecursive5)
		// 애니메이션 개수에 따라 이거 개수 늘어나야 함 6개 되니까 안되네

public:
	// 에디터 데이터
	bool bOnFrameEnter{ false };
	bool bOnFrame{ false };
	bool bOnFrameExit{ false };

	std::string FileName0{ ".fbx" };
	std::string SpriteEffectKey0;
	unsigned int FrameInterval0{ 7u };
	bool bIsRecursive0{ false };

	std::string FileName1{ ".fbx" };
	std::string SpriteEffectKey1;
	bool bIsRecursive1{ false };
	unsigned int FrameInterval1{ 1u };

	std::string FileName2{ ".fbx" };
	std::string SpriteEffectKey2;
	bool bIsRecursive2{ false };
	unsigned int FrameInterval2{ 1u };

	std::string FileName3{ ".fbx" };
	std::string SpriteEffectKey3;
	bool bIsRecursive3{ false };
	unsigned int FrameInterval3{ 1u };

	std::string FileName4{ ".fbx" };
	std::string SpriteEffectKey4;
	bool bIsRecursive4{ false };
	unsigned int FrameInterval4{ 1u };

	std::string FileName5{ ".fbx" };
	std::string SpriteEffectKey5;
	bool bIsRecursive5{ false };
	unsigned int FrameInterval5{ 1u };

	// 로직 데이터
	bool bIsInit{ false };

	struct ClipInfo
	{
		std::string* SpriteEffectKey;
		unsigned int* FrameInterval;
		bool* bIsRecursive;
	};

	std::map<std::string, ClipInfo> ClipInfos;
};