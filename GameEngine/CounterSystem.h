#pragma once
#include "System.h"

/// 2024.01.18 ��â��
/// �ϴ��� �������� ����
/// System�� ������ ����
/// Transform Component�� ���� Entity�� ã�Ƽ� LocalPosition�� 1�� ������Ű�� System ����.
class CounterSystem : public System, public IUpdatable
{
public:
	/// �����ڸ� �θ����� �����Ѵ�.
	using System::System;

	void Update(float deltaTime) override;
	//void GuiRender() override;
};

