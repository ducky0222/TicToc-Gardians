#pragma once
#include "EventListener.h"
#include "System.h"

class RenderSystem : public System, public IRenderable, public IStartable, public EventListener
{
public:
	RenderSystem(GameObjectManager* goManager);

	void Render() override;

	void Initialize() override;
	void Start(uint32_t gameObjectId) override;
	void Finish(uint32_t gameObjectId) override;
	void Finalize() override;

private:
	std::map<std::string, std::string> mSkyBoxNamePaths;
};

