#include "pch.h"
#include "CounterSystem.h"

#include "Transform.h"
#include "TestCounter.h"

void CounterSystem::Update(float deltaTime)
{
	/// ComponentItr은 레퍼런스 래퍼 이용했기 떄문에
	/// operator&가 구현되어있는데 
	/// 이게 auto&로는 작동을 안해서 왠만하면 타입을 그대로 써주자
	for (Transform& transform : ComponentItr<Transform>())
	{
		auto sibling = transform.Sibling<TestCounter>(m_GameObjectManager);

		if (sibling)
		{
			transform.LocalPosition.x += 1.0f;
			sibling->Counter++;
		}
	}
}

//void CounterSystem::GuiRender()
//{
//	for (Transform& transform : ComponentItr<Transform>())
//	{
//		auto sibling = transform.Sibling<TestCounter>(m_GameObjectManager);
//
//		if (sibling)
//		{
//			ImGui::Text("ID : %d", sibling->MyId);
//			ImGui::Text("Counter : %d", sibling->Counter);
//		}
//	}
//}
