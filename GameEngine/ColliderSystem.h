#pragma once
#include "System.h"


/// <summary>
/// 콜라이더도 어쨋든 여기서 뭔가 돌아야한다. 
/// onCollisionEnter 등도 여기서 뭔가 할수 있을것같다. 
/// </summary>

// 수정 홍지환 선언 struct로 되어 있음
struct BoxCollider;

class ColliderSystem :
	public System, public IUpdatable
{
public:
	using System::System;

	void Update(float deltaTime) override;

private:
	bool IsCollide(const BoxCollider& bc1, const BoxCollider& bc2);
};

class ColliderEventSystem : public System, public IUpdatable
{
public:
	using System::System;

	void Update(float deltaTime) override;
};