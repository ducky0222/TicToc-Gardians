#pragma once

#include <concepts>
#include <map>
#include <memory>
#include <string>
#include <string_view>
#include <typeinfo>
#include <vector>

#include "../GameEngine/ComponentPoolManager.h"

class Method;
class Property;
class TypeInfo;
class GameObjectManager;
struct Component;


template <typename T>
concept IsComponent2 = std::is_base_of_v<Component, T>;

template <typename T>
concept IsSystem = std::is_base_of_v<System, T>;

class ISystemGenerator
{
public:
	virtual ~ISystemGenerator() = default;
	virtual std::unique_ptr<System> Generate(GameObjectManager* goManager) const = 0;
};

template <typename T>
class SystemGenerator : public ISystemGenerator
{
public:
	std::unique_ptr<System> Generate(GameObjectManager* goManager) const override
	{
		return std::make_unique<T>(goManager);
	}
};

template <typename T>
concept HasSuper = requires
{
	typename T::Super;
} && !std::same_as<typename T::Super, void>;

template <typename T>
concept HasStaticTypeInfo = requires
{
	T::StaticTypeInfo();
};

template <typename T, typename U = void>
struct SuperClassTypeDeduction
{
	using Type = void;
};

template <typename T>
struct SuperClassTypeDeduction<T, std::void_t<typename T::ThisType>>
{
	using Type = T::ThisType;
};

template <typename T>
struct TypeInfoInitializer
{
	TypeInfoInitializer(const char* name)
		: m_name(name)
	{
		if constexpr (HasSuper<T>)
		{
			m_super = &(typename T::Super::StaticTypeInfo());
		}
	}

	const char* m_name = nullptr;
	const TypeInfo* m_super = nullptr;
};

class TypeInfo
{
public:
	using ComponentTypeMap = std::map<std::string, const Component*>;
	using SystemGeneratorMap = std::map<std::string, std::unique_ptr<ISystemGenerator>>;

	template <typename T>
	explicit TypeInfo(const TypeInfoInitializer<T>& initializer)
		: m_typeHash(typeid(T).hash_code())
		, m_name(initializer.m_name)
		, m_fullName(typeid(T).name())
		, m_super(initializer.m_super)
		, m_isArray(std::is_array_v<T>)
	{
		if constexpr (HasSuper<T>)
		{
			CollectSuperProperties();
		}

		if constexpr (IsComponent2<T>)
		{
			ComponentTypes.insert({ m_name, new T{} });
		}

		if constexpr (IsSystem<T>)
		{
			SystemGenerators.insert({ m_fullName, std::make_unique<SystemGenerator<T>>() });
		}
	}

	const TypeInfo* GetSuper() const;


	template <typename T> requires HasStaticTypeInfo<T>
	static const TypeInfo& GetStaticTypeInfo()
	{
		return T::StaticTypeInfo();
	}

	template <typename T> requires
		std::is_pointer_v<T>
		&& HasStaticTypeInfo<std::remove_pointer_t<T>>
		static const TypeInfo& GetStaticTypeInfo()
	{
		return std::remove_pointer_t<T>::StaticTypeInfo();
	}

	template <typename T> requires
		(!HasStaticTypeInfo<T>) &&
		(!HasStaticTypeInfo<std::remove_pointer_t<T>>)
		static const TypeInfo& GetStaticTypeInfo()
	{
		static TypeInfo typeInfo{ TypeInfoInitializer<T>("unreflected_type_variable") };
		return typeInfo;
	}

	bool IsA(const TypeInfo& other) const;
	bool IsChildOf(const TypeInfo& other) const;

	template <typename T>
	bool IsA() const
	{
		return IsA(GetStaticTypeInfo<T>());
	}

	template <typename T>
	bool IsChildOf() const
	{
		return IsChildOf(GetStaticTypeInfo<T>());
	}

	const std::vector<const Property*>& GetProperties() const
	{
		return m_properties;
	}

	const Property* GetProperty(const char* name) const;

	const char* GetName() const
	{
		return m_name.c_str();
	}

	size_t GetHash() const
	{
		return m_typeHash;
	}

	const char* GetFullName() const
	{
		return m_fullName.c_str();
	}

	bool IsArray() const
	{
		return m_isArray;
	}

	static const ComponentTypeMap& GetComponentMap()
	{
		return ComponentTypes;
	}

	static std::unique_ptr<System> GenerateSystem(const std::string& systemName, GameObjectManager* goManager);


private:
	void CollectSuperProperties();

	friend Property;
	using PropertyMap = std::map<std::string_view, const Property*>;

	void AddProperty(const Property* property);

	size_t m_typeHash;
	std::string m_name;
	std::string m_fullName;
	const TypeInfo* m_super = nullptr;

	bool m_isArray = false;

	std::vector<const Property*> m_properties;
	PropertyMap m_propertyMap;

	inline static ComponentTypeMap ComponentTypes;
	inline static SystemGeneratorMap SystemGenerators;

	friend class SystemManager;
	friend class GameProcess; // 삭제용
	friend class SerializeSystem; // 시리얼라이즈용
};