#include "ISoundEngine.h"
#include "SoundEngine.h"

ISoundEngine* Sound::GetInstance()
{
	static ISoundEngine* instance = nullptr;
	if (instance == nullptr)
	{
		instance = new SoundEngine();
	}
	return instance;
}

void Sound::Destroy()
{
	ISoundEngine* instance = GetInstance();
	if (instance != nullptr)
	{
		delete instance;
		instance = nullptr;
	}
}

