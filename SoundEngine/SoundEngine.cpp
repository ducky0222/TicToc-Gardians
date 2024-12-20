
#include "SoundEngine.h"

#include <ranges>


SoundEngine::SoundEngine()
{

}

SoundEngine::~SoundEngine()
{

}

void SoundEngine::Initialize()
{
	// 파일 패스 먼저 설정해주기
	//TODO 이거 프로젝트 관리해서 디버그 폴더로 옮기기
	m_SoundPath = std::filesystem::current_path().parent_path().string();
	m_SoundPath += "/Resources/Sound/";

	// fmod 시스템 관련 초기화
	FMOD::System_Create(&m_System);

	m_System->getVersion(&m_Version);

	m_System->init(8, FMOD_INIT_NORMAL, m_ExtraDriverData);

	// 디스턴스 유닛 설정
	m_System->set3DSettings(1.0f, DISTANCE_FACTOR, 1.0f);



	m_MasterVolume = 1.0f;
}

void SoundEngine::Update()
{
	m_System->update();
}

void SoundEngine::Load2DSound(const std::string& path, const std::string& key, SoundType type)
{
	if (m_SoundMap.find(key) != m_SoundMap.end())
		return;

	FMOD::Sound* sound = nullptr;

	switch (type)
	{
	case SoundType::BGM:
		m_System->createStream(path.c_str(), FMOD_LOOP_NORMAL, nullptr, &sound);
		break;
	case SoundType::EFFECT:
		m_System->createSound(path.c_str(), FMOD_DEFAULT, nullptr, &sound);
		break;
	}

	m_SoundMap.insert(std::make_pair(key, sound));
}

void SoundEngine::Load3DSound(const std::string& path, const std::string& key, SoundType type)
{
	if (m_SoundMap.find(key) != m_SoundMap.end())
		return;

	FMOD::Sound* sound = nullptr;

	std::string fullPath = m_SoundPath + path;

	/// TODO : 이것도 모드 바꿔서 들고올수 있는데 고민하기
	switch (type)
	{
	case SoundType::BGM:
		m_System->createStream(fullPath.c_str(), FMOD_LOOP_NORMAL | FMOD_3D, nullptr, &sound);
		break;
	case SoundType::EFFECT:
		m_System->createSound(fullPath.c_str(), FMOD_DEFAULT | FMOD_3D, nullptr, &sound);
		break;
	}

	m_SoundMap.insert(std::make_pair(key, sound));

}

void SoundEngine::SetSoundMinMaxDistance(const std::string& key, float min, float max)
{
	if (m_SoundMap.find(key) == m_SoundMap.end())
		return;

	m_SoundMap.find(key)->second->set3DMinMaxDistance(min * DISTANCE_FACTOR, max * DISTANCE_FACTOR);
}

void SoundEngine::Play(const uint32_t& id, const std::string& key, float volume)
{
	// 사운드 맵에서 사운드를 찾습니다.
	auto soundIter = m_SoundMap.find(key);
	if (soundIter == m_SoundMap.end()) 
		return; 

	FMOD::Sound* sound = soundIter->second;

	// 해당 게임 오브젝트의 채널 목록을 검사합니다.
	auto& channels = m_GameObjectChannels[id];
	for (auto it = channels.begin(); it != channels.end(); ++it) {
		FMOD::Channel* channel = it->first;
		const std::string& currentKey = it->second;

		// 현재 채널이 재생하는 사운드가 들어온 사운드 key와 일치하면 정지합니다.
		if (currentKey == key) {
			channel->stop();
			it = channels.erase(it); // 해당 채널을 목록에서 제거합니다.
			break;
		}
	}

	// 해당 사운드의 반복 여부를 확인합니다.
	FMOD_MODE mode;
	if (sound->getMode(&mode) != FMOD_OK)
		return;

	// 새로운 채널을 생성하고 사운드를 재생합니다.
	FMOD::Channel* channel = nullptr;
	m_System->playSound(sound, nullptr, true, &channel);

	if ((mode & FMOD_LOOP_NORMAL) != 0) {
		channel->setMode(FMOD_LOOP_NORMAL); // 반복 모드 설정
	}
	else {
		channel->setMode(FMOD_LOOP_OFF); // 반복 없음
	}

	// 볼륨 설정
	channel->setVolume(volume);

	// 새로운 채널과 사운드 key를 게임 오브젝트의 채널 목록에 추가합니다.
	channels.push_back({ channel, key });

	channel->setPaused(false); // 재생을 시작합니다.
}

void SoundEngine::Stop(const uint32_t& id, const std::string& soundKey)
{
	auto channelsIter = m_GameObjectChannels.find(id);
	if (channelsIter != m_GameObjectChannels.end()) {
		auto& channels = channelsIter->second;
		for (auto channelIter = channels.begin(); channelIter != channels.end();) {
			FMOD::Channel* channel = channelIter->first;
			std::string& key = channelIter->second;

			// 해당 사운드 키에 대한 채널만 정지시킵니다.
			if (key == soundKey && channel != nullptr) {
				channel->stop();
				// 채널을 벡터에서 제거
				channelIter = channels.erase(channelIter);
			}
			else {
				++channelIter;
			}
		}

		// 모든 채널이 제거되었다면, 해당 오브젝트 ID도 맵에서 제거합니다.
		if (channels.empty()) {
			m_GameObjectChannels.erase(channelsIter);
		}
	}
}


void SoundEngine::Set3DAttributes(const uint32_t& id, float* pos, float* vel)
{
	auto channelIter = m_ChannelMap.find(id);
	if (channelIter == m_ChannelMap.end())
		return;
	//channel[0]->set3DAttributes(reinterpret_cast<FMOD_VECTOR*>(pos), reinterpret_cast<FMOD_VECTOR*>(vel));
	FMOD_VECTOR posVec = { pos[0], pos[1], pos[2] };
	FMOD_VECTOR velVec = { 0, 0, 0 };
	channelIter->second->set3DAttributes(&posVec, &velVec);
	channelIter->second->get3DAttributes(&posVec, &velVec);
}

void SoundEngine::SetListenerPosition(float* pos, float* vel)
{
	m_ListenerPos = { pos[0], pos[1], pos[2] };
	// 임시
	FMOD_VECTOR velVec = { 0, 0, 0 };
	//m_ListenerVel = { vel[0], vel[1], vel[2] };

	m_System->set3DListenerAttributes(0, &m_ListenerPos, &velVec, &m_ListenerForward, &m_ListenerUp);
}

void SoundEngine::CleanChannel()
{
	for (auto& gameObjectPair : m_GameObjectChannels) {
		auto& channels = gameObjectPair.second;
		for (auto& channelPair : channels) {
			FMOD::Channel* channel = channelPair.first;
			if (channel != nullptr) {
				channel->stop();
			}
		}
		channels.clear(); // 각 게임 오브젝트에 대한 채널 목록을 비웁니다.
	}
	m_GameObjectChannels.clear(); // 전체 맵을 비웁니다.;
}
