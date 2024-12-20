#pragma once

#include "IBindable.h"

namespace Bind
{
	class GeometryShader : public IBindable
	{
	public:
		GeometryShader(Graphics& graphics, const string& path);
		virtual ~GeometryShader() = default;

		static shared_ptr<GeometryShader> Create(Graphics& graphics, const string& path);
		static string GenerateUID(const string& path);

		virtual void Bind(Graphics& graphics) override;

		string GetUID() const override;

	private:
		string mPath;
		Microsoft::WRL::ComPtr<ID3D11GeometryShader> mGeometryShader;
	};
}