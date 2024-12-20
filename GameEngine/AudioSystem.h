#pragma once
#include "EventListener.h"
#include "System.h"

struct AudioSource;

class AudioSystem : public System, public EventListener, public IStartable, public IUpdatable
{
public:
	AudioSystem(GameObjectManager* goManager);
	~AudioSystem() override = default;

	void Update(float deltaTime) override;

	void Initialize() override;
	void Start(uint32_t gameObjectId) override;
	void Finish(uint32_t gameObjectId) override;
	void Finalize() override;

private:
	void LoadSoundBySource(AudioSource& source);
	void OnPlayAudio(std::any data);
};
