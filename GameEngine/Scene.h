#pragma once

class ISerializable;

class Scene
{
public:
	Scene(std::string_view name);
	~Scene();

	std::string Name;
};