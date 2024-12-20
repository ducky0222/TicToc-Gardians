#pragma once
#include <string>




/// <summary>
/// 사운드 엔진의 인터페이스
/// </summary>
class ISoundEngine
{
public:
	enum class SoundType
	{
		BGM, // 이건 루프
		EFFECT, // 이건 루프 아님
	};

public:
	virtual ~ISoundEngine() = default;

	virtual void Initialize() = 0;
	virtual void Update() = 0;

	virtual void Load2DSound(const std::string& path, const std::string& key, SoundType type) = 0;

	virtual void Load3DSound(const std::string& path, const std::string& key, SoundType type) = 0;

	virtual void SetSoundMinMaxDistance(const std::string& key, float min, float max) = 0;

	virtual void Play(const uint32_t& id, const std::string& key, float volume = 1.0f) = 0;

	virtual void Stop(const uint32_t& id, const std::string& soundKey) = 0;

/// 이거 알고보니 채널별이다. 채널별로 변경해야할듯
	virtual void Set3DAttributes(const uint32_t& id, float* pos, float* vel) = 0;

	virtual void SetListenerPosition(float* pos, float* vel) = 0;

	virtual void CleanChannel() = 0;
};

namespace Sound
{
	ISoundEngine* GetInstance();
	void Destroy();
}