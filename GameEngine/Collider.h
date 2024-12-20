#pragma once
#include "Component.h"

/// <summary>
/// 콜라이더를 상속받을 수 없다. 컴포넌트 풀 T문제와 시리얼라이즈 문제 때문
/// 이름도 맘에 안들면 나중에 바꾸도록 하자 
/// 일단 임시 
/// </summary>
struct BoxCollider : public Component
{
public:
	GENERATE_BODY(BoxCollider)

		// 중간 
		VSM::Vector3 Center;
	// 크기 ( 중간부터 끝까지.) 
	VSM::Vector3 Size;

	VSM::Quaternion Orientation;

};

/// <summary>
/// 얘는 사실 이벤트 매니저나 다른 걸로 만드는게 좋아보인다. 근데 난 아직 생각이 안난다. 
/// TODO : 나중에 생각해보자
/// </summary>
struct CollideEvent : public Component
{
public:
	GENERATE_BODY(CollideEvent)

	bool IsCollide = false;
	bool IsPrevCollide = false;

	// 여기 이벤트
	bool IsEnter = false;
	bool IsStay = false;
	bool IsExit = false;

	// 충돌한 오브젝트
	uint32_t CollideObjectID;



};
