#pragma once

#include "Graphics.h"

#define DVTX_ELEMENT_AI_EXTRACTOR(member) static SysType Extract( const aiMesh& mesh,size_t i )  { return *reinterpret_cast<const SysType*>(&mesh.member[i]); }

struct BGRAColor
{
	unsigned char a;
	unsigned char r;
	unsigned char g;
	unsigned char b;
};

struct BoneIndex
{
	int x;
	int y;
	int z;
	int w;
};

#define LAYOUT_ELEMENT_TYPES \
	X( Position2D ) \
	X( Position3D ) \
	X( Texture2D ) \
	X( Normal ) \
	X( Tangent ) \
	X( Bitangent ) \
	X( Float3Color ) \
	X( Float4Color ) \
	X( BGRAColor ) \
	X( BoneIndices ) \
	X( BoneWeights ) \
	X( Count ) \
	X( QuadSize ) \
	X( Velocity ) \
	X( Age ) \

namespace Dvtx
{
	class VertexLayout
	{
	public:
		enum ElementType
		{
#define X(el) el,
			LAYOUT_ELEMENT_TYPES
#undef X
		};

		template<ElementType> struct Map;
		template<> struct Map<Position2D>
		{
			using SysType = DirectX::SimpleMath::Vector2;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32_FLOAT;
			static constexpr const char* semantic = "Position";
			static constexpr const char* code = "P2";
			DVTX_ELEMENT_AI_EXTRACTOR(mVertices)
		};
		template<> struct Map<Position3D>
		{
			using SysType = DirectX::SimpleMath::Vector3;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32_FLOAT;
			static constexpr const char* semantic = "Position";
			static constexpr const char* code = "P3";
			DVTX_ELEMENT_AI_EXTRACTOR(mVertices)
		};
		template<> struct Map<Texture2D>
		{
			using SysType = DirectX::SimpleMath::Vector2;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32_FLOAT;
			static constexpr const char* semantic = "Texcoord";
			static constexpr const char* code = "T2";
			DVTX_ELEMENT_AI_EXTRACTOR(mTextureCoords[0])
		};
		template<> struct Map<Normal>
		{
			using SysType = DirectX::SimpleMath::Vector3;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32_FLOAT;
			static constexpr const char* semantic = "Normal";
			static constexpr const char* code = "N";
			DVTX_ELEMENT_AI_EXTRACTOR(mNormals)
		};
		template<> struct Map<Tangent>
		{
			using SysType = DirectX::SimpleMath::Vector3;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32_FLOAT;
			static constexpr const char* semantic = "Tangent";
			static constexpr const char* code = "Nt";
			DVTX_ELEMENT_AI_EXTRACTOR(mTangents)
		};
		template<> struct Map<Bitangent>
		{
			using SysType = DirectX::SimpleMath::Vector3;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32_FLOAT;
			static constexpr const char* semantic = "Bitangent";
			static constexpr const char* code = "Nb";
			DVTX_ELEMENT_AI_EXTRACTOR(mBitangents)
		};
		template<> struct Map<Float3Color>
		{
			using SysType = DirectX::SimpleMath::Vector3;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32_FLOAT;
			static constexpr const char* semantic = "Color";
			static constexpr const char* code = "C3";
			DVTX_ELEMENT_AI_EXTRACTOR(mColors[0])
		};
		template<> struct Map<Float4Color>
		{
			using SysType = DirectX::XMFLOAT4;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32A32_FLOAT;
			static constexpr const char* semantic = "Color";
			static constexpr const char* code = "C4";
			DVTX_ELEMENT_AI_EXTRACTOR(mColors[0])
		};
		template<> struct Map<BGRAColor>
		{
			using SysType = ::BGRAColor;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
			static constexpr const char* semantic = "Color";
			static constexpr const char* code = "C8";
			DVTX_ELEMENT_AI_EXTRACTOR(mColors[0])
		};
		template<> struct Map<BoneIndices>
		{
			using SysType = BoneIndex;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32A32_SINT;
			static constexpr const char* semantic = "Indices";
			static constexpr const char* code = "BI";
			DVTX_ELEMENT_AI_EXTRACTOR(mFaces)
		};
		template<> struct Map<BoneWeights>
		{
			using SysType = Vector4;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32A32_FLOAT;
			static constexpr const char* semantic = "Weights";
			static constexpr const char* code = "BW";
			DVTX_ELEMENT_AI_EXTRACTOR(mFaces)
		};
		template<> struct Map<QuadSize>
		{
			using SysType = Vector2;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32_FLOAT;
			static constexpr const char* semantic = "Size";
			static constexpr const char* code = "Size";
			DVTX_ELEMENT_AI_EXTRACTOR(mFaces)
		};
		template<> struct Map<Velocity>
		{
			using SysType = Vector3;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32_FLOAT;
			static constexpr const char* semantic = "Velocity";
			static constexpr const char* code = "Velo";
			DVTX_ELEMENT_AI_EXTRACTOR(mFaces)
		};
		template<> struct Map<Age>
		{
			using SysType = float;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32_FLOAT;
			static constexpr const char* semantic = "Age";
			static constexpr const char* code = "Age";
			DVTX_ELEMENT_AI_EXTRACTOR(mFaces)
		};
		template<> struct Map<Count>
		{
			using SysType = long double;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_UNKNOWN;
			static constexpr const char* semantic = "!INVALID!";
			static constexpr const char* code = "!INV!";
			DVTX_ELEMENT_AI_EXTRACTOR(mFaces)
		};

		template<template<VertexLayout::ElementType> class F, typename... Args>
		static constexpr auto Bridge(VertexLayout::ElementType type, Args&&... args)
		{
			switch (type)
			{
#define X(el) case VertexLayout::el: return F<VertexLayout::el>::Exec( std::forward<Args>( args )... );
				LAYOUT_ELEMENT_TYPES
#undef X
			}
			assert("Invalid element type" && false);
			return F<VertexLayout::Count>::Exec(std::forward<Args>(args)...);
		}

		class Element
		{
		public:
			Element(ElementType type, size_t offset);
			size_t GetOffsetAfter() const;
			size_t GetOffset() const;
			size_t Size() const;
			static constexpr size_t SizeOf(ElementType type);
			ElementType GetType() const;
			D3D11_INPUT_ELEMENT_DESC GetDesc() const;
			const char* GetCode() const;
		private:
			ElementType type;
			size_t offset;
		};
	public:
		template<ElementType Type>
		const Element& Create() const
		{
			for (auto& e : elements)
			{
				if (e.GetType() == Type)
				{
					return e;
				}
			}
			assert("Could not resolve element type" && false);
			return elements.front();
		}
		const Element& ResolveByIndex(size_t i) const;
		VertexLayout& Append(ElementType type);
		size_t Size() const;
		size_t GetElementCount() const;
		std::vector<D3D11_INPUT_ELEMENT_DESC> GetD3DLayout() const;
		std::string GetCode() const;
		bool Has(ElementType type) const;
	private:
		std::vector<Element> elements;
	};

	class Vertex
	{
		friend class VertexBuffer;
	private:
		// necessary for Bridge to SetAttribute
		template<VertexLayout::ElementType type>
		struct AttributeSetting
		{
			template<typename T>
			static constexpr auto Exec(Vertex* pVertex, char* pAttribute, T&& val)
			{
				return pVertex->SetAttribute<type>(pAttribute, std::forward<T>(val));
			}
		};
	public:
		template<VertexLayout::ElementType Type>
		auto& Attr()
		{
			auto pAttribute = pData + layout.Create<Type>().GetOffset();
			return *reinterpret_cast<typename VertexLayout::Map<Type>::SysType*>(pAttribute);
		}
		template<typename T>
		void SetAttributeByIndex(size_t i, T&& val)
		{
			const auto& element = layout.ResolveByIndex(i);
			auto pAttribute = pData + element.GetOffset();
			VertexLayout::Bridge<AttributeSetting>(
				element.GetType(), this, pAttribute, std::forward<T>(val)
			);
		}
	protected:
		Vertex(char* pData, const VertexLayout& layout);
	private:
		// enables parameter pack setting of multiple parameters by element index
		template<typename First, typename ...Rest>
		void SetAttributeByIndex(size_t i, First&& first, Rest&&... rest)
		{
			SetAttributeByIndex(i, std::forward<First>(first));
			SetAttributeByIndex(i + 1, std::forward<Rest>(rest)...);
		}
		// helper to reduce code duplication in SetAttributeByIndex
		template<VertexLayout::ElementType DestLayoutType, typename SrcType>
		void SetAttribute(char* pAttribute, SrcType&& val)
		{
			using Dest = typename VertexLayout::Map<DestLayoutType>::SysType;
			if constexpr (std::is_assignable<Dest, SrcType>::value)
			{
				*reinterpret_cast<Dest*>(pAttribute) = val;
			}
			else
			{
				assert("Parameter attribute type mismatch" && false);
			}
		}
	private:
		char* pData = nullptr;
		const VertexLayout& layout;
	};

	class ConstVertex
	{
	public:
		ConstVertex(const Vertex& v);
		template<VertexLayout::ElementType Type>
		const auto& Attr() const
		{
			return const_cast<Vertex&>(vertex).Attr<Type>();
		}
	private:
		Vertex vertex;
	};

	class VertexBuffer
	{
	public:
		VertexBuffer(VertexLayout layout, size_t size = 0u);
		VertexBuffer(VertexLayout layout, const aiMesh& mesh);

		const char* GetData() const;
		const VertexLayout& GetLayout() const;
		void Resize(size_t newSize);
		size_t Size() const;
		size_t SizeBytes() const;
		template<typename ...Params>
		void EmplaceBack(Params&&... params)
		{
			assert(sizeof...(params) == layout.GetElementCount() && "Param count doesn't match number of vertex elements");
			buffer.resize(buffer.size() + layout.Size());
			Back().SetAttributeByIndex(0u, std::forward<Params>(params)...);
		}
		Vertex Back();
		Vertex Front();
		Vertex operator[](size_t i);
		ConstVertex Back() const;
		ConstVertex Front() const;
		ConstVertex operator[](size_t i) const;
	private:
		std::vector<char> buffer;
		VertexLayout layout;
	};
}

#undef DVTX_ELEMENT_AI_EXTRACTOR
#ifndef DVTX_SOURCE_FILE
#undef LAYOUT_ELEMENT_TYPES
#endif