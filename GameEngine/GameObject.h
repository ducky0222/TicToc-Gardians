#pragma once
#include <nlohmann/json.hpp>


class GameObject
{
	GENERATE_CLASS_TYPE_INFO(GameObject)

public:
	PROPERTY(Name)
	PROPERTY(Id)
	NLOHMANN_DEFINE_TYPE_INTRUSIVE(GameObject, Name, Id)

	GameObject(std::string_view name, uint32_t id) : Name(name), Id(id) {}

	// 디시리얼라이징 임시 객체 생성자(사용 X)
	GameObject() = default;
	virtual ~GameObject() = default;

	std::string Name;
	uint32_t Id;
};
