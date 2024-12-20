#pragma once

enum class KEY_STATE
{
	NONE,
	TAP,
	HOLD,
	AWAY,
};

enum class KEY
{
	LEFT,
	RIGHT,
	UP,
	DOWN,
	Q,
	W,
	E,
	R,
	T,
	Y,
	U,
	I,
	O,
	P,
	A,
	S,
	D,
	F,
	G,
	H,
	J,
	K,
	L,
	Z,
	X,
	C,
	V,
	B,
	N,
	M,

	NUM1,
	NUM2,
	NUM3,
	NUM4,
	NUM5,

	ALT,
	CTRL,
	LSHIFT,
	SPACE,
	ENTER,
	ESC,
	LBTN,
	RBTN,

	F1,
	F2,
	F3,
	F4,
	F5,

	LAST,// Ű ������
};


struct KeyInfo
{
	KEY_STATE keyState;
	bool bPrevPushed;
};

/// ��ǲ�Ŵ����ε�
/// �̰͵� ������Ʈ�� ���� �ֳ� ����غ���
class Input
{
public:
	static const int arrayVK[(int)KEY::LAST];

	Input() = default;
	~Input() = default;

	void Initialize();
	void Update();


	static Input& GetInstance() {
		static Input instance;
		return instance;
	}

public:
	static bool GetKeyDown(KEY key);
	static bool GetKeyUp(KEY key);
	static bool GetKey(KEY key);
	static VSM::Vector2 GetMousePos();
	static bool IsAnyKeyDown();

	std::vector<KeyInfo> GetKeyVector() { return m_KeyVector; }

private:
	std::vector<KeyInfo> m_KeyVector;
	VSM::Vector2 m_CurMousePos;

	 

};