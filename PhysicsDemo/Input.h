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
private:
	static const int arrayVK[(int)KEY::LAST];

	Input();
public:
	~Input();
	void Initialize();
	void Update();


	static Input* GetInstance() {
		static Input* instance(new Input);
		return instance;
	}

	static void ReleaseInstance() {
		Input* instance = GetInstance();
		delete instance;
	}

public:
	static bool GetKeyDown(KEY key);
	static bool GetKeyUp(KEY key);
	static bool GetKey(KEY key);
	static Vector2 GetMousePos();


private:
	std::vector<KeyInfo> m_KeyVector;
	Vector2 m_CurMousePos;

};