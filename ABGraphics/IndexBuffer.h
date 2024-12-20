#pragma once

#include "IBindable.h"
#include "BindableResourceManager.h"

class Graphics;

namespace Bind
{
	class IndexBuffer : public IBindable
	{
	public:
		IndexBuffer(Graphics& graphics, const std::vector<UINT>& indices);
		IndexBuffer(Graphics& graphics, std::string tag, const std::vector<UINT>& indices);
		~IndexBuffer() = default;

		static shared_ptr<IndexBuffer> Create(Graphics& graphics, const std::string& tag, const vector<UINT>& indices);
		template<typename...Ignore>
		static std::string GenerateUID(const std::string& tag, Ignore&&...ignore);

		virtual void Bind(Graphics& graphics) override;

		UINT GetCount() const;
		string GetUID() const override;

	private:
		static std::string generateUID(const std::string& tag);

	protected:
		std::string mTag;
		UINT mCount;
		Microsoft::WRL::ComPtr<ID3D11Buffer> mpIndexBuffer;
	};
}