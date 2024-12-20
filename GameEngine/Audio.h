#pragma once
#include "Component.h"

struct AudioSource
{
	GENERATE_CLASS_TYPE_INFO(AudioSource)
	PROPERTY(AudioName)
	PROPERTY(Volume)
	PROPERTY(IsLoop)
	NLOHMANN_DEFINE_TYPE_INTRUSIVE(AudioSource, AudioName, Volume, IsLoop)

public:
	std::string AudioName = ".mp3";
	float Volume = 1.0f;
	bool IsPlaying = false;
	bool IsLoop = false;
};

struct Audio : public Component
{
public:
	GENERATE_BODY(Audio)
	GENERATE_PROPERTY(Audio, Source1)

	AudioSource Source1;
};





