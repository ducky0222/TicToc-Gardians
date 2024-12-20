#include "pch.h"

#include "InputLayout.h"

#include "BindableResourceManager.h"

namespace Bind
{
	InputLayout::InputLayout(Graphics& graphics, const Dvtx::VertexLayout& layout, ID3DBlob* pVSBytecode)
		: mLayout(layout)
	{
		const auto d3dLayout = layout.GetD3DLayout();

		GetDevice(graphics)->CreateInputLayout(
			d3dLayout.data(),
			(UINT)d3dLayout.size(),
			pVSBytecode->GetBufferPointer(),
			pVSBytecode->GetBufferSize(),
			&mpInputLayout
		);
	}

	shared_ptr<InputLayout> InputLayout::Create(Graphics& graphics, const Dvtx::VertexLayout& layout, ID3DBlob* pVSBytecode)
	{
		return ResourceManager::Create<InputLayout>(graphics, layout, pVSBytecode);
	}
	string InputLayout::GenerateUID(const Dvtx::VertexLayout& layout, ID3DBlob* pVSBytecode)
	{
		return typeid(InputLayout).name() + "#"s + layout.GetCode();
	}

	void InputLayout::Bind(Graphics& graphics)
	{
		GetContext(graphics)->IASetInputLayout(mpInputLayout.Get());
	}

	const Dvtx::VertexLayout InputLayout::GetLayout() const
	{
		return mLayout;
	}
	std::string InputLayout::GetUID() const
	{
		return GenerateUID(mLayout);
	}
}