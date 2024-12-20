#pragma once

#include "RenderQueuePass.h"

namespace Rgph
{
	class CutScenePass : public RenderQueuePass
	{
	public:
		CutScenePass(Graphics& graphics, std::string name);
	};
}
