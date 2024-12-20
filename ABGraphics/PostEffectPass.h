#pragma once
#include "FullScreenPass.h"

namespace Rgph
{
	class PostEffectPass : public FullScreenPass
	{
	public:
		PostEffectPass(Graphics& graphics, std::string name);
		void Execute(Graphics& graphics) const override;
	};
}
