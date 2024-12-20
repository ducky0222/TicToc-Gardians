#include "pch.h"
#include "Sample.h"

#include "../ABGraphics/RenderManager.h"
#include "../ABGraphics/MathUtil.h"
#include "../PhysicEngine/PhysicsEngine.h"
#include <GeometryGenerator.h>
#include "Input.h"
#include <cmath>

/// �̰� ���� ����(dll�� ����ϴ� ��)�� ���Ӽ��� �߰� ��Ű��
/// �Լ� ������ ��� ����
/// �Լ��� ����ϴ� ������Ʈ�� ���� �ű�
#pragma comment(lib, "../lib/PhysXCommon_64.lib")
#pragma comment(lib, "../lib/PhysXCooking_64.lib")
#pragma comment(lib, "../lib/PhysXFoundation_64.lib")

using namespace physx;

bool isNormalOrthogonal(const PxMat33& mat)
{
	// �� �� ������ ���� Ȯ��
	float column0Length = mat.column0.magnitude();
	if (std::abs(column0Length - 1.0f) > 0.001f) return false;
	float column1Length = mat.column1.magnitude();
	if (std::abs(column1Length - 1.0f) > 0.001f) return false;
	float column2Length = mat.column2.magnitude();
	if (std::abs(column2Length - 1.0f) > 0.001f) return false;

	PxVec3 columns[3] = { mat.column0, mat.column1, mat.column2 };

	// �� �� ���� ���� ������ Ȯ��
	for (int i = 0; i < 3; ++i)
	{
		for (int j = i + 1; j < 3; ++j)
		{
			float dot = columns[i].dot(columns[j]);
			if (std::abs(dot) > 0.001f)
			{ // �Ҽ��� ���� ���
				return false;
			}
		}
	}

	return true;
}

//Matrix PxTransformToSimpleMathMatrix(const PxTransform& pxTrans)
//{
//	// ���ʹϾ𿡼� ȸ�� ��ķ� ��ȯ
//	PxMat44 pxMat(pxTrans.q);
//
//	// ��ġ ���� �߰� (Z�� ����)
//	pxMat.setPosition(pxTrans.p);
//	pxMat[3][2] = -pxMat[3][2]; // Z ��ġ ����
//
//	// Z�� ���� ������ ���� �߰����� ó��
//	PxMat44 temp = PxMat44(PxIdentity);
//	temp.column2.z = -1;
//	pxMat = temp * pxMat;
//
//	//pxMat[2][0] = -pxMat[2][0];
//	//pxMat[2][1] = -pxMat[2][1];
//	//pxMat[2][2] = -pxMat[2][2];
//	//pxMat[2][3] = -pxMat[2][3];
//
//	// PxMat44���� SimpleMath::Matrix�� ��ȯ
//	return Matrix(
//		pxMat.column0.x, pxMat.column0.y, pxMat.column0.z, pxMat.column0.w,
//		pxMat.column1.x, pxMat.column1.y, pxMat.column1.z, pxMat.column1.w,
//		pxMat.column2.x, pxMat.column2.y, pxMat.column2.z, pxMat.column2.w,
//		pxMat.column3.x, pxMat.column3.y, pxMat.column3.z, pxMat.column3.w
//	);
//}

//Matrix PxTransformToSimpleMathMatrix(const PxTransform& pxTrans)
//{
//	// ���ʹϾ𿡼� ȸ�� ��ķ� ��ȯ
//	PxMat44 pxMat(pxTrans.q);
//
//	// ��ġ ���� �߰�
//	pxMat.setPosition(pxTrans.p);
//
//	// PxMat44���� SimpleMath::Matrix�� ��ȯ
//	return Matrix(
//		pxMat.column0.x, pxMat.column0.y, pxMat.column0.z, pxMat.column0.w,
//		pxMat.column1.x, pxMat.column1.y, pxMat.column1.z, pxMat.column1.w,
//		pxMat.column2.x, pxMat.column2.y, pxMat.column2.z, pxMat.column2.w,
//		pxMat.column3.x, pxMat.column3.y, pxMat.column3.z, pxMat.column3.w
//	);
//}

PxTransform SimpleMathMatrixToPxTransform(const Matrix& matrix)
{
	Matrix alpha = matrix;

	// ��ġ ���� (Z�� ���� ���)
	PxVec3 position(matrix._41, matrix._42, -matrix._43);
	Matrix temp = Matrix::Identity;
	temp._33 = -1;

	alpha = temp * alpha;

	// ȸ�� ��� ���� (Z�� ���� ���)
	PxMat33 rotationMatrix(PxVec3(alpha._11, alpha._12, alpha._13),
		PxVec3(alpha._21, alpha._22, alpha._23),
		PxVec3(alpha._31, alpha._32, alpha._33));

	bool isNorOrth = isNormalOrthogonal(rotationMatrix);

	// ���ʹϾ����� ��ȯ
	PxQuat rotation(rotationMatrix);

	/// ����ȭ �׻�
	rotation.normalize();

	// PxTransform ����
	return PxTransform(position, rotation);
}

entryPoint::Sample::Sample(HINSTANCE hInstance, UINT width, UINT height, std::wstring name)
	: Processor(hInstance, width, height, name)
{

}

entryPoint::Sample::~Sample()
{
	Input::GetInstance()->ReleaseInstance();
	RenderManager::GetInstance().Destroy();
	Physics::Destroy();
}

bool entryPoint::Sample::Init()
{
	if (!Processor::Init())
	{
		return false;
	}

	RenderManager::GetInstance().Init(mhWnd, mWidth, mHeight, true);
	Physics::GetInstance()->Initialize();
	Input::GetInstance()->Initialize();
	Physics::GetInstance()->AddMaterial("Character", 0.5f, 0.5f, 0.5f);

	auto camera = RenderManager::GetInstance().GetCamera();
	camera->SetLens(0.25f * PI, GetAspectRatio(), 1.0f, 10000.0f);
	camera->SetPosition(0.0f, -50.0f, -1100.0f);
	camera->RotatePitch(0.f);
	camera->RotateYaw(0.5f);

	m_screenFront = Vector3(camera->GetLook().x, 0, camera->GetLook().z);
	m_screenFront.Normalize();
	m_screenBack = Vector3(-camera->GetLook().x, 0, -camera->GetLook().z);
	m_screenBack.Normalize();

	m_screenRight = Vector3(camera->GetRight().x, 0, camera->GetRight().z);
	m_screenRight.Normalize();
	m_screenLeft = Vector3(-camera->GetRight().x, 0, -camera->GetRight().z);
	m_screenLeft.Normalize();

	

	//RenderManager::GetInstance().SetProjection(camera->GetProj());
	//RenderManager::GetInstance().SetView(camera->GetView());


	

	// ������ �ڽ� -> ������ ���̳���
	{
		Vector3 Size = { 50,50,50 };
		Matrix plainTransform = Matrix::Identity;
		plainTransform.Translation({ 950.0f, -50.0f, 300.0f });
		PxTransform pxTransform = SimpleMathMatrixToPxTransform(plainTransform);
		Physics::GetInstance()->AddPxRigidActor(0);
		Physics::GetInstance()->AttachCollision(0, 10.0f, PhysicsEngine::BOX_SHAPE);
		/*	PxMaterial* tempMaterial = Physics::GetInstance()->FindMaterial(PhysicsEngine::ePxMaterial::PLAYER_MATERIAL);
			Physics::GetInstance()->MakeBoxCollider("smallbox", pxTransform, Size.x / 2, Size.y / 2, Size.z / 2, tempMaterial, 10.0f, PhysicsEngine::eActorType::DYNAMIC_ACTORTYPE);*/

		GeometryGenerator::MeshData meshdata;
		GeometryGenerator::CreateBox(Size, &meshdata);
		RenderManager::GetInstance().CreateGeometryModel("0", meshdata);
		RenderManager::GetInstance().ApplyGeomodelTransform("0", plainTransform);
	}
	// ������ ���� -> �Ǹ���
	{
		Vector3 Size = { 25,25,25 };
		Matrix plainTransform = Matrix::Identity;
		plainTransform.Translation({ 850.f, 50.0f, 300.0f });
		PxTransform pxTransform = {};
		pxTransform.p = { 50.f, 50, 500.0f };
		Physics::GetInstance()->AddPxRigidActor(1);
		Physics::GetInstance()->AttachCollision(1, 10.0f, PhysicsEngine::CYLINDER_SHAPE);

		//PxTransform pxTransform = SimpleMathMatrixToPxTransform(plainTransform);
		//PxMaterial* tempMaterial = Physics::GetInstance()->FindMaterial(PhysicsEngine::ePxMaterial::PLAYER_MATERIAL);
		//Physics::GetInstance()->MakeCapsuleCollider("Cylinder", pxTransform, 25.f, 25.f, tempMaterial, .1f);

		GeometryGenerator::MeshData meshdata;
		//GeometryGenerator::CreateCylinder(25.f, 25.f, 50.f, 5, 5, &meshdata);
		GeometryGenerator::CreateSphere(25.f, 25, 25, &meshdata);
		RenderManager::GetInstance().CreateGeometryModel("1", meshdata);
		RenderManager::GetInstance().ApplyGeomodelTransform("1", plainTransform);
	}

	// 16 by 16 ����
	{
		Vector3 Size = { 100,100,100 };
		Matrix plainTransform = Matrix::Identity;
		for (size_t i = 0; i < 16; i++)
		{
			float tempX = 100.0f * i;
			for (size_t j = 0; j < 16; j++)
			{
				float tempZ = 100.0f * j;
				plainTransform.Translation({ 50.f + tempX, -300, -500.0f + tempZ });
				PxTransform pxTransform = SimpleMathMatrixToPxTransform(plainTransform);
				uint32_t index = 2 + j + 16 * i;

				Physics::GetInstance()->AddPxRigidActor(index);
				//Physics::GetInstance()->AttachCollision(index, 10.0f, PhysicsEngine::BOX_SHAPE);

				GeometryGenerator::MeshData meshdata;
				GeometryGenerator::CreateBox(Size, &meshdata);
				std::string key = std::to_string(index);
				RenderManager::GetInstance().CreateGeometryModel(key, meshdata);
				RenderManager::GetInstance().ApplyGeomodelTransform(key, plainTransform);
			}
		}
	}

	return true;
}

void entryPoint::Sample::Update(float deltaTime)
{
	Input::GetInstance()->Update();

	auto camera = RenderManager::GetInstance().GetCamera();

	if (Input::GetInstance()->GetKey(KEY::W))
	{
		camera->TranslateLook(deltaTime * 100);
	}
	if (Input::GetInstance()->GetKey(KEY::S))
	{
		camera->TranslateLook(-deltaTime * 100);
	}
	if (Input::GetInstance()->GetKey(KEY::A))
	{
		camera->TranslateRight(-deltaTime * 100);
	}
	if (Input::GetInstance()->GetKey(KEY::D))
	{
		camera->TranslateRight(deltaTime * 100);
	}
	if (Input::GetInstance()->GetKey(KEY::Q))
	{
		camera->TranslateUp(deltaTime * 100);
	}
	if (Input::GetInstance()->GetKey(KEY::E))
	{
		camera->TranslateUp(-deltaTime * 100);
	}
	static float MaxKeyDownTime = 0.25;
	static float keydowned = 0;
	static bool temp = false;
	if (Input::GetInstance()->GetKeyDown(KEY::SPACE))
	{
		Physics::GetInstance()->AddImpulse(1, { 0,1,0 }, 10.f);

	}
	if (Input::GetInstance()->GetKey(KEY::SPACE))
	{
		keydowned += deltaTime;

		if (keydowned > MaxKeyDownTime && temp == false)
		{
			Physics::GetInstance()->AddForce(1, { 0,1,0 }, 1000.f);
			temp = true;
		}
	}


	if (Input::GetInstance()->GetKey(KEY::UP))
	{
		Physics::GetInstance()->AddAcceleration(1, { 0,0,1 }, 50.f);
		if (Physics::GetInstance()->GetVelocity_xz(1) > 200)
		{
			Physics::GetInstance()->SetVelocity_xz(1, 200);
		}
	}

	if (Input::GetInstance()->GetKey(KEY::DOWN))
	{
		Physics::GetInstance()->AddAcceleration(1, { 0,0,-1 }, 50.f);
		if (Physics::GetInstance()->GetVelocity_xz(1) > 200)
		{
			Physics::GetInstance()->SetVelocity_xz(1, 200);
		}
	}
	if (Input::GetInstance()->GetKey(KEY::LEFT))
	{
		Physics::GetInstance()->AddAcceleration(1, { -1,0,0 }, 50.f);
		if (Physics::GetInstance()->GetVelocity_xz(1) > 200)
		{
			Physics::GetInstance()->SetVelocity_xz(1, 200);
		}
	}
	if (Input::GetInstance()->GetKey(KEY::RIGHT))
	{
		Physics::GetInstance()->AddAcceleration(1, { 1,0,0 }, 50.f);
		if (Physics::GetInstance()->GetVelocity_xz(1) > 200)
		{
			Physics::GetInstance()->SetVelocity_xz(1, 200);
		}
	}

	// �߷� ����, ���� ��������
	if (Input::GetInstance()->GetKeyDown(KEY::CTRL))
	{
		Physics::GetInstance()->AddAcceleration(1, { 0,-1,0 }, 500.f);
	}

	if (Input::GetInstance()->GetKeyDown(KEY::P))
	{
		Physics::GetInstance()->ChangeScale(0, { 3,1,1 }, 10.0f);
	}

	static float accumulateTime = 0.f;
	accumulateTime += deltaTime;

	// ���� ����
	Physics::GetInstance()->Update(deltaTime);
}

void entryPoint::Sample::Render()
{
	RenderManager::GetInstance().BeginRender();

	//RenderManager::GetInstance().SubmitModel("model1");
	RenderManager::GetInstance().SubmitGeometryModel("0");
	RenderManager::GetInstance().SubmitGeometryModel("1");
	RenderManager::GetInstance().SubmitGeometryModel("2");

	/// ����� ���� -> �ݶ��̴� ���̾� ������ �ʿ�
	RenderManager::GetInstance().Excute();


	RenderManager::GetInstance().EndRender();
}
