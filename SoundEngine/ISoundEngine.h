#pragma once
#include <string>




/// <summary>
/// ���� ������ �������̽�
/// </summary>
class ISoundEngine
{
public:
	enum class SoundType
	{
		BGM, // �̰� ����
		EFFECT, // �̰� ���� �ƴ�
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

/// �̰� �˰��� ä�κ��̴�. ä�κ��� �����ؾ��ҵ�
	virtual void Set3DAttributes(const uint32_t& id, float* pos, float* vel) = 0;

	virtual void SetListenerPosition(float* pos, float* vel) = 0;

	virtual void CleanChannel() = 0;
};

namespace Sound
{
	ISoundEngine* GetInstance();
	void Destroy();
}