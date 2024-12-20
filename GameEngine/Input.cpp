#include "pch.h"
#include "Input.h"

const int Input::arrayVK[(int)KEY::LAST] =
{
	VK_LEFT,
	VK_RIGHT,
	VK_UP,
	VK_DOWN,

	'Q',
	'W',
	'E',
	'R',
	'T',
	'Y',
	'U',
	'I',
	'O',
	'P',

	'A',
	'S',
	'D',
	'F',
	'G',
	'H',
	'J',
	'K',
	'L',

	'Z',
	'X',
	'C',
	'V',
	'B',
	'N',
	'M',

	'1',
	'2',
	'3',
	'4',
	'5',

	VK_MENU,
	VK_CONTROL,
	VK_LSHIFT,
	VK_SPACE,
	VK_RETURN,
	VK_ESCAPE,
	VK_LBUTTON,
	VK_RBUTTON,

	VK_F1,
	VK_F2,
	VK_F3,
	VK_F4,
	VK_F5,
};

void Input::Initialize()
{
	for (int i = 0; i < (int)KEY::LAST; ++i) {
		m_KeyVector.push_back(KeyInfo{ KEY_STATE::NONE, false });
	}
}

void Input::Update()
{
	// 윈도우 포커싱 알아내기
	// HWND hMainWnd = CCore::GetInst()->GetMainHwnd();

	HWND hWnd = GetFocus();

	// 윈도우가 포커싱중일 때 
	if (hWnd != nullptr) {
		// Mouse 위치 계산 

		POINT ptPos = {};
		GetCursorPos(&ptPos);
		ScreenToClient(hWnd, &ptPos);

		m_CurMousePos = DirectX::XMFLOAT2((float)ptPos.x, (float)ptPos.y);

		for (int i = 0; i < (int)KEY::LAST; ++i) {
			// 키가 눌려있따. 
			if (GetAsyncKeyState(arrayVK[i]) & 0x8000)
			{
				m_KeyVector[i].keyState = m_KeyVector[i].bPrevPushed ? KEY_STATE::HOLD : KEY_STATE::TAP;
				m_KeyVector[i].bPrevPushed = true;
			}
			// 키가 안눌려있다. 
			else
			{
				m_KeyVector[i].keyState = m_KeyVector[i].bPrevPushed ? KEY_STATE::AWAY : KEY_STATE::NONE;
				m_KeyVector[i].bPrevPushed = false;
			}

		}
	}
	// 포커싱이 아닐때 
	else {
		for (int i = 0; i < (int)KEY::LAST; ++i) {
			m_KeyVector[i].bPrevPushed = false;

			if (KEY_STATE::TAP == m_KeyVector[i].keyState || KEY_STATE::HOLD == m_KeyVector[i].keyState)
			{
				m_KeyVector[i].keyState = KEY_STATE::AWAY;
			}
			else if (KEY_STATE::AWAY == m_KeyVector[i].keyState) 
			{
				m_KeyVector[i].keyState = KEY_STATE::NONE;
			}
		}
	}

}

bool Input::GetKeyDown(KEY key)
{
	return GetInstance().m_KeyVector[(int)key].keyState == KEY_STATE::TAP;
}

bool Input::GetKeyUp(KEY key)
{
	return GetInstance().m_KeyVector[(int)key].keyState == KEY_STATE::AWAY;
}

bool Input::GetKey(KEY key)
{
	return GetInstance().m_KeyVector[(int)key].keyState == KEY_STATE::HOLD;
}

bool Input::IsAnyKeyDown()
{
	for (size_t key = static_cast<size_t>(KEY::LEFT); key < static_cast<size_t>(KEY::LAST); key++)
	{
		if (key >= static_cast<size_t>(KEY::ESC) && key <= static_cast<size_t>(KEY::RBTN))
			continue;

		if (key == static_cast<size_t>(KEY::X) or key == static_cast<size_t>(KEY::NUM1) or key == static_cast<size_t>(KEY::R))
			continue;

		if (GetKeyDown(static_cast<KEY>(key)))
			return true;
	}
	return false;
}

VSM::Vector2 Input::GetMousePos()
{
	return GetInstance().m_CurMousePos;
}

