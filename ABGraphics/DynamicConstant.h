#pragma once

#define LEAF_ELEMENT_TYPES \
	X( Float ) \
	X( Float2 ) \
	X( Float3 ) \
	X( Float4 ) \
	X( Matrix ) \
	X( Bool ) \
	X( Integer )

namespace Dcb
{
	namespace dx = DirectX;

	enum Type
	{
#define X(el) el,
		LEAF_ELEMENT_TYPES
#undef X
		Struct,
		Array,
		Empty,
	};

	template<Type type>
	struct Map
	{
		static constexpr bool valid = false;
	};
	template<> struct Map<Float>
	{
		using SysType = float; // type used in the CPU side
		static constexpr size_t hlslSize = sizeof(SysType); // size of type on GPU side
		static constexpr const char* code = "F1"; // code used for generating signature of layout
		static constexpr bool valid = true; // metaprogramming flag to check validity of Map <param>
	};
	template<> struct Map<Float2>
	{
		using SysType = dx::SimpleMath::Vector2;
		static constexpr size_t hlslSize = sizeof(SysType);
		static constexpr const char* code = "F2";
		static constexpr bool valid = true;
	};
	template<> struct Map<Float3>
	{
		using SysType = dx::SimpleMath::Vector3;
		static constexpr size_t hlslSize = sizeof(SysType);
		static constexpr const char* code = "F3";
		static constexpr bool valid = true;
	};
	template<> struct Map<Float4>
	{
		using SysType = dx::SimpleMath::Vector4;
		static constexpr size_t hlslSize = sizeof(SysType);
		static constexpr const char* code = "F4";
		static constexpr bool valid = true;
	};
	template<> struct Map<Matrix>
	{
		using SysType = dx::SimpleMath::Matrix;
		static constexpr size_t hlslSize = sizeof(SysType);
		static constexpr const char* code = "M4";
		static constexpr bool valid = true;
	};
	template<> struct Map<Bool>
	{
		using SysType = bool;
		static constexpr size_t hlslSize = 4u;
		static constexpr const char* code = "BL";
		static constexpr bool valid = true;
	};
	template<> struct Map<Integer>
	{
		using SysType = int;
		static constexpr size_t hlslSize = sizeof(SysType);
		static constexpr const char* code = "IN";
		static constexpr bool valid = true;
	};

#define X(el) static_assert(Map<el>::valid,"Missing map implementation for " #el);
	LEAF_ELEMENT_TYPES
#undef X

		template<typename T>
	struct ReverseMap
	{
		static constexpr bool valid = false;
	};
#define X(el) \
	template<> struct ReverseMap<typename Map<el>::SysType> \
	{ \
		static constexpr Type type = el; \
		static constexpr bool valid = true; \
	};
	LEAF_ELEMENT_TYPES
#undef X

		class LayoutElement
	{
	private:
		struct ExtraDataBase
		{
			virtual ~ExtraDataBase() = default;
		};
		friend class RawLayout;
		friend struct ExtraData;
	public:
		std::string GetSignature() const;
		bool Exists() const;
		std::pair<size_t, const LayoutElement*> CalculateIndexingOffset(size_t offset, size_t index) const;
		LayoutElement& operator[](const std::string& key);
		const LayoutElement& operator[](const std::string& key) const;
		LayoutElement& T();
		const LayoutElement& T() const;
		size_t GetOffsetBegin() const;
		size_t GetOffsetEnd() const;
		size_t GetSizeInBytes() const;
		LayoutElement& Add(Type addedType, std::string name);
		template<Type typeAdded>
		LayoutElement& Add(std::string key)
		{
			return Add(typeAdded, std::move(key));
		}
		LayoutElement& Set(Type addedType, size_t size);
		template<Type typeAdded>
		LayoutElement& Set(size_t size)
		{
			return Set(typeAdded, size);
		}
		template<typename T>
		size_t Create() const
		{
			switch (type)
			{
#define X(el) case el: assert(typeid(Map<el>::SysType) == typeid(T)); return *offset;
				LEAF_ELEMENT_TYPES
#undef X
			default:
				assert("Tried to resolve non-leaf element" && false);
				return 0u;
			}
		}
	private:
		LayoutElement() = default;
		LayoutElement(Type typeIn);
		size_t Finalize(size_t offsetIn);
		std::string GetSignatureForStruct() const;
		std::string GetSignatureForArray() const;
		size_t FinalizeForStruct(size_t offsetIn);
		size_t FinalizeForArray(size_t offsetIn);
		static LayoutElement& GetEmptyElement()
		{
			static LayoutElement empty{};
			return empty;
		}
		static size_t AdvanceToBoundary(size_t offset);
		static bool CrossesBoundary(size_t offset, size_t size);
		static size_t AdvanceIfCrossesBoundary(size_t offset, size_t size);
		static bool ValidateSymbolName(const std::string& name);
	private:
		std::optional<size_t> offset;
		Type type = Empty;
		std::unique_ptr<ExtraDataBase> pExtraData;
	};

	class Layout
	{
		friend class LayoutCodex;
		friend class Buffer;
	public:
		size_t GetSizeInBytes() const;
		std::string GetSignature() const;
	protected:
		Layout(std::shared_ptr<LayoutElement> pRoot);
		std::shared_ptr<LayoutElement> pRoot;
	};

	class RawLayout : public Layout
	{
		friend class LayoutCodex;
	public:
		RawLayout();
		LayoutElement& operator[](const std::string& key);
		template<Type type>
		LayoutElement& Add(const std::string& key)
		{
			return pRoot->Add<type>(key);
		}
	private:
		void ClearRoot();
		std::shared_ptr<LayoutElement> DeliverRoot();
	};

	class CookedLayout : public Layout
	{
		friend class LayoutCodex;
		friend class Buffer;
	public:
		const LayoutElement& operator[](const std::string& key) const;
		std::shared_ptr<LayoutElement> ShareRoot() const;
	private:
		CookedLayout(std::shared_ptr<LayoutElement> pRoot);
		std::shared_ptr<LayoutElement> RelinquishRoot() const;
	};

	class ConstElementRef
	{
		friend class Buffer;
		friend class ElementRef;
	public:
		class Ptr
		{
			friend ConstElementRef;
		public:
			template<typename T>
			operator const T* () const
			{
				static_assert(ReverseMap<std::remove_const_t<T>>::valid, "Unsupported SysType used in pointer conversion");
				return &static_cast<const T&>(*ref);
			}
		private:
			Ptr(const ConstElementRef* ref);
			const ConstElementRef* ref;
		};
	public:
		bool Exists() const;
		ConstElementRef operator[](const std::string& key) const;
		ConstElementRef operator[](size_t index) const;
		Ptr operator&() const;
		template<typename T>
		operator const T& () const
		{
			static_assert(ReverseMap<std::remove_const_t<T>>::valid, "Unsupported SysType used in conversion");
			return *reinterpret_cast<const T*>(pBytes + offset + pLayout->Create<T>());
		}
	private:
		ConstElementRef(const LayoutElement* pLayout, const char* pBytes, size_t offset);
		size_t offset;
		const LayoutElement* pLayout;
		const char* pBytes;
	};

	class ElementRef
	{
		friend class Buffer;
	public:
		class Ptr
		{
			friend ElementRef;
		public:
			template<typename T>
			operator T* () const
			{
				static_assert(ReverseMap<std::remove_const_t<T>>::valid, "Unsupported SysType used in pointer conversion");
				return &static_cast<T&>(*ref);
			}
		private:
			Ptr(ElementRef* ref);
			ElementRef* ref;
		};
	public:
		operator ConstElementRef() const;
		bool Exists() const;
		ElementRef operator[](const std::string& key) const;
		ElementRef operator[](size_t index) const;
		// optionally set value if not an empty Ref
		template<typename S>
		bool SetIfExists(const S& val)
		{
			if (Exists())
			{
				*this = val;
				return true;
			}
			return false;
		}
		Ptr operator&() const;
		template<typename T>
		operator T& () const
		{
			static_assert(ReverseMap<std::remove_const_t<T>>::valid, "Unsupported SysType used in conversion");
			return *reinterpret_cast<T*>(pBytes + offset + pLayout->Create<T>());
		}
		template<typename T>
		T& operator=(const T& rhs) const
		{
			static_assert(ReverseMap<std::remove_const_t<T>>::valid, "Unsupported SysType used in assignment");
			return static_cast<T&>(*this) = rhs;
		}
	private:
		ElementRef(const LayoutElement* pLayout, char* pBytes, size_t offset);
		size_t offset;
		const LayoutElement* pLayout;
		char* pBytes;
	};

	class Buffer
	{
	public:
		Buffer(RawLayout&& lay);
		Buffer(const CookedLayout& lay);
		Buffer(CookedLayout&& lay);
		Buffer(const Buffer&);
		Buffer(Buffer&&);
		ElementRef operator[](const std::string& key);
		ConstElementRef operator[](const std::string& key) const;
		const char* GetData() const;
		size_t GetSizeInBytes() const;
		const LayoutElement& GetRootLayoutElement() const;
		void CopyFrom(const Buffer&);
		std::shared_ptr<LayoutElement> ShareLayoutRoot() const;
	private:
		std::shared_ptr<LayoutElement> pLayoutRoot;
		std::vector<char> bytes;
	};
}

#ifndef DCB_IMPL_SOURCE
#undef LEAF_ELEMENT_TYPES
#endif
