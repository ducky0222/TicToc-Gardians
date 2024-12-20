#pragma once

#include "IBindable.h"
#include "Vertex.h"

namespace Bind
{
	class InputLayout : public IBindable
	{
	public:
		InputLayout(Graphics& graphics, const Dvtx::VertexLayout& layout, ID3DBlob* pVSBytecode);
		~InputLayout() = default;

		static shared_ptr<InputLayout> Create(Graphics& graphics, const Dvtx::VertexLayout& layout, ID3DBlob* pVSBytecode);
		static string GenerateUID(const Dvtx::VertexLayout& layout, ID3DBlob* pVSBytecode = nullptr);

		virtual void Bind(Graphics& graphics) override;

		const Dvtx::VertexLayout GetLayout() const;
		std::string GetUID() const override;

	protected:
		Dvtx::VertexLayout mLayout;
		Microsoft::WRL::ComPtr<ID3D11InputLayout> mpInputLayout;
	};
}