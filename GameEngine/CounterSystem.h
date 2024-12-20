#pragma once
#include "System.h"

/// 2024.01.18 정창도
/// 일단은 삭제하지 말자
/// System의 간단한 예제
/// Transform Component를 가진 Entity를 찾아서 LocalPosition을 1씩 증가시키는 System 예제.
class CounterSystem : public System, public IUpdatable
{
public:
	/// 생성자를 부모한테 위임한다.
	using System::System;

	void Update(float deltaTime) override;
	//void GuiRender() override;
};

