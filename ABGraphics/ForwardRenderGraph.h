#pragma once

#include "RenderGraph.h"
#include "ConstantBufferEx.h"

class Graphics;
class IBindable;
class Camera;
class FrameLight;

namespace Bind
{
	class RenderTarget;
}

namespace Rgph
{
	class ForwardRenderGraph : public RenderGraph
	{
	public:
		ForwardRenderGraph(Graphics& graphics, FrameLight* frameLight);

		// 여기에 런타임에 제어하는 로직을 넣어줄 예정
		void Execute(Graphics& graphics) override;

		void OnResize(Graphics& graphics, UINT width, UINT height);
		void ResetSkymap();
		void ResetIBL();
		void SetSkymap(Graphics& graphics, const std::string& path);
		void SetIBL(Graphics& graphics, const std::string& diffuse, const std::string& specular, const std::string& brdfLUT);

	private:
		FrameLight* mFrameLight;
	};
}
