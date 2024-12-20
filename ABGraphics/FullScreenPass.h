#pragma once

#include "BindingPass.h"

namespace Rgph
{
	class FullScreenPass : public BindingPass
	{
	public:
		FullScreenPass(Graphics& graphics, const std::string name);
		void Execute(Graphics& graphics) const override;
	};
}