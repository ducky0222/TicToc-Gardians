#pragma once

#include "IBindable.h"
#include "DynamicConstant.h"

class Graphics;

/*
Ex 타입은 Dcb::Buffer를 통해 상수버퍼를 생성해주는 클래스들이다.
CachingConstantBufferEx 클래스는 시스템 메모리에 상수버퍼 데이터를 유지하여,
프로브 객체가 탐색하고 수정할 수 있게 만든 클래스이다.
*/

namespace Bind
{
	class ConstantBufferEx : public IBindable
	{
	public:
		void Update(Graphics& graphics, const Dcb::Buffer& buf);

		virtual const Dcb::LayoutElement& GetRootLayoutElement() const  = 0;

	protected:
		ConstantBufferEx(Graphics& graphics, const Dcb::LayoutElement& layoutRoot, UINT slot, const Dcb::Buffer* pBuf);

	protected:
		Microsoft::WRL::ComPtr<ID3D11Buffer> pConstantBuffer;
		UINT slot;
	};

	class VertexConstantBufferEx : public ConstantBufferEx
	{
	public:
		using ConstantBufferEx::ConstantBufferEx;
		inline void Bind(Graphics& graphics)  override;
	};

#pragma region VertexConstantBufferExFunc
	inline void VertexConstantBufferEx::Bind(Graphics& graphics)
	{
		(GetContext(graphics)->VSSetConstantBuffers(slot, 1u, pConstantBuffer.GetAddressOf()));
	}
#pragma endregion

	class PixelConstantBufferEx : public ConstantBufferEx
	{
	public:
		using ConstantBufferEx::ConstantBufferEx;
		inline void Bind(Graphics& graphics)  override;
	};

#pragma region PixelConstantBufferExFunc
	void PixelConstantBufferEx::Bind(Graphics& graphics)
	{
		GetContext(graphics)->PSSetConstantBuffers(slot, 1u, pConstantBuffer.GetAddressOf());
	}
#pragma endregion

	template<class T>
	class CachingConstantBufferEx : public T
	{
	public:
		CachingConstantBufferEx(Graphics& graphics, const Dcb::CookedLayout& layout, UINT slot);
		CachingConstantBufferEx(Graphics& graphics, const Dcb::Buffer& buf, UINT slot);

		void Bind(Graphics& graphics) override;

		void SetBuffer(const Dcb::Buffer& buf_in);
		
		const Dcb::LayoutElement& GetRootLayoutElement() const override;
		const Dcb::Buffer& GetBuffer() const;

	private:
		bool dirty = false;
		Dcb::Buffer buf;
	};

#pragma region CachingConstantBufferExFunc
	template<class T>
	CachingConstantBufferEx<T>::CachingConstantBufferEx(Graphics& graphics, const Dcb::CookedLayout& layout, UINT slot)
		: T(graphics, *layout.ShareRoot(), slot, nullptr)
		, buf(Dcb::Buffer(layout))
	{
	}

	template<class T>
	CachingConstantBufferEx<T>::CachingConstantBufferEx(Graphics& graphics, const Dcb::Buffer& buf, UINT slot)
		: T(graphics, buf.GetRootLayoutElement(), slot, &buf)
		, buf(buf)
	{
	}

	template<class T>
	void CachingConstantBufferEx<T>::Bind(Graphics& graphics) 
	{
		if (dirty)
		{
			T::Update(graphics, buf);
			dirty = false;
		}
		T::Bind(graphics);
	}

	template<class T>
	void CachingConstantBufferEx<T>::SetBuffer(const Dcb::Buffer& buf_in)
	{
		buf.CopyFrom(buf_in);
		dirty = true;
	}

	template<class T>
	const Dcb::LayoutElement& CachingConstantBufferEx<T>::GetRootLayoutElement() const 
	{
		return buf.GetRootLayoutElement();
	}
	template<class T>
	const Dcb::Buffer& CachingConstantBufferEx<T>::GetBuffer() const
	{
		return buf;
	}
#pragma endregion

	using CachingPixelConstantBufferEx = CachingConstantBufferEx<PixelConstantBufferEx>;
	using CachingVertexConstantBufferEx = CachingConstantBufferEx<VertexConstantBufferEx>;
}