#include "pch.h"
#include "AudioSystem.h"

#include "Audio.h"
#include "ISoundEngine.h"

AudioSystem::AudioSystem(GameObjectManager* goManager)
	: System(goManager)
{
	EventManager::Subscribe("OnPlayAudio", CreateListenerInfo(&AudioSystem::OnPlayAudio));
}

void AudioSystem::Update(float deltaTime)
{
	Sound::GetInstance()->Update();
}

void AudioSystem::Initialize()
{
	for (Audio& audio : ComponentItr<Audio>())
	{
		LoadSoundBySource(audio.Source1);
	}
}

void AudioSystem::Start(uint32_t gameObjectId)
{
	auto audio = m_GameObjectManager->QueryComponent<Audio>(gameObjectId);
	
	if(!audio)
		return;

	LoadSoundBySource(audio->Source1);
}

void AudioSystem::Finish(uint32_t gameObjectId)
{
	
}

void AudioSystem::Finalize()
{
	Sound::GetInstance()->CleanChannel();
}

void AudioSystem::LoadSoundBySource(AudioSource& source)
{
	if (source.IsLoop)
		Sound::GetInstance()->Load2DSound(source.AudioName, source.AudioName, ISoundEngine::SoundType::BGM);
	else
		Sound::GetInstance()->Load2DSound(source.AudioName, source.AudioName, ISoundEngine::SoundType::EFFECT);
}

void AudioSystem::OnPlayAudio(std::any data)
{
	auto [id, audioName] = std::any_cast<std::pair<uint32_t, std::string>>(data);

	auto audio = m_GameObjectManager->QueryComponent<Audio>(id);

	if (!audio)
		return;

	AudioSource* source = nullptr;

	if (audio->Source1.AudioName == audioName)
		source = &audio->Source1;
	//else if (audio->Source2.AudioName == audioName)
	//	source = &audio->Source2;
	//else if (audio->Source3.AudioName == audioName)
	//	source = &audio->Source3;

	source->IsPlaying = true;

	if (source->IsLoop)
		Sound::GetInstance()->Play(id, audioName, ISoundEngine::SoundType::BGM, source->IsLoop, source->Volume);
	else
		Sound::GetInstance()->Play(id, audioName, ISoundEngine::SoundType::EFFECT, source->IsLoop, source->Volume);
}
