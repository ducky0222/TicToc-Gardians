#include "pch.h"
#include "Input.h"


Input::Input()
{

}

Input::~Input()
{

}

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
				if (m_KeyVector[i].bPrevPushed) {
					// 이전 프레임에 눌려 있었다. 
					m_KeyVector[i].keyState = KEY_STATE::HOLD;
				}
				else {
					// 이전 프레임에 눌려있찌 않앗따. 
					m_KeyVector[i].keyState = KEY_STATE::TAP;
				}
				m_KeyVector[i].bPrevPushed = true;
			}
			// 키가 안눌려있다. 
			else {
				if (m_KeyVector[i].bPrevPushed) {
					// 이전 프레임에는 눌려있었따. 
					m_KeyVector[i].keyState = KEY_STATE::AWAY;
				}
				else {
					// 이전 프레임에 눌려있지 않았따. 
					m_KeyVector[i].keyState = KEY_STATE::NONE;
				}
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
			else if (KEY_STATE::AWAY == m_KeyVector[i].keyState) {
				m_KeyVector[i].keyState = KEY_STATE::NONE;
			}
		}
	}

}

bool Input::GetKeyDown(KEY key)
{
	assert(GetInstance());
	return GetInstance()->m_KeyVector[(int)key].keyState == KEY_STATE::TAP;
}

bool Input::GetKeyUp(KEY key)
{
	assert(GetInstance());
	return GetInstance()->m_KeyVector[(int)key].keyState == KEY_STATE::AWAY;
}

bool Input::GetKey(KEY key)
{
	assert(GetInstance());
	return GetInstance()->m_KeyVector[(int)key].keyState == KEY_STATE::HOLD;
}

Vector2 Input::GetMousePos()
{
	assert(GetInstance());
	return GetInstance()->m_CurMousePos;
}

