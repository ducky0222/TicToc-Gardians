#pragma once

#include "FullScreenPass.h"

namespace Rgph
{
	class OffScreenMappingPass : public FullScreenPass
	{
	public:
		OffScreenMappingPass(Graphics& graphics, std::string name);
		void Execute(Graphics& graphics) const override;
	};
}
