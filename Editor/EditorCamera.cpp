#include "pch.h"
#include "EditorCamera.h"
#include "Input.h"

void EditorCamera::SetLens()
{
	/// 프로젝션 행렬 만드는 핵심함수
	if (OrthographicValue == true)
	{
// 		X = 1;
// 		AspectValue = X / y;

		ProjectionMatrix = XMMatrixOrthographicLH(
			size * AspectValue, size,
			NearClipPlaneValue,
			FarClipPlaneValue);
	}
	else
	{
		ProjectionMatrix = XMMatrixPerspectiveFovLH(FieldOfViewValue, AspectValue, NearClipPlaneValue, FarClipPlaneValue);
	}
}

void EditorCamera::CameraMove(float deltaTime)
{
	Vector3 forw = -m_WorldMatrix.Forward();
	Vector3 right = m_WorldMatrix.Right();
	Vector3 up = m_WorldMatrix.Up();

	if (Input::GetKey(KEY::W))
	{
		Position += forw * Speed * deltaTime;
	}
	if (Input::GetKey(KEY::S))
	{
		Position -= forw * Speed * deltaTime;
	}
	if (Input::GetKey(KEY::A))
	{
		Position -= right * Speed * deltaTime;
	}
	if (Input::GetKey(KEY::D))
	{
		Position += right * Speed * deltaTime;
	}
	if (Input::GetKey(KEY::Q))
	{
		Position -= up * Speed * deltaTime;
	}
	if (Input::GetKey(KEY::E))
	{
		Position += up * Speed * deltaTime;
	}

	if (Input::GetKey(KEY::Z))
	{
		size += 1 * deltaTime;
	}

	if (Input::GetKey(KEY::X))
	{
		size -= 1 * deltaTime;
	}
}

void EditorCamera::CameraRotate(float deltaTime)
{
	Vector2 mousePos = Input::GetMousePos();

	/// 회전
	
	Quaternion r = Rotation;
	if (Input::GetKey(KEY::RBTN))
	{
		DirectX::XMFLOAT2 offset = { mousePos.x - m_PrevCur.x, mousePos.y - m_PrevCur.y };

		if (offset.x > 0 || offset.x < 0 || offset.y < 0 || offset.y > 0)
		{
			// yaw
			XMVECTOR r2 = DirectX::XMQuaternionRotationAxis({ 0,1,0,0 }, offset.x * 0.005f);
			// pitch
			XMVECTOR r3 = DirectX::XMQuaternionRotationAxis({ 1,0,0,0 }, offset.y * 0.005f);

			XMVECTOR r4 = DirectX::XMQuaternionMultiply(r3, XMLoadFloat4(&r));
			//cameraOrientation * frameYaw
			XMVECTOR r5 = DirectX::XMQuaternionMultiply(r4, r2);

			XMStoreFloat4(&r, r5);
		}
	}
	m_PrevCur = mousePos;

	Rotation = r;

}
