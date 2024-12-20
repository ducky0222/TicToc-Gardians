#pragma once

#include "Component.h"
#include "NeoNeoAnimationClip.h"

class NeoAnimationClip : public Component
{
	GENERATE_BODY(NeoAnimationClip)
	GENERATE_PROPERTY(NeoAnimationClip, bOnFrameEnter, bOnFrame, bOnFrameExit,
		Clip0,Clip1,Clip2,Clip3,
		Clip4,Clip5,Clip6,Clip7)

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

	// 로직 데이터
	std::map<std::string, ClipInformation*> ClipInfos;
};

