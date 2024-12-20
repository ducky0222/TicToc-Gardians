#pragma once
#include "Component.h"

// ���� �߰� ���� �Ұ��� ������Ʈ (Ʈ�������� ���ӵ�)
struct Parent : public Component
{
	GENERATE_BODY(Parent)
	GENERATE_PROPERTY(Parent, ParentID)

	uint32_t ParentID = 0;
};

// ���� �߰� ���� �Ұ��� ������Ʈ (Ʈ�������� ���ӵ�)
struct Children : public Component
{
	GENERATE_BODY(Children)
	GENERATE_PROPERTY(Children, ChildrenID)

	std::list<uint32_t> ChildrenID;
};

// ����, ���� ���� Ʈ������ ���� �� ���� ���� ���� ������Ʈ
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



