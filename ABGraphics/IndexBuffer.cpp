#include "pch.h"

#include "IndexBuffer.h"

namespace Bind
{
	IndexBuffer::IndexBuffer(Graphics& graphics, const std::vector<UINT>& indices)
		: IndexBuffer(graphics, "?", indices)
	{
	}
	IndexBuffer::IndexBuffer(Graphics& graphics, std::string tag, const std::vector<UINT>& indices)
		: mTag(tag)
		, mCount((UINT)indices.size())
	{
		D3D11_BUFFER_DESC ibd = {};
		ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		ibd.Usage = D3D11_USAGE_DEFAULT;
		ibd.CPUAccessFlags = 0u;
		ibd.MiscFlags = 0u;
		ibd.ByteWidth = UINT(mCount * sizeof(UINT));
		ibd.StructureByteStride = sizeof(UINT);

		D3D11_SUBRESOURCE_DATA isd = {};
		isd.pSysMem = indices.data();

		(GetDevice(graphics)->CreateBuffer(&ibd, &isd, &mpIndexBuffer));
	}

	shared_ptr<IndexBuffer> IndexBuffer::Create(Graphics& graphics, const std::string& tag, const vector<UINT>& indices)
	{
		return ResourceManager::Create<IndexBuffer>(graphics, tag, indices);
	}
	template<typename...Ignore>
	static string IndexBuffer::GenerateUID(const std::string& tag, Ignore&&...ignore)
	{
		return generateUID(tag);
	}

	void IndexBuffer::Bind(Graphics& graphics)
	{
		GetContext(graphics)->IASetIndexBuffer(mpIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0u);
	}

	UINT IndexBuffer::GetCount() const
	{
		return mCount;
	}
	string IndexBuffer::GetUID() const
	{
		return generateUID(mTag);
	}

	std::string IndexBuffer::generateUID(const std::string& tag)
	{
		return typeid(IndexBuffer).name() + "#"s + tag;
	}

}