#pragma once

#include "ISoundEngine.h"
#include <unordered_map>
#include <filesystem>
#include "fmod/fmod.hpp"


/// <summary>
/// ���� ������ ������
/// </summary>
class SoundEngine : public ISoundEngine
{
public:
	SoundEngine();
	virtual ~SoundEngine() override;

	void Initialize() override;

	void Update() override;

	void Load2DSound(const std::string& path, const std::string& key, SoundType type) override;

	void Load3DSound(const std::string& path, const std::string& key, SoundType type) override;

	void SetSoundMinMaxDistance(const std::string& key, float min, float max) override;

	void Play(const uint32_t& id, const std::string& key, float volume = 1.0f) override;

	void Stop(const uint32_t& id, const std::string& soundKey) override;

	void Set3DAttributes(const uint32_t& id, float* pos, float* vel) override;

	void SetListenerPosition(float* pos, float* vel) override;

	void CleanChannel() override;

private:
	constexpr static float DISTANCE_FACTOR = 1.0f;

	// ���� �ý��� Ȱ���ؼ� ���� ��ġ �˾ƿ���
	ISoundEngine* m_Instance;

	std::string m_SoundPath;

	FMOD::System* m_System;


	std::unordered_map<std::string, FMOD::Sound*> m_SoundMap;

	std::unordered_map<uint32_t, FMOD::Channel*> m_ChannelMap;
	std::unordered_map<uint32_t, std::vector<std::pair<FMOD::Channel*, std::string>>> m_GameObjectChannels;

	unsigned int m_Version;

	void* m_ExtraDriverData;

	float m_MasterVolume;


	/// ������ ������
	FMOD_VECTOR m_ListenerPos = { 0.0f, 0.0f, 0.0f };

	/// ������ �ӵ�
	FMOD_VECTOR m_ListenerVel = { 0.0f, 0.0f, 0.0f };


	FMOD_VECTOR m_ListenerForward = { 0.0f, 0.0f, 1.0f };
	FMOD_VECTOR m_ListenerUp = { 0.0f, 1.0f, 0.0f };


};

