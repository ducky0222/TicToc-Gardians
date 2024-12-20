#include "pch.h"

#include "VertexBuffer.h"
#include "BindableResourceManager.h"

namespace Bind
{
	VertexBuffer::VertexBuffer(Graphics& graphics, const Dvtx::VertexBuffer& vbuf, D3D11_BIND_FLAG bindFlag)
		: VertexBuffer(graphics, "?", vbuf)
	{
	}
	VertexBuffer::VertexBuffer(Graphics& graphics, const std::string& tag, const Dvtx::VertexBuffer& vbuf, D3D11_BIND_FLAG bindFlag)
		: mStride((UINT)vbuf.GetLayout().Size())
		, mTag(tag)
		, mLayout(vbuf.GetLayout())
	{
		D3D11_BUFFER_DESC bd = {};
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER | bindFlag;
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.CPUAccessFlags = 0u;
		bd.MiscFlags = 0u;
		bd.ByteWidth = UINT(vbuf.SizeBytes());
		bd.StructureByteStride = mStride;

		D3D11_SUBRESOURCE_DATA sd = {};
		sd.pSysMem = vbuf.GetData();

		GetDevice(graphics)->CreateBuffer(&bd, &sd, &mpVertexBuffer);
	}

	std::shared_ptr<VertexBuffer> VertexBuffer::Create(Graphics& graphics, const std::string& tag, const Dvtx::VertexBuffer& vbuf)
	{
		assert(tag != "?");
		return ResourceManager::Create<VertexBuffer>(graphics, tag, vbuf);
	}

	void VertexBuffer::Bind(Graphics& graphics)
	{
		const UINT offset = 0u;
		GetContext(graphics)->IASetVertexBuffers(0u, 1u, mpVertexBuffer.GetAddressOf(), &mStride, &offset);
	}

	std::string VertexBuffer::GetUID() const
	{
		return GenerateUID(mTag);
	}
	const Dvtx::VertexLayout& VertexBuffer::GetLayout() const
	{
		return mLayout;
	}

	std::string VertexBuffer::generateUID(const std::string& tag)
	{
		using namespace std::string_literals;
		return typeid(VertexBuffer).name() + "#"s + tag;
	}
}
