#pragma once

#include "RenderQueuePass.h"
#include "DepthStencil.h"

namespace Rgph
{
	class ShadowPass : public RenderQueuePass
	{
	public:
		ShadowPass(Graphics& graphics, std::string name);
		~ShadowPass() = default;

		void OnResize(Graphics& graphics, UINT width, UINT height) override
		{
			// mSsaoMap->OnResize(graphics, width * 0.5f, height * 0.5f);
		}

		void Execute(Graphics& graphics) const override;

	private:
		enum { VIEWPORT_SIZE = 2048 };

		std::shared_ptr<Bind::ShaderInputDepthStencil> mpDepthMap;
	};
}