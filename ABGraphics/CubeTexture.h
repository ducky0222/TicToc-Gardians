#pragma once

#include "IBindable.h"

class Surface;

namespace Bind
{
	class CubeTexture : public IBindable
	{
	public:
		CubeTexture(Graphics& graphics, const std::string& path, UINT slot = 0);
		CubeTexture(Graphics& graphics, const std::vector<std::string>& paths, UINT slot = 0);
		~CubeTexture() = default;

		static std::shared_ptr<CubeTexture> Create(Graphics& graphics, const std::string& path, UINT slot = 0);
		static std::string GenerateUID(const std::string& path, UINT slot = 0);

		void Bind(Graphics& graphics);
		
		std::string GetUID() const override;

	protected:
		std::string mPath;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mpTextureView;

	private:
		unsigned int mSlot;
	};

}