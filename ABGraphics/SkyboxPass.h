#pragma once

#include "Graphics.h"
#include "ConstantBuffer.h"
#include "BindingPass.h"

namespace Bind
{
	class CubeTexture;
}

class Camera;

namespace Rgph
{
	class RenderGraph;

	class SkyboxPass : public BindingPass
	{
	public:
		SkyboxPass(Graphics& graphics, const std::string& name);
		~SkyboxPass() = default;

		void Execute(Graphics& graphics) const override;

		void ResetSkymap() override { mSkybox = nullptr; }
		void SetSkymap(Graphics& graphics, const std::string& path) override;

	private:
		std::shared_ptr<Bind::VertexBuffer> mpSphereVertices;
		std::shared_ptr<Bind::IndexBuffer> mpSphereIndices;
		UINT mSphereCount;
		std::shared_ptr<Bind::CubeTexture> mSkybox;
	};
}