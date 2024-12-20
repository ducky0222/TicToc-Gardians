#pragma once
#include "Component.h"

#include <bitset>

struct Replayer : public Component
{
	GENERATE_BODY(Replayer)
	GENERATE_PROPERTY(Replayer, TraceSize, TraceIndex, ReplayIndex)

	bool IsTracing = false;
	bool IsReplaying = false;
	bool IsFinishReplaying = false;
	bool IsWaiting = false;

	VSM::Vector3 StartPosition = VSM::Vector3::Zero;
	VSM::Quaternion StartRotation = VSM::Quaternion::Identity;

	std::vector<std::bitset<8>> Tracer;
	uint32_t TraceSize = 0;
	uint32_t ReplayIndex = 0;
	uint32_t TraceIndex = 0;
};