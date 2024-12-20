#include "pch.h"

#include "ConstantBufferEx.h"

namespace Bind
{
	void ConstantBufferEx::Update(Graphics& graphics, const Dcb::Buffer& buf)
	{
		assert(&buf.GetRootLayoutElement() == &GetRootLayoutElement());

		D3D11_MAPPED_SUBRESOURCE msr;
		(GetContext(graphics)->Map(
			pConstantBuffer.Get(), 0u,
			D3D11_MAP_WRITE_DISCARD, 0u,
			&msr
		));

		memcpy(msr.pData, buf.GetData(), buf.GetSizeInBytes());

		GetContext(graphics)->Unmap(pConstantBuffer.Get(), 0u);
	}

	ConstantBufferEx::ConstantBufferEx(Graphics& graphics, const Dcb::LayoutElement& layoutRoot, UINT slot, const Dcb::Buffer* pBuf)
		: slot(slot)
	{
		D3D11_BUFFER_DESC cbd;
		cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbd.Usage = D3D11_USAGE_DYNAMIC;
		cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		cbd.MiscFlags = 0u;
		cbd.ByteWidth = (UINT)layoutRoot.GetSizeInBytes();
		cbd.StructureByteStride = 0u;

		if (pBuf != nullptr)
		{
			D3D11_SUBRESOURCE_DATA csd = {};
			csd.pSysMem = pBuf->GetData();
			(GetDevice(graphics)->CreateBuffer(&cbd, &csd, &pConstantBuffer));
		}
		else
		{
			(GetDevice(graphics)->CreateBuffer(&cbd, nullptr, &pConstantBuffer));
		}
	}
}