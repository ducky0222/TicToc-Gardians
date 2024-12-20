#pragma once

#include "RenderQueuePass.h"

namespace Rgph
{
	class BackgroundPass : public RenderQueuePass
	{
	public:
		BackgroundPass(Graphics& graphics, std::string name);
	};
}
