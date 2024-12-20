#pragma once

#include "IBindable.h"

namespace Bind
{
	class VertexShader : public IBindable
	{
	public:
		VertexShader(Graphics& graphics, const string& path);
		virtual ~VertexShader() = default;

		static shared_ptr<VertexShader> Create(Graphics& graphics, const string& path);
		static string GenerateUID(const string& path);

		virtual void Bind(Graphics& graphics) override;

		ID3D10Blob* GetBytecode() const;
		string GetUID() const override;

	private:
		string mPath;
		Microsoft::WRL::ComPtr<ID3D10Blob> mpBytecodeBlob;
		Microsoft::WRL::ComPtr<ID3D11VertexShader> mpVertexShader;
	};
}