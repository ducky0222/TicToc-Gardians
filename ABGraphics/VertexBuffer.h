#pragma once

#include "IBindable.h"
#include "Vertex.h"

namespace Bind
{
	class VertexBuffer : public IBindable
	{
	public:
		VertexBuffer(Graphics& graphics, const std::string& tag, const Dvtx::VertexBuffer& vbuf, D3D11_BIND_FLAG bindFlag = D3D11_BIND_VERTEX_BUFFER);
		VertexBuffer(Graphics& graphics, const Dvtx::VertexBuffer& vbuf, D3D11_BIND_FLAG bindFlag = D3D11_BIND_VERTEX_BUFFER);

		static std::shared_ptr<VertexBuffer> Create(Graphics& graphics, const std::string& tag, const Dvtx::VertexBuffer& vbuf);
		template<typename...Ignore>
		static std::string GenerateUID(const std::string& tag, Ignore&&...ignore);

		void Bind(Graphics& graphics) override;

		const Dvtx::VertexLayout& GetLayout() const;
		std::string GetUID() const  override;

	private:
		static std::string generateUID(const std::string& tag);

	protected:
		std::string mTag;
		UINT mStride;
		Microsoft::WRL::ComPtr<ID3D11Buffer> mpVertexBuffer;
		Dvtx::VertexLayout mLayout;
	};

	template<typename...Ignore>
	static std::string VertexBuffer::GenerateUID(const std::string& tag, Ignore&&...ignore)
	{
		return generateUID(tag);
	}

	class DynamicVectexBuffer : public IBindable
	{
	public:

	private:
		const IDrawable* mParent = nullptr;
	};
}
