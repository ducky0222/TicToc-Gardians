#pragma once

#include "Component.h"

struct ClipInformation
{
	GENERATE_CLASS_TYPE_INFO(ClipInformation)
		PROPERTY(FileName)
		PROPERTY(SpriteEffectKey)
		PROPERTY(FrameInterval)
		PROPERTY(bIsRecursive)
		PROPERTY(SoundKey)
		NLOHMANN_DEFINE_TYPE_INTRUSIVE(ClipInformation, FileName, SpriteEffectKey, FrameInterval, bIsRecursive, SoundKey)

public:
	std::string FileName{ ".fbx" };
	std::string SpriteEffectKey;
	unsigned int FrameInterval{ 7u };
	bool bIsRecursive{ false };
	std::string SoundKey;
};


class NeoNeoAnimationClip : public Component
{
	GENERATE_BODY(NeoNeoAnimationClip)
	GENERATE_PROPERTY(NeoNeoAnimationClip, bOnFrameEnter, bOnFrame, bOnFrameExit,
			Clip0, Clip1, Clip2, Clip3,
			Clip4, Clip5, Clip6, Clip7, Clip8)

	bool bOnFrameEnter{ false };
	bool bOnFrame{ false };
	bool bOnFrameExit{ false };

	ClipInformation Clip0;
	ClipInformation Clip1;
	ClipInformation Clip2;
	ClipInformation Clip3;
	ClipInformation Clip4;
	ClipInformation Clip5;
	ClipInformation Clip6;
	ClipInformation Clip7;
	ClipInformation Clip8;

	// 로직 데이터
	std::map<std::string, ClipInformation*> ClipInfos;
};

