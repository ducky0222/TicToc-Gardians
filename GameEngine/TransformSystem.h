#pragma once
#include "System.h"

class GameObject;

class TransformSystem : public System, public IUpdatable, public IStartable
{
public:
	using System::System;

	void Update(float deltaTime) override;

	void Initialize() override;
	void Start(uint32_t id) override;
	void Finish(uint32_t id) override;
	void Finalize() override;

	void Optimize();


private:
	void ProcessTransform(Transform* transform);
	void GenerateQuaternion(Transform* transform);
};

class ParentChildrenSystem : public System
{
public:
	using System::System;

	bool CheckParent(uint32_t parent, uint32_t child);
	void AddChild(GameObject* parent, GameObject* child);
	void AddChild(uint32_t parent, uint32_t child);
	void RemoveChild(uint32_t parent, uint32_t child);
	void RemoveParent(uint32_t childID);
};