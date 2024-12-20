#pragma once
#include "Component.h"

// 임의 추가 삭제 불가능 컴포넌트 (트랜스폼에 종속됨)
struct Parent : public Component
{
	GENERATE_BODY(Parent)
	GENERATE_PROPERTY(Parent, ParentID)

	uint32_t ParentID = 0;
};

// 임의 추가 삭제 불가능 컴포넌트 (트랜스폼에 종속됨)
struct Children : public Component
{
	GENERATE_BODY(Children)
	GENERATE_PROPERTY(Children, ChildrenID)

	std::list<uint32_t> ChildrenID;
};

// 로컬, 월드 상의 트랜스폼 정보 및 계층 구조 관리 컴포넌트
struct Transform : public Component
{
	GENERATE_BODY(Transform)
	GENERATE_PROPERTY(Transform, LocalPosition, LocalRotation, LocalScale)

	VSM::Vector3 LocalPosition = VSM::Vector3::Zero;
	VSM::Quaternion LocalRotation = VSM::Quaternion::Identity;
	VSM::Vector3 LocalScale = VSM::Vector3::One;
	//VSM::Vector3 LocalEuler = VSM::Vector3::Zero;

	VSM::Vector3 WorldPosition = VSM::Vector3::Zero;
	VSM::Quaternion WorldRotation = VSM::Quaternion::Identity;
	VSM::Vector3 WorldScale = VSM::Vector3::One;

	VSM::Matrix LocalMatrix = VSM::Matrix::Identity;
	VSM::Matrix WorldMatrix = VSM::Matrix::Identity;
};



