#pragma once
class EditorCamera
{
public:
	EditorCamera() = default;
	~EditorCamera() = default;

	void SetLens();


	void putNearClipPlane(float nVal) { NearClipPlaneValue = nVal; SetLens(); }
	float getNearClipPlane() { return NearClipPlaneValue; }

	void putFarClipPlane(float nVal) { FarClipPlaneValue = nVal; SetLens(); }
	float getFarClipPlane() { return FarClipPlaneValue; }

	void putAspect(float nVal) { AspectValue = nVal; SetLens(); }
	float getAspect() { return AspectValue; }

	void putFieldOfView(float nVal) { FieldOfViewValue = nVal; SetLens(); }
	float getFieldOfView() { return FieldOfViewValue; }

	void putOrthographic(bool nVal) { OrthographicValue = nVal; SetLens(); }
	bool getOrthographic() { return OrthographicValue; }

	Matrix getViewMatrix() { 
		Matrix world = Matrix::CreateTranslation(Position);
		Matrix rotation = Matrix::CreateFromQuaternion(Rotation);
		m_WorldMatrix = rotation * world;
		return m_WorldMatrix.Invert();
	}

	Matrix GetWorldMatrix() { return m_WorldMatrix; }


	void CameraMove(float deltaTime);
	void CameraRotate(float deltaTime);


	__declspec(property(get = getNearClipPlane, put = putNearClipPlane)) float NearClipPlane;
	__declspec(property(get = getFarClipPlane, put = putFarClipPlane)) float FarClipPlane;
	__declspec(property(get = getAspect, put = putAspect)) float Aspect;
	__declspec(property(get = getFieldOfView, put = putFieldOfView)) float FieldOfView;
	__declspec(property(get = getOrthographic, put = putOrthographic)) bool Orthographic;

	__declspec(property(get = getViewMatrix)) Matrix ViewMatrix;

	Matrix ProjectionMatrix = Matrix::Identity;

	Vector3 Position = Vector3::Zero;
	Quaternion Rotation = Quaternion::Identity;

	float size = 2;

private:
	float NearClipPlaneValue = 0.1f;
	float FarClipPlaneValue = 5000.f;
	float AspectValue = 0.1f;
	float FieldOfViewValue = 45.0f * (XM_PI / 180.0f);

	bool OrthographicValue = false;




	Matrix m_WorldMatrix = Matrix::Identity;
	float Speed = 500;

	Vector2 m_PrevCur = {};
};

