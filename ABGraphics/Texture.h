#pragma once

#include "IBindable.h"

class Surface;

namespace Bind
{
	class Texture : public IBindable
	{
	public:
		Texture(Graphics& graphics, const std::string& path, UINT slot = 0);
		virtual ~Texture() = default;

		static std::shared_ptr<Texture> Create(Graphics& graphics, const std::string& path, UINT slot = 0);
		static std::string GenerateUID(const std::string& path, UINT slot = 0);

		void Bind(Graphics& graphics) override;

		std::string GetUID() const override;
		bool HasAlpha() const;

	private:
		static UINT calculateNumberOfMipLevels(UINT width, UINT height);

	protected:
		bool mbHasAlpha = false;
		std::string mPath;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mpTextureView;

	private:
		unsigned int mSlot;
	};

	class TextureArray : public IBindable
	{
	public:
		TextureArray(Graphics& graphics, const std::vector<std::string>& paths, UINT slot = 0);
		virtual ~TextureArray() = default;

		void Bind(Graphics& graphics) override;

	protected:
		std::vector<std::string> mPaths;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mTextureView;
		unsigned int mSlot;
	};
}
