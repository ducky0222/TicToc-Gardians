#pragma once

struct Component;
struct Transform;

template <typename T, size_t N = 4096 / sizeof(T)> requires std::is_base_of_v<Component, T>
class ComponentPool
{
public:
	ComponentPool() = default;

public:
	size_t ActivatedIndex = 0;
	std::array<T, N> Pool = {};
};