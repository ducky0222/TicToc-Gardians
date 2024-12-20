#pragma once

#include "IBindable.h"

class Graphics;

namespace Bind
{
	class PixelShader : public IBindable
	{
	public:
		PixelShader(Graphics& graphics, const std::string& path);
		virtual ~PixelShader() = default;

		static shared_ptr<PixelShader> Create(Graphics& graphics, const std::string& path);
		static string GenerateUID(const std::string& path);

		void Bind(Graphics& graphics) override;

		string GetUID() const override;

	protected:
		string mPath;
		Microsoft::WRL::ComPtr<ID3D11PixelShader> mpPixelShader;
	};
}