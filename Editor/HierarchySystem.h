#pragma once
#include "System.h"



class HierarchySystem : public System
{
public:
	using System::System;

	/// 하이어라키에서 선택된 값을 리턴
	inline static uint32_t SelectedNumber = 0;
	inline static uint32_t RClickedNumber = 0;
	inline static bool IsItemDoubleClicked = false;
	inline static bool IsObjectRightClicked = false;

	void EditorRender();
	void RealRender(uint32_t id);
};