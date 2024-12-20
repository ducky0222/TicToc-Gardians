#pragma once

#include "Graphics.h"
#include "IBindable.h"
#include "IDrawable.h"
#include "BindableResourceManager.h"

namespace Bind
{
	template<typename C>
	class ConstantBuffer : public IBindable
	{
		static_assert(sizeof(C) % 16 == 0, "constant buffer must be aligned by 16 bytes");

	public:
		ConstantBuffer(Graphics& graphics, UINT slot = 0u);
		ConstantBuffer(Graphics& graphics, const C& consts, UINT slot = 0u);
		virtual ~ConstantBuffer() = default;

		void Update(Graphics& graphics, const C& consts);

	protected:
		Microsoft::WRL::ComPtr<ID3D11Buffer> mpConstantBuffer;
		UINT mSlot;
	};

#pragma region ConstantBufferFunc
	template<typename C>
	ConstantBuffer<C>::ConstantBuffer(Graphics& graphics, const C& consts, UINT slot)
		: mSlot(slot)
	{
		D3D11_BUFFER_DESC cbd = {};
		cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbd.Usage = D3D11_USAGE_DYNAMIC;
		cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		cbd.MiscFlags = 0u;
		cbd.ByteWidth = sizeof(consts);
		cbd.StructureByteStride = 0u;

		D3D11_SUBRESOURCE_DATA csd = {};
		csd.pSysMem = &consts;
		GetDevice(graphics)->CreateBuffer(&cbd, &csd, &mpConstantBuffer);
	}

	template<typename C>
	ConstantBuffer<C>::ConstantBuffer(Graphics& graphics, UINT slot)
		: mSlot(slot)
	{
		D3D11_BUFFER_DESC cbd = {};
		cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbd.Usage = D3D11_USAGE_DYNAMIC;
		cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		cbd.MiscFlags = 0u;
		cbd.ByteWidth = sizeof(C);
		cbd.StructureByteStride = 0u;

		HRESULT hr = GetDevice(graphics)->CreateBuffer(&cbd, NULL, &mpConstantBuffer);
		assert(SUCCEEDED(hr));
	}

	template<typename C>
	void ConstantBuffer<C>::Update(Graphics& graphics, const C& consts)
	{
		D3D11_MAPPED_SUBRESOURCE msr;

		GetContext(graphics)->Map(
			mpConstantBuffer.Get(), 0u,
			D3D11_MAP_WRITE_DISCARD, 0u,
			&msr);

		memcpy(msr.pData, &consts, sizeof(consts));

		GetContext(graphics)->Unmap(mpConstantBuffer.Get(), 0u);
	}
#pragma endregion

	template<typename C>
	class VertexConstantBuffer : public ConstantBuffer<C>
	{
		using ConstantBuffer<C>::ConstantBuffer;
		using ConstantBuffer<C>::mpConstantBuffer;
		using ConstantBuffer<C>::mSlot;
		using IBindable::GetContext;

	public:
		static std::shared_ptr<VertexConstantBuffer<C>> Create(Graphics& graphics, const C& consts, UINT slot = 0);
		static std::shared_ptr<VertexConstantBuffer<C>> Create(Graphics& graphics, UINT slot = 0);
		static std::string GenerateUID(const C&, UINT slot);
		static std::string GenerateUID(UINT slot = 0);

		virtual void Bind(Graphics& graphics) override;

		string GetUID() const override;
	};

#pragma region VertexConstantBufferFunc
	template<typename C>
	shared_ptr<VertexConstantBuffer<C>> VertexConstantBuffer<C>::Create(Graphics& graphics, const C& consts, UINT slot)
	{
		return ResourceManager::Create<VertexConstantBuffer>(graphics, consts, slot);
	}

	template<typename C>
	shared_ptr<VertexConstantBuffer<C>> VertexConstantBuffer<C>::Create(Graphics& graphics, UINT slot)
	{
		return ResourceManager::Create<VertexConstantBuffer>(graphics, slot);
	}

	template<typename C>
	string VertexConstantBuffer<C>::GenerateUID(const C&, UINT slot)
	{
		return GenerateUID(slot);
	}

	template<typename C>
	string VertexConstantBuffer<C>::GenerateUID(UINT slot)
	{
		return typeid(VertexConstantBuffer).name() + "#"s + to_string(slot);
	}

	template<typename C>
	void VertexConstantBuffer<C>::Bind(Graphics& graphics)
	{
		GetContext(graphics)->VSSetConstantBuffers(mSlot, 1u, mpConstantBuffer.GetAddressOf());
	}

	template<typename C>
	string VertexConstantBuffer<C>::GetUID() const
	{
		return GenerateUID(mSlot);
	}
#pragma endregion

	template<typename C>
	class PixelConstantBuffer : public ConstantBuffer<C>
	{
		using ConstantBuffer<C>::ConstantBuffer;
		using ConstantBuffer<C>::mpConstantBuffer;
		using ConstantBuffer<C>::mSlot;
		using IBindable::GetContext;

	public:
		static std::shared_ptr<PixelConstantBuffer<C>> Create(Graphics& graphics, const C& consts, UINT slot = 0);
		static std::shared_ptr<PixelConstantBuffer<C>> Create(Graphics& graphics, UINT slot = 0);
		static std::string GenerateUID(const C&, UINT slot);
		static std::string GenerateUID(UINT slot = 0);

		virtual void Bind(Graphics& graphics) override;

		string GetUID() const override;
	};

#pragma region PixelConstantBufferFunc
	template<typename C>
	std::shared_ptr<PixelConstantBuffer<C>> PixelConstantBuffer<C>::Create(Graphics& graphics, const C& consts, UINT slot)
	{
		return ResourceManager::Create<PixelConstantBuffer>(graphics, consts, slot);
	}

	template<typename C>
	std::shared_ptr<PixelConstantBuffer<C>> PixelConstantBuffer<C>::Create(Graphics& graphics, UINT slot)
	{
		return ResourceManager::Create<PixelConstantBuffer>(graphics, slot);
	}

	template<typename C>
	std::string PixelConstantBuffer<C>::GenerateUID(const C&, UINT slot)
	{
		return GenerateUID(slot);
	}

	template<typename C>
	std::string PixelConstantBuffer<C>::GenerateUID(UINT slot)
	{
		return typeid(PixelConstantBuffer).name() + "#"s + std::to_string(slot);
	}

	template<typename C>
	void PixelConstantBuffer<C>::Bind(Graphics& graphics)
	{
		GetContext(graphics)->PSSetConstantBuffers(mSlot, 1u, mpConstantBuffer.GetAddressOf());
	}

	template<typename C>
	string PixelConstantBuffer<C>::GetUID() const
	{
		return GenerateUID(mSlot);
	}
#pragma endregion

	template<typename C>
	class GeometryConstantBuffer : public ConstantBuffer<C>
	{
		using ConstantBuffer<C>::ConstantBuffer;
		using ConstantBuffer<C>::mpConstantBuffer;
		using ConstantBuffer<C>::mSlot;
		using IBindable::GetContext;

	public:
		static std::shared_ptr<GeometryConstantBuffer<C>> Create(Graphics& graphics, const C& consts, UINT slot = 0);
		static std::shared_ptr<GeometryConstantBuffer<C>> Create(Graphics& graphics, UINT slot = 0);
		static std::string GenerateUID(const C&, UINT slot);
		static std::string GenerateUID(UINT slot = 0);

		virtual void Bind(Graphics& graphics) override;

		string GetUID() const override;
	};

	template<typename C>
	std::shared_ptr<GeometryConstantBuffer<C>> GeometryConstantBuffer<C>::Create(Graphics& graphics, const C& consts, UINT slot)
	{
		return ResourceManager::Create<GeometryConstantBuffer>(graphics, consts, slot);
	}

	template<typename C>
	std::shared_ptr<GeometryConstantBuffer<C>> GeometryConstantBuffer<C>::Create(Graphics& graphics, UINT slot)
	{
		return ResourceManager::Create<GeometryConstantBuffer>(graphics, slot);
	}

	template<typename C>
	std::string GeometryConstantBuffer<C>::GenerateUID(const C&, UINT slot)
	{
		return GenerateUID(slot);
	}

	template<typename C>
	std::string GeometryConstantBuffer<C>::GenerateUID(UINT slot)
	{
		return typeid(GeometryConstantBuffer).name() + "#"s + std::to_string(slot);
	}

	template<typename C>
	void GeometryConstantBuffer<C>::Bind(Graphics& graphics)
	{
		GetContext(graphics)->GSSetConstantBuffers(mSlot, 1u, mpConstantBuffer.GetAddressOf());
	}

	template<typename C>
	string GeometryConstantBuffer<C>::GetUID() const
	{
		return GenerateUID(mSlot);
	}
}