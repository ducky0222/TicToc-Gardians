// PhysicEngine.cpp : 정적 라이브러리를 위한 함수를 정의합니다.

#include "pch.h"
#include "framework.h"
#include "HysxEngine.h"
#include <crtdbg.h>
#include "CollisionCallback.h"
#include "../ABGraphics/DebugRenderManager.h"



using namespace physx;

enum { ADJ_LIST_SIZE = 64 };


namespace Physics
{
	PxFilterFlags CustomFilterShader(PxFilterObjectAttributes attributes0, PxFilterData filterData0,
		PxFilterObjectAttributes attributes1, PxFilterData filterData1,
		PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize)
	{
		//Physics::GetInstance()->findPxFilterData(filterData0);// 얘가 trigger박스이다
		//Physics::GetInstance()->findPxFilterData(filterData1);	//그냥 trigger박스가 제외된 물체이다.
		//filterData0.word3==ePxLayer::COLLISION_DETECTION_LAYER
		if (filterData0.word3 & filterData1.word3)
		{
			pairFlags = PxPairFlag::eNOTIFY_TOUCH_FOUND | PxPairFlag::eNOTIFY_TOUCH_LOST | PxPairFlag::eNOTIFY_TOUCH_PERSISTS;
			pairFlags |= PxPairFlag::eTRIGGER_DEFAULT;

			return PxFilterFlag::eDEFAULT;
		}

		if (!(filterData0.word1 & filterData1.word0))
		{
			return PxFilterFlag::eSUPPRESS;
		}


		pairFlags = PxPairFlag::eCONTACT_DEFAULT;
		return PxFilterFlag::eDEFAULT;
	}

	/// <summary>
	/// 게임 오브젝트의 트렌스폼 >> pxTransform으로 스케일 포함x
	/// </summary>
	/// <param name="simpleMathMatrix"></param>
	/// <returns></returns>
	physx::PxTransform MatrixToPxTransform(const DirectX::SimpleMath::Matrix& simpleMathMatrix)
	{
		// SimpleMath(Matrix)에서 크기, 회전, 위치를 추출합니다.
		DirectX::SimpleMath::Vector3 scale;
		DirectX::SimpleMath::Vector3 position;
		DirectX::SimpleMath::Quaternion rotation;

		// 크기는 간단하게 X, Y, Z 스케일 값을 추출합니다.
		scale.x = simpleMathMatrix._11;
		scale.y = simpleMathMatrix._22;
		scale.z = simpleMathMatrix._33;

		// 회전은 행렬에서 직접 추출합니다.
		rotation = DirectX::SimpleMath::Quaternion::CreateFromRotationMatrix(simpleMathMatrix);

		// 위치는 행렬의 마지막 열에서 추출합니다.
		position.x = simpleMathMatrix._41;
		position.y = simpleMathMatrix._42;
		position.z = -simpleMathMatrix._43;

		// PxTransform으로 변환합니다.
		PxTransform pxTrans(PxVec3(position.x, position.y, position.z), PxQuat(rotation.x, rotation.y, rotation.z, rotation.w));

		return pxTrans;
	}

	Matrix PxTransformToMatrix(const physx::PxTransform& pxTrans)
	{
		// PxTransform에서 위치와 회전을 가져옵니다.
		DirectX::SimpleMath::Vector3 position(pxTrans.p.x, pxTrans.p.y, -pxTrans.p.z);
		DirectX::SimpleMath::Quaternion rotation(pxTrans.q.x, pxTrans.q.y, pxTrans.q.z, pxTrans.q.w);
		// SimpleMath(Matrix)로 변환합니다.

		/// 여기 scale 대신 게임오브젝트의 스케일값을 가져와서 곱해서 보내주는 기능추가.
		DirectX::SimpleMath::Matrix simpleMathMatrix =
			DirectX::SimpleMath::Matrix::CreateScale({ 1.f,1.f,1.f }) *
			DirectX::SimpleMath::Matrix::CreateFromQuaternion(rotation) *
			DirectX::SimpleMath::Matrix::CreateTranslation(position);

		return simpleMathMatrix;
	}

	/// <summary>
	/// gameobject_scale * gameobject_rotation * physic_Pose.
	/// </summary>
	/// <param name="pxTrans"></param>
	/// <returns></returns>
	Matrix PxTranstoTransform(const physx::PxTransform& pxTrans)
	{
		// PxTransform에서 위치를 가져옵니다.
		DirectX::SimpleMath::Vector3 position(pxTrans.p.x, pxTrans.p.y, -pxTrans.p.z);
		//DirectX::SimpleMath::Quaternion rotation(pxTrans.q.x, pxTrans.q.y, pxTrans.q.z, pxTrans.q.w);
		// SimpleMath(Matrix)로 변환합니다.

		// GameObject의 트랜스폼 값 가져오기.	G
		DirectX::SimpleMath::Matrix matrixA = DirectX::SimpleMath::Matrix::Identity;
		// Matrix에서 위치, 회전, 스케일을 추출
		DirectX::SimpleMath::Vector3 gameObjectposition;
		DirectX::SimpleMath::Quaternion gameObjectRotation;
		DirectX::SimpleMath::Vector3 gameObjectScale;
		matrixA.Decompose(gameObjectScale, gameObjectRotation, gameObjectposition);



		/// 여기 scale 대신 게임오브젝트의 스케일값을 가져와서 곱해서 보내주는 기능추가.
		DirectX::SimpleMath::Matrix newTransform =
			DirectX::SimpleMath::Matrix::CreateScale(gameObjectScale) *
			DirectX::SimpleMath::Matrix::CreateFromQuaternion(gameObjectRotation) *
			DirectX::SimpleMath::Matrix::CreateTranslation(position);

		return newTransform;
	}

#pragma region Physics_Instance

	HysxEngine* GetInstance()
	{
		static HysxEngine* instance = nullptr;
		if (instance == nullptr)
		{
			instance = new HysxEngine();
		}

		return instance;
	}
	void Destroy()
	{
		HysxEngine* instance = GetInstance();
		if (instance != nullptr)
		{
			delete instance;
			instance = nullptr;
		}
		_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
		_CrtSetBreakAlloc(7087);

	}
#pragma endregion
}


HysxEngine::HysxEngine()
{
}

HysxEngine::~HysxEngine()
{
	finalize();
}

Matrix GetTranslationFromPxTransform(const PxTransform& pxTrans)
{
	// PxTransform에서 위치와 회전을 가져옵니다.
	Vector3 position(pxTrans.p.x, pxTrans.p.y, -pxTrans.p.z);
	// SimpleMath(Matrix)로 변환합니다.
	Matrix simpleMathMatrix = Matrix::CreateTranslation(position);

	return simpleMathMatrix;
}

#pragma region Public Functions
void HysxEngine::Initialize()
{	
	m_Allocator = PxDefaultAllocator();
	m_ErrorCallback = PxDefaultErrorCallback();

	// PhysX Foundation 생성
	m_pFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, m_Allocator, m_ErrorCallback);

#ifdef _DEBUG
	m_pPvd = PxCreatePvd(*m_pFoundation);
	m_pTransport = PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
	m_pPvd->connect(*m_pTransport, PxPvdInstrumentationFlag::eALL);

#endif
	m_pPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_pFoundation, PxTolerancesScale(), true, m_pPvd);
	// PhysX Physics 객체 생성

	///Scene 설정
	CreatePxScene();

	/// PhysX Material (정지 마찰력, 운동 마찰력, 반환력) Component로 추가할 것.

	initialize_PxFilterDatas();
	initialize_PhysxMaterial();
	//PxVec3 a;
	//a.magnitude(); //xz 속력
	//PxRigidDynamic* asd;
	//if (a.magnitude()>300)
	//{
	//	PxVec3 dir = a.getNormalized();
	//	auto temp = asd->getLinearVelocity();
	//	PxVec3 tempvelospeed = dir * 300;
	//	tempvelospeed.y = temp.y;
	//	asd->setLinearVelocity(tempvelospeed);
	//}

}

void HysxEngine::Update(float deltaTime)
{
	updatingCollisionState();

	m_pScene->simulate(deltaTime);
	m_pScene->fetchResults(true);


}

void HysxEngine::AddPxRigidActor(uint32_t ComponentID, unsigned int material, unsigned int physicsLayer, unsigned int actorType, bool Lock_lvX, bool Lock_lvY, bool Lock_lvZ, bool Lock_avX, bool Lock_avY, bool Lock_avZ, unsigned int shapeType, Vector3 Size, float weight, Vector3 Scale, DirectX::SimpleMath::Matrix transform, bool isCharModel, bool noGravity)
{
	if (shapeType == eColliderShape::CAPSULE_COLLIDER)
	{
		makeCapsuleCollider(ComponentID, material, physicsLayer, actorType, Lock_lvX, Lock_lvY, Lock_lvZ, Lock_avX, Lock_avY, Lock_avZ, shapeType, Size, weight, Scale, transform, isCharModel, noGravity);
	}
	else if (shapeType == eColliderShape::BOX_COLLIDER)
	{
		makeBoxCollider(ComponentID, material, physicsLayer, actorType, Lock_lvX, Lock_lvY, Lock_lvZ, Lock_avX, Lock_avY, Lock_avZ, shapeType, Size, weight, Scale, transform, isCharModel, noGravity);
	}
	else if (shapeType == eColliderShape::CYLINDER_COLLIDER)
	{
		makeCylinderCollider(ComponentID, material, physicsLayer, actorType, Lock_lvX, Lock_lvY, Lock_lvZ, Lock_avX, Lock_avY, Lock_avZ, shapeType, Size, weight, Scale, transform, isCharModel, noGravity);
	}

	//for (auto& actor : m_PhysicsActors)
	//{
	//	if (actor.second->getType() == PxActorType::eRIGID_DYNAMIC)
	//	{
	//		auto dynamic = static_cast<PxRigidDynamic*>(actor.second);
	//		dynamic->setSolverIterationCounts(8, 4);
	//	}
	//}
}

void HysxEngine::ChangeStaticToDynamic(uint32_t ComponentID
	, unsigned int material, unsigned int physicsLayer, unsigned int actorType
	, bool Lock_lvX, bool Lock_lvY, bool Lock_lvZ, bool Lock_avX, bool Lock_avY, bool Lock_avZ
	, unsigned int shapeType, DirectX::SimpleMath::Vector3 Size, float weight, DirectX::SimpleMath::Vector3 Scale, DirectX::SimpleMath::Matrix transform, bool isCharModel, bool noGravity)
{
	ReleaseActor(ComponentID);
	AddPxRigidActor(ComponentID, material, physicsLayer, 1, Lock_lvX, Lock_lvY, Lock_lvZ, Lock_avX, Lock_avY, Lock_avZ, shapeType, Size, weight, Scale, transform, isCharModel, noGravity);

}


void HysxEngine::AttachCollision(uint32_t ComponentID, unsigned int type, unsigned int material, Vector3 Size, Vector3 Scale, Vector3 extention)
{
	if ((eCollisionShape)type == eCollisionShape::CAPSULE_COLLISION)
	{
		makeCapsuleCollision(ComponentID, material, Size, Scale, extention);
	}
	else if ((eCollisionShape)type == eCollisionShape::BOX_COLLISION)
	{
		makeBoxCollision(ComponentID, material, Size, Scale, extention);
	}
	else if ((eCollisionShape)type == eCollisionShape::CYLINDER_COLLISION)
	{
		makeCylinderCollision(ComponentID, material, Size, Scale, extention);
	}
	else if ((eCollisionShape)type == eCollisionShape::GIMMICK_BOX_COLLISION)
	{
		makeGimmickBoxCollision(ComponentID, material, Size, Scale, extention);
	}
}
/// <summary>
/// 이거 좀 수정해야할 것 같습니다. 현재 Boxcolider_ static 의 경우 두개의 shape 가 들어가게 됩니다.
/// 
/// 현재 trigger
/// </summary>
/// <param name="ComponentID"></param>
void HysxEngine::DetachCollision(uint32_t ComponentID)
{
	auto actor = findPxActorbyCompID(ComponentID);

	unsigned int numShapes = actor->getNbShapes();
	///그리고 trigger만 들어갈수도 있을 것 같은데, 이것도 처리하려면 그냥 numshapes
	//

	std::vector<physx::PxShape*> shapes(numShapes);
	actor->getShapes(&shapes[0], numShapes);

	for (auto& shape : shapes)
	{
		std::string* name = static_cast<std::string*>(shape->userData);
		if (name != nullptr)
		{
			if (*name == "trigger")
			{
				delete static_cast<std::string*>(shape->userData);
				actor->detachShape(*shape);
			}
		}

	}
}

void HysxEngine::ChangeScale(uint32_t ComponentID, float Weight, DirectX::SimpleMath::Vector3 size, DirectX::SimpleMath::Vector3 currentscale, const DirectX::SimpleMath::Vector3 scale, DirectX::SimpleMath::Vector3 extension)
{
	// id를 통해서 현재 오브젝트에 설정된 액터에 접근
	auto currentActor = findPxActorbyCompID(ComponentID);
	DirectX::SimpleMath::Vector3 preSize = { size.x * currentscale.x ,size.y * currentscale.y ,size.z * currentscale.z };
	DirectX::SimpleMath::Vector3 AfterSize = { size.x * scale.x,size.y * scale.y,size.z * scale.z };

	// 액터가 가지고 있는 shape들을 가져온다 -> collider와 trigger shape 때문에 왠만하면 2개씩 가지고 있다
	const PxU32 numOfShapes = currentActor->getNbShapes();
	PxShape** shapes = new PxShape * [numOfShapes];
	PxU32 temp = currentActor->getShapes(shapes, numOfShapes);

	for (size_t i = 0; i < numOfShapes; i++)
	{
		PxShape* curShape = shapes[i];
		std::string* name = static_cast<std::string*>(curShape->userData);

		if (name != nullptr)
		{
			if (*name != "trigger")
			{
				PxGeometryHolder curGeometry = curShape->getGeometry();
				PxGeometryType::Enum geoType = curGeometry.getType();
				PxTransform tempPose = currentActor->getGlobalPose();
				switch (geoType)
				{
					case PxGeometryType::eBOX:
					{
						// box 갱신하는 함수
						PxBoxGeometry newBox;
						PxTransform AddPose = { 0,(AfterSize.y / 2) - (preSize.y / 2) + 1.f,0 };
						PxTransform resultTransform;
						resultTransform.q = tempPose.q;
						resultTransform.p = tempPose.p + AddPose.p;

						if (currentActor->getType() == PxActorType::eRIGID_DYNAMIC)
						{
							auto dynamicActor = static_cast<PxRigidDynamic*>(currentActor);

							float tempdensity = Weight / (AfterSize.x * AfterSize.y * AfterSize.z);
							PxRigidBodyExt::updateMassAndInertia(*dynamicActor, tempdensity);
							newBox.halfExtents = { AfterSize.x / 2 ,AfterSize.y / 2,AfterSize.z / 2 };
							curShape->setGeometry(newBox);
							dynamicActor->setGlobalPose(resultTransform);
							curShape->setContactOffset(10.0f);
						}

						else if (currentActor->getType() == PxActorType::eRIGID_DYNAMIC)
						{
							auto staticActor = static_cast<PxRigidStatic*>(currentActor);
							newBox.halfExtents = { AfterSize.x / 2 ,AfterSize.y / 2,AfterSize.z / 2 };
							curShape->setGeometry(newBox);
							staticActor->setGlobalPose(resultTransform);
							curShape->setContactOffset(10.0f);
						}
					}

					break;

					case PxGeometryType::eCAPSULE:
					{
						// capsule 갱신하는 함수
						PxCapsuleGeometry newCapsule;
						PxTransform AddPose = { 0,(AfterSize.y / 2) - (preSize.y / 2) + 0.001f,0 };
						PxTransform resultTransform;
						resultTransform.q = tempPose.q;
						resultTransform.p = tempPose.p + AddPose.p;
						//가로 세로 중에 큰값을 지름으로 설정.
						float tempradius = AfterSize.x;
						if (tempradius <= AfterSize.z)
							tempradius = AfterSize.z;
						//반지름 = 지름/2
						tempradius = tempradius / 2;

						// 상자의 절반 길이 = 전체높이/2 - 반지름;
						float tempboxheight = AfterSize.y / 2 - tempradius;
						newCapsule.radius = tempradius;
						newCapsule.halfHeight = tempboxheight;

						curShape->setGeometry(newCapsule);
					}
					break;

					// 아마 cylinder는 custom callback
					case PxGeometryType::eCUSTOM:
					{
						std::shared_ptr <PxCustomGeometryExt::CylinderCallbacks> newCylinder = std::make_shared<PxCustomGeometryExt::CylinderCallbacks>(0.f, 0.f, 0, 0.f);
						//가로 세로 중에 큰값을 지름으로 설정.
						float tempradius = AfterSize.x;
						if (tempradius <= AfterSize.z)
							tempradius = AfterSize.z;
						//반지름 = 지름/2
						tempradius = tempradius / 2;

						float tempheight = AfterSize.y;
						newCylinder->setRadius(tempradius);
						newCylinder->setHeight(tempheight);

						curShape->setGeometry(PxCustomGeometry(*newCylinder));
						//m_CylinderTriggerGeometrys.emplace_back(newCylinder);
						m_cylinderGeometrys[ComponentID].emplace_back(newCylinder);
					}
					break;
				}
			}
			else
			{
				PxGeometryHolder curGeometry = curShape->getGeometry();

				PxGeometryType::Enum geoType = curGeometry.getType();

				switch (geoType)
				{
					case PxGeometryType::eBOX:
					{
						// box 갱신하는 함수
						PxBoxGeometry newBox;
						assert(AfterSize.x / 2 + extension.x > 0.00001f);
						assert(AfterSize.y / 2 + extension.y > 0.00001f);
						assert(AfterSize.z / 2 + extension.z > 0.00001f);

						newBox.halfExtents = { AfterSize.x / 2 + extension.x,AfterSize.y / 2 + extension.y, AfterSize.z / 2 + extension.z };
						curShape->setGeometry(newBox);
					}
					break;
					case PxGeometryType::eCAPSULE:
					{
						// capsule 갱신하는 함수
						PxCapsuleGeometry newCapsule;

						//가로 세로 중에 큰값을 지름으로 설정.
						float tempradius = AfterSize.x + extension.x;
						if (tempradius <= AfterSize.z + extension.z)
							tempradius = AfterSize.z + extension.z;
						assert(AfterSize.x / 2 + extension.x > 0.00001f);
						assert(AfterSize.z / 2 + extension.z > 0.00001f);

						//반지름 = 지름/2
						tempradius = tempradius / 2;

						// 상자의 절반 길이 = 전체높이/2 - 반지름;
						float tempboxheight = AfterSize.y / 2 + extension.y - tempradius;
						assert(AfterSize.y / 2 + extension.y - tempradius > 0.00001f);
						newCapsule.radius = tempradius;
						newCapsule.halfHeight = tempboxheight;

						curShape->setGeometry(newCapsule);
					}
					break;

					// 아마 cylinder는 custom callback
					case PxGeometryType::eCUSTOM:
					{
						std::shared_ptr <PxCustomGeometryExt::CylinderCallbacks> newCylinder = std::make_shared<PxCustomGeometryExt::CylinderCallbacks>(0.f, 0.f, 0, 0.f);
						//가로 세로 중에 큰값을 지름으로 설정.
						float tempradius = AfterSize.x + extension.x;
						if (tempradius <= AfterSize.z + extension.z)
							tempradius = AfterSize.z + extension.z;
						assert(AfterSize.x / 2 + extension.x > 0.00001f);
						assert(AfterSize.z / 2 + extension.z > 0.00001f);
						//반지름 = 지름/2
						tempradius = tempradius / 2;

						float tempheight = AfterSize.y + extension.y * 2;
						assert(AfterSize.y + extension.y * 2 > 0.00001f);
						newCylinder->setRadius(tempradius);
						newCylinder->setHeight(tempheight);

						curShape->setGeometry(PxCustomGeometry(*newCylinder));
						//m_CylinderTriggerGeometrys.emplace_back(newCylinder);
						m_cylinderGeometrys[ComponentID].emplace_back(newCylinder);
					}
					break;
					default:
						break;
				}
			}
		}
	}
	delete[] shapes;
	/// 이 함수가 호출되기 전 혹은 후에 게임엔지의 transform matrix에도 scale matrix를 곱해야됨
}
void HysxEngine::AddForce(uint32_t ComponentID, const DirectX::SimpleMath::Vector3 forcedir, float force)
{
	PxVec3 temp = { forcedir.x,forcedir.y, forcedir.z };
	addForce(ComponentID, temp, force, eForceType::eFORCE);
}

void HysxEngine::AddForce(uint32_t ComponentID, const DirectX::SimpleMath::Vector3 Force)
{
	auto GameObejectRigidDyamic = static_cast<PxRigidDynamic*> (findPxActorbyCompID(ComponentID));
	PxVec3 Tempforce = { Force.x,Force.y,-Force.z };
	GameObejectRigidDyamic->addForce(Tempforce, PxForceMode::eIMPULSE);
}


uint32_t HysxEngine::RaycastFromPosition_Nearest(VSM::Vector3 pos, VSM::Vector3 dir, float MaxDistance)
{
	dir.Normalize();
	physx::PxRaycastBuffer hit;
	PxVec3 tempPose = { pos.x,pos.y,-pos.z };
	PxVec3 tempDir = { dir.x,dir.y,-dir.z };

	PxRigidActor* collidedActor = nullptr;
	if (m_pScene->raycast(tempPose, tempDir, MaxDistance, hit))
	{
		PxRigidActor* hited = hit.block.actor;
		auto id = findCompIDbyPxActor(hited);

		return findCompIDbyPxActor(hited);
	}
	PxRigidActor* hited = hit.block.actor;
	auto id = findCompIDbyPxActor(hited);
	return 0;
}

std::vector<uint32_t>		HysxEngine::RaycastFromPositions(VSM::Vector3 pos, VSM::Vector3 dir, float MaxDistance)
{
	std::vector<uint32_t> result;

	physx::PxVec3 tempDir = { dir.x,dir.y,-dir.z };
	tempDir.normalize();
	//RayCastInfo info;
	PxF32 max = (PxF32)MaxDistance;

	const PxU32 bufferSize = 32;                 // [in] size of 'hitBuffer'
	PxRaycastHit hitBuffer[bufferSize];          // [out] User provided buffer for results
	PxRaycastBuffer buf(hitBuffer, bufferSize);  // [out] Blocking and touching hits stored here
	PxVec3 tempPose = { pos.x,pos.y,-pos.z };

#ifdef _DEBUG
	DirectX::SimpleMath::Ray ray = { pos, dir * MaxDistance };
	DebugRenderManager::Get().Submit(ray);
#endif

	if (!tempDir.isZero())
	{

		bool find = m_pScene->raycast(
			tempPose,				// 시작점
			tempDir,									// 단위벡터
			PxReal(MaxDistance),                          // 거리
			buf);										  // PxRaycastCallback& hitCall

		for (PxU32 i = 0; i < buf.nbTouches; i++)
		{

			PxVec3 p = buf.getTouch(i).position;
			PxVec3 n = buf.getTouch(i).normal;
			PxF32 d = buf.getTouch(i).distance;
			auto tempID = findCompIDbyPxActor(buf.touches[i].actor);

			if (buf.touches[i].shape->userData != "trigger")
				result.push_back(tempID);
		}
	}
	return result;
}

std::vector<uint32_t>		HysxEngine::RaycastFromPositions_CheckCollision(VSM::Vector3 pos, VSM::Vector3 dir, float MaxDistance)
{
	std::vector<uint32_t> result;

	physx::PxVec3 tempDir = { dir.x,dir.y,-dir.z };
	tempDir.normalize();
	//RayCastInfo info;
	PxF32 max = (PxF32)MaxDistance;

	const PxU32 bufferSize = 32;                 // [in] size of 'hitBuffer'
	PxRaycastHit hitBuffer[bufferSize];          // [out] User provided buffer for results
	PxRaycastBuffer buf(hitBuffer, bufferSize);  // [out] Blocking and touching hits stored here
	PxVec3 tempPose = { pos.x,pos.y,-pos.z };
	if (!tempDir.isZero())
	{

		bool find = m_pScene->raycast(
			tempPose,				// 시작점
			tempDir,									// 단위벡터
			PxReal(MaxDistance),                          // 거리
			buf);										  // PxRaycastCallback& hitCall

		for (PxU32 i = 0; i < buf.nbTouches; i++)
		{

			PxVec3 p = buf.getTouch(i).position;
			PxVec3 n = buf.getTouch(i).normal;
			PxF32 d = buf.getTouch(i).distance;
			auto tempID = findCompIDbyPxActor(buf.touches[i].actor);

			result.push_back(tempID);
		}
	}
	return result;
}

uint32_t HysxEngine::RaycastFromPosition_IgnoreID_Nearest(uint32_t GameobjectID, VSM::Vector3 pos, VSM::Vector3 dir, float MaxDistance)
{
	auto tempActor = findPxActorbyCompID(GameobjectID);
	physx::PxVec3 tempDir = { dir.x,dir.y,-dir.z };
	tempDir.normalize();
	//RayCastInfo info;
	PxF32 max = (PxF32)MaxDistance;

	const PxU32 bufferSize = 32;                 // [in] size of 'hitBuffer'
	PxRaycastHit hitBuffer[bufferSize];          // [out] User provided buffer for results
	PxRaycastBuffer buf(hitBuffer, bufferSize);  // [out] Blocking and touching hits stored here
	PxVec3 tempPose = { pos.x,pos.y,-pos.z };
	if (!tempDir.isZero())
	{

		bool find = m_pScene->raycast(
			tempPose,				// 시작점
			tempDir,									// 단위벡터
			PxReal(MaxDistance),                          // 거리
			buf);										  // PxRaycastCallback& hitCall

		for (PxU32 i = 0; i < buf.nbTouches; i++)
		{

			PxVec3 p = buf.getTouch(i).position;
			PxVec3 n = buf.getTouch(i).normal;
			PxF32 d = buf.getTouch(i).distance;
			auto tempID = findCompIDbyPxActor(buf.touches[i].actor);
			if (tempID != GameobjectID)
			{
				if(buf.touches[i].shape->userData != "trigger")
				return tempID;
			}
		}

		return 0;
	}

	return 0;
}

uint32_t HysxEngine::RaycastFromID_Offset_Nearest(uint32_t GameobjectID, VSM::Vector3 pos, VSM::Vector3 unitDir, float maxDistance)
{
	auto tempActor = findPxActorbyCompID(GameobjectID);
	physx::PxVec3 tempDir = { unitDir.x,unitDir.y,-unitDir.z };
	tempDir.normalize();
	//RayCastInfo info;
	PxF32 max = (PxF32)maxDistance;
	PxVec3 tempPos = PxVec3{ pos.x,pos.y,-pos.z } + tempActor->getGlobalPose().p;
	const PxU32 bufferSize = 32;                 // [in] size of 'hitBuffer'
	PxRaycastHit hitBuffer[bufferSize];          // [out] User provided buffer for results
	PxRaycastBuffer buf(hitBuffer, bufferSize);  // [out] Blocking and touching hits stored here

#ifdef _DEBUG
	DirectX::SimpleMath::Vector3 pos1 = VSM::Vector3{ tempActor->getGlobalPose().p.x, tempActor->getGlobalPose().p.y, -tempActor->getGlobalPose().p.z } + pos;
	DirectX::SimpleMath::Ray ray = { pos1, unitDir * PxReal(maxDistance) };
	DebugRenderManager::Get().Submit(ray);
#endif

	if (!tempDir.isZero())
	{
		bool find = m_pScene->raycast(
			tempPos,				// 시작점
			tempDir,									// 단위벡터
			PxReal(maxDistance),                          // 거리
			buf);										  // PxRaycastCallback& hitCall

		for (PxU32 i = 0; i < buf.nbTouches; i++)
		{

			PxVec3 p = buf.getTouch(i).position;
			PxVec3 n = buf.getTouch(i).normal;
			PxF32 d = buf.getTouch(i).distance;
			auto tempID = findCompIDbyPxActor(buf.touches[i].actor);

			if (tempID != GameobjectID)
				if (buf.touches[i].shape->userData != "trigger")
					return tempID;
		}

		return 0;
	}

	return 0;

}


void HysxEngine::AddImpulse(uint32_t ComponentID, const DirectX::SimpleMath::Vector3 forcedir, float force)
{
	PxVec3 temp = { forcedir.x,forcedir.y, forcedir.z };
	addForce(ComponentID, temp, force, eForceType::eIMPULSE);
}

void HysxEngine::AddVelocity(uint32_t ComponentID, const DirectX::SimpleMath::Vector3 forcedir, float force)
{
	PxVec3 temp = { forcedir.x,forcedir.y, forcedir.z };
	addForce(ComponentID, temp, force, eForceType::eVELOCITY_CHANGE);
}

void HysxEngine::AddAcceleration(uint32_t ComponentID, const DirectX::SimpleMath::Vector3 forcedir, float force)
{
	PxVec3 temp = { forcedir.x,forcedir.y, forcedir.z };
	addForce(ComponentID, temp, force, eForceType::eACCELERATION);
}

const float HysxEngine::GetCurrentSpeed_x(uint32_t ComponentID)
{
	if (findPxActorbyCompID(ComponentID)->getType() == PxActorType::eRIGID_DYNAMIC)
	{
		auto GameObejectRigidDyamic = static_cast<PxRigidDynamic*>(findPxActorbyCompID(ComponentID));
		physx::PxVec3 linearVelocity = GameObejectRigidDyamic->getLinearVelocity();
		return linearVelocity.x;
	}
	return 0;
}

const float HysxEngine::GetCurrentSpeed_y(uint32_t ComponentID)
{
	if (findPxActorbyCompID(ComponentID)->getType() == PxActorType::eRIGID_DYNAMIC)
	{
		auto GameObejectRigidDyamic = static_cast<PxRigidDynamic*>(findPxActorbyCompID(ComponentID));
		physx::PxVec3 linearVelocity = GameObejectRigidDyamic->getLinearVelocity();
		return linearVelocity.y;
	}
	return 0;
}

const float HysxEngine::GetCurrenSpeed_z(uint32_t ComponentID)
{
	if (findPxActorbyCompID(ComponentID)->getType() == PxActorType::eRIGID_DYNAMIC)
	{
		auto GameObejectRigidDyamic = static_cast<PxRigidDynamic*>(findPxActorbyCompID(ComponentID));
		physx::PxVec3 linearVelocity = GameObejectRigidDyamic->getLinearVelocity();
		return -linearVelocity.z;
	}
	return 0;
}

const Vector3 HysxEngine::GetCurrentVelocity(uint32_t ComponentID)
{
	if (findPxActorbyCompID(ComponentID)->getType() == PxActorType::eRIGID_DYNAMIC)
	{
		auto GameObejectRigidDyamic = static_cast<PxRigidDynamic*>(findPxActorbyCompID(ComponentID));
		physx::PxVec3 linearVelocity = GameObejectRigidDyamic->getLinearVelocity();
		DirectX::SimpleMath::Vector3 result = { linearVelocity.x,linearVelocity.y,-linearVelocity.z };
		return result;
	}

	return { 0,0,0 };
}


const Vector3 HysxEngine::GetCurrentVelocity_dir_xz(uint32_t ComponentID)
{
	if (findPxActorbyCompID(ComponentID)->getType() == PxActorType::eRIGID_DYNAMIC)
	{
		auto GameObejectRigidDyamic = static_cast<PxRigidDynamic*>(findPxActorbyCompID(ComponentID));
		physx::PxVec3 linearVelocity = GameObejectRigidDyamic->getLinearVelocity();
		linearVelocity.y = 0;
		linearVelocity.normalize();
		DirectX::SimpleMath::Vector3 result = { linearVelocity.x,linearVelocity.y,-linearVelocity.z };
		return result;
	}
	return { 0,0,0 };
}

const Vector3 HysxEngine::GetCurrentVelocity_dir_xyz(uint32_t ComponentID)
{
	if (findPxActorbyCompID(ComponentID)->getType() == PxActorType::eRIGID_DYNAMIC)
	{
		auto GameObejectRigidDyamic = static_cast<PxRigidDynamic*>(findPxActorbyCompID(ComponentID));
		physx::PxVec3 linearVelocity = GameObejectRigidDyamic->getLinearVelocity();
		linearVelocity.normalize();
		DirectX::SimpleMath::Vector3 result = { linearVelocity.x,linearVelocity.y,-linearVelocity.z };
		return result;
	}
	return { 0,0,0 };
}

const Vector3 HysxEngine::GetGolbalTranslation(uint32_t ComponentID)
{
	if (findPxActorbyCompID(ComponentID))
	{
		auto GameObejectRigidDyamic = findPxActorbyCompID(ComponentID);

		PxVec3 tempPose = GameObejectRigidDyamic->getGlobalPose().p;
		Vector3 result = { tempPose.x,tempPose.y,-tempPose.z };
		return result;
	}
	else
	{
		assert(findPxActorbyCompID(ComponentID) != nullptr);
		return { 0,0,0 };
	}
}

void HysxEngine::SetSpeedToCurrentVelocity_xz(uint32_t ComponentID, float velocity)
{
	if (findPxActorbyCompID(ComponentID)->getType() == PxActorType::eRIGID_DYNAMIC)
	{
		auto GameObejectRigidDyamic = static_cast<PxRigidDynamic*>(findPxActorbyCompID(ComponentID));
		physx::PxVec3 linearVelocity = GameObejectRigidDyamic->getLinearVelocity();
		physx::PxVec3 result;
		result.y = linearVelocity.y;
		linearVelocity.y = 0;
		linearVelocity.normalize();
		linearVelocity = linearVelocity * velocity;
		result.x = linearVelocity.x;
		result.z = linearVelocity.z;
		GameObejectRigidDyamic->setLinearVelocity(result);
		return;
	}
	else
	{
		assert(findPxActorbyCompID(ComponentID)->getType() != PxActorType::eRIGID_DYNAMIC);
		return;
	}

}

void HysxEngine::SetDirection_xz(uint32_t ComponentID, Vector3 dir)
{
	if (findPxActorbyCompID(ComponentID)->getType() == PxActorType::eRIGID_DYNAMIC)
	{
		auto GameObejectRigidDyamic = static_cast<PxRigidDynamic*>(findPxActorbyCompID(ComponentID));
		physx::PxVec3 currentVelocity = GameObejectRigidDyamic->getLinearVelocity();
		physx::PxVec3 resultVelocity = { dir.x ,0 ,-dir.z };
		resultVelocity.normalize();
		resultVelocity.y = currentVelocity.y;
		currentVelocity.y = 0;
		float velocity = currentVelocity.magnitude();
		resultVelocity.x = resultVelocity.x * velocity;
		resultVelocity.z = resultVelocity.z * velocity;

		GameObejectRigidDyamic->setLinearVelocity(resultVelocity);
		return;
	}
	else
	{
		assert(findPxActorbyCompID(ComponentID)->getType() != PxActorType::eRIGID_DYNAMIC);
		return;
	}

}

void HysxEngine::SetVelocity(uint32_t ComponentID, Vector3 dir, float speed)
{
	assert(findPxActorbyCompID(ComponentID) != nullptr);

	if (findPxActorbyCompID(ComponentID)->getType() == PxActorType::eRIGID_DYNAMIC)
	{
		auto GameObejectRigidDyamic = static_cast<PxRigidDynamic*>(findPxActorbyCompID(ComponentID));
		physx::PxVec3 resultVelocity = { dir.x ,dir.y ,-dir.z };
		resultVelocity.normalize();
		resultVelocity = resultVelocity * speed;
		GameObejectRigidDyamic->setLinearVelocity(resultVelocity);
		return;
	}

	else
	{
		assert(findPxActorbyCompID(ComponentID)->getType() != PxActorType::eRIGID_DYNAMIC);
		return;
	}

}

void HysxEngine::SetVelocity(uint32_t ComponentID, Vector3 velocity)
{
	assert(findPxActorbyCompID(ComponentID) != nullptr);

	if (findPxActorbyCompID(ComponentID)->getType() == PxActorType::eRIGID_DYNAMIC)
	{
		auto GameObejectRigidDyamic = static_cast<PxRigidDynamic*>(findPxActorbyCompID(ComponentID));
		//GameObejectRigidDyamic->setLinearDamping(0.0f);
		PxVec3 resultVelocity = { velocity.x,velocity.y,-velocity.z };
		GameObejectRigidDyamic->setLinearVelocity(resultVelocity);
		return;
	}

	else
	{
		assert(findPxActorbyCompID(ComponentID)->getType() != PxActorType::eRIGID_DYNAMIC);
		return;
	}
}

void HysxEngine::SetGolbalTranslation(uint32_t ComponentID, Vector3 pose)
{
	if (findPxActorbyCompID(ComponentID)->getType() == PxActorType::eRIGID_DYNAMIC)
	{
		auto GameObjectRigidDyamic = static_cast<PxRigidDynamic*>(findPxActorbyCompID(ComponentID));
		PxVec3 pxDir = { pose.x,pose.y,-pose.z };
		PxTransform pxTransform = GameObjectRigidDyamic->getGlobalPose();
		pxTransform.p = pxDir;
		// 일단 빼본다..
		//GameObjectRigidDyamic->setLinearVelocity({});
		GameObjectRigidDyamic->setGlobalPose(pxTransform);
		return;
	}

	else if (findPxActorbyCompID(ComponentID)->getType() == PxActorType::eRIGID_STATIC)
	{
		auto GameObjectRigidDyamic = static_cast<PxRigidStatic*>(findPxActorbyCompID(ComponentID));
		PxVec3 pxDir = { pose.x,pose.y,-pose.z };
		PxTransform pxTransform = GameObjectRigidDyamic->getGlobalPose();
		pxTransform.p = pxDir;
		GameObjectRigidDyamic->setGlobalPose(pxTransform);
		return;
	}

}

void HysxEngine::SetGolbalTransform(uint32_t ComponentID, const DirectX::SimpleMath::Matrix& simpleMathMatrix)
{
	physx::PxTransform TransformofTranscomp = Physics::MatrixToPxTransform(simpleMathMatrix);
	if (auto actor = findPxActorbyCompID(ComponentID))
		actor->setGlobalPose(TransformofTranscomp);
}

void HysxEngine::SetGolbalTransform(uint32_t ComponentID, const DirectX::SimpleMath::Vector3& pos, const DirectX::SimpleMath::Quaternion& quat)
{
	PxVec3 tempPos = { pos.x, pos.y, -pos.z };
	physx::PxQuat tempQuat = physx::PxQuat(-quat.x, -quat.y, quat.z, quat.w);

	if (auto actor = findPxActorbyCompID(ComponentID))
	{
		actor->setGlobalPose({ tempPos, tempQuat });
	}

}

void HysxEngine::SetMaxSpeed(uint32_t ComponentID, float MaxVelocity)
{
	if (findPxActorbyCompID(ComponentID)->getType() == PxActorType::eRIGID_DYNAMIC)
	{
		auto GameObejectRigidDyamic = static_cast<PxRigidDynamic*>(findPxActorbyCompID(ComponentID));
		GameObejectRigidDyamic->setMaxLinearVelocity(MaxVelocity);
		return;
	}
	else
	{
		assert(findPxActorbyCompID(ComponentID)->getType() != PxActorType::eRIGID_DYNAMIC);
		return;
	}
}

float HysxEngine::GetMaxSpeed(uint32_t ComponentID, float MaxVelocity)
{
	if (findPxActorbyCompID(ComponentID)->getType() == PxActorType::eRIGID_DYNAMIC)
	{
		auto GameObejectRigidDyamic = static_cast<PxRigidDynamic*>(findPxActorbyCompID(ComponentID));
		return GameObejectRigidDyamic->getMaxLinearVelocity();
	}
	else
	{
		assert(findPxActorbyCompID(ComponentID)->getType() != PxActorType::eRIGID_DYNAMIC);
		return{};
	}
}

void HysxEngine::ClearForce(uint32_t ComponentID)
{
	if (findPxActorbyCompID(ComponentID)->getType() == PxActorType::eRIGID_DYNAMIC)
	{
		auto GameObejectRigidDyamic = static_cast<PxRigidDynamic*>(findPxActorbyCompID(ComponentID));
		GameObejectRigidDyamic->clearForce();
	}
	else
		assert(findPxActorbyCompID(ComponentID)->getType() != PxActorType::eRIGID_DYNAMIC);
	return;
}
bool HysxEngine::HaveRigidActor(uint32_t GameobjectID)
{
	auto temp = findPxActorbyCompID(GameobjectID);

	if (temp == nullptr)
	{
		return false;
	}

	return true;
}
void HysxEngine::ReleaseActor(uint32_t GameobjectID)
{
	auto actor = findPxActorbyCompID(GameobjectID);
	unsigned int numShapes = actor->getNbShapes();
	std::vector<physx::PxShape*> shapes(numShapes);
	actor->getShapes(&shapes[0], numShapes);
	for (auto& shape : shapes)
	{
		delete static_cast<std::string*>(shape->userData);

	}
	delete static_cast<ActorState*>(actor->userData);

	m_PhysicsActors.erase(GameobjectID);
	PX_RELEASE(actor);

	m_cylinderGeometrys.erase(GameobjectID);

	m_CollisionStayLists.erase(GameobjectID);
	while (!m_CollisionEnterDeque.empty())
	{
		m_CollisionEnterDeque.pop_front();
	}

	while (!m_CollisionExitDeque.empty())
	{
		m_CollisionExitDeque.pop_front();
	}
	//mcal
}

void HysxEngine::ReleaseAllActor()
{
	m_CollisionStayLists.clear();

	while (!m_CollisionEnterDeque.empty())
	{
		m_CollisionEnterDeque.pop_front();
	}

	while (!m_CollisionExitDeque.empty())
	{
		m_CollisionExitDeque.pop_front();
	}

	for (auto& actor : m_PhysicsActors)
	{
		if (actor.second)
		{
			unsigned int numShapes = actor.second->getNbShapes();
			std::vector<physx::PxShape*> shapes(numShapes);
			actor.second->getShapes(&shapes[0], numShapes);
			for (auto& shape : shapes)
			{
				delete static_cast<std::string*>(shape->userData);
			}
			delete static_cast<ActorState*>(actor.second->userData);
			PX_RELEASE(actor.second);
		}
	}
	m_PhysicsActors.clear();

	m_cylinderGeometrys.clear();

	ReleaseCallback();
}

void HysxEngine::ReleaseCallback()
{
	m_pScene->setSimulationEventCallback(nullptr);
	//delete m_pCollisionCallback;
}

void HysxEngine::SetCallback()
{
	if (!m_pScene->getSimulationEventCallback())
	{
		m_pCollisionCallback = std::make_shared<CollisionCallback>();
		m_pScene->setSimulationEventCallback(m_pCollisionCallback.get());
	}
}

void HysxEngine::DeletePxScene()
{
	if (m_pScene != nullptr)
	{
		int num = m_pScene->getNbActors(PxActorTypeFlag::eRIGID_STATIC | PxActorTypeFlag::eRIGID_DYNAMIC);

		m_pScene->setSimulationEventCallback(nullptr);
		//delete m_pCollisionCallback;

		PX_RELEASE(m_pScene);
		PX_RELEASE(m_pDispatcher);
	}
}

void HysxEngine::CreatePxScene()
{
#ifdef _DEBUG
	bool currentState = m_pPvd->isConnected();
	if (!currentState)
	{
		m_pPvd->connect(*m_pTransport, PxPvdInstrumentationFlag::eALL);
	}
#endif

	if (!m_pScene)
	{
		// PhysX Scene 설정
		PxSceneDesc sceneDesc = m_pPhysics->getTolerancesScale();
		sceneDesc.gravity = PxVec3(0.0f, -9.81, 0.0f);

		// CPU 디스패처 생성 및 할당
		m_pDispatcher = PxDefaultCpuDispatcherCreate(2);
		sceneDesc.cpuDispatcher = m_pDispatcher;

		sceneDesc.filterShader = Physics::CustomFilterShader;
		sceneDesc.flags |= PxSceneFlag::eENABLE_ACTIVE_ACTORS;
		sceneDesc.flags |= PxSceneFlag::eENABLE_STABILIZATION;

		// PhysX Scene 생성
		m_pScene = m_pPhysics->createScene(sceneDesc);
		m_pScene->setVisualizationParameter(PxVisualizationParameter::eCOLLISION_SHAPES, 1.0f);
		m_pScene->setVisualizationParameter(PxVisualizationParameter::eSCALE, 1.0f);

		m_pCollisionCallback = std::make_shared<CollisionCallback>();
		m_pScene->setSimulationEventCallback(m_pCollisionCallback.get());
	}
}

void HysxEngine::SetDamping(uint32_t gameobjectID, float force)
{
	auto curActor = findPxActorbyCompID(gameobjectID);
	if (curActor->getType() == PxActorType::eRIGID_DYNAMIC)
	{
		auto curDynamic = static_cast<PxRigidDynamic*>(curActor);
		curDynamic->setLinearDamping(force);
	}
}

void HysxEngine::SetGravity(VSM::SimpleMath::Vector3 gravity)
{
	if (m_pScene)
	{
		m_pScene->setGravity({ gravity.x,gravity.y, gravity.z });
	}
}

VSM::SimpleMath::Vector3 HysxEngine::GetGravity()
{
	return { m_pScene->getGravity().x, m_pScene->getGravity().y, -m_pScene->getGravity().z };
}

VSM::Quaternion HysxEngine::GetQuaternion(uint32_t gameobjectID)
{
	auto curActor = findPxActorbyCompID(gameobjectID);
	PxQuat quat = curActor->getGlobalPose().q;
	return { quat.x, quat.y, -quat.z, quat.w };
}

uint32_t HysxEngine::RaycastFromID_Nearest(uint32_t GameobjectID, VSM::Vector3 unitDir, float maxDistance)
{
	// 	RayCastInfo info;
	// 	
	// 	PxRaycastBuffer hit;  // [out] User provided buffer for results
	// 	bool find = m_scene->raycast(
	// 		PxVec3(origin.x, origin.y, origin.z),         // 시작점
	// 		PxVec3(unitDir.x, unitDir.y, unitDir.z),      // 단위벡터
	// 		PxReal(maxDistance), hit);
	auto tempActor = findPxActorbyCompID(GameobjectID);
	physx::PxVec3 tempDir = { unitDir.x,unitDir.y,-unitDir.z };
	tempDir.normalize();
	//RayCastInfo info;
	PxF32 max = (PxF32)maxDistance;

	const PxU32 bufferSize = 32;                 // [in] size of 'hitBuffer'
	PxRaycastHit hitBuffer[bufferSize];          // [out] User provided buffer for results
	PxRaycastBuffer buf(hitBuffer, bufferSize);  // [out] Blocking and touching hits stored here
	if (!tempDir.isZero())
	{
		// 		DirectX::SimpleMath::Vector3 pos = {tempActor->getGlobalPose().p.x, tempActor->getGlobalPose().p.y, -tempActor->getGlobalPose().p.z};
		// 		DirectX::SimpleMath::Ray ray = { pos, unitDir * PxReal(maxDistance) };
		// 		DebugRenderManager::Get().Submit(ray);


		bool find = m_pScene->raycast(
			tempActor->getGlobalPose().p,				// 시작점
			tempDir,									// 단위벡터
			PxReal(maxDistance),                          // 거리
			buf);										  // PxRaycastCallback& hitCall
		for (PxU32 i = 0; i < buf.nbTouches; i++)
		{

			PxVec3 p = buf.getTouch(i).position;
			PxVec3 n = buf.getTouch(i).normal;
			PxF32 d = buf.getTouch(i).distance;
			auto tempID = findCompIDbyPxActor(buf.touches[i].actor);

			if (tempID != GameobjectID)
				if (buf.touches[i].shape->userData != "trigger")
					return tempID;
		}

		return 0;
	}

	return 0;
}

#pragma endregion

#pragma region Private Functions

void HysxEngine::initialize_PhysxMaterial()
{
	addMaterial(ePxMaterial::FLOOR_MATERIAL, 0.4f, 0.15f, 0);
	addMaterial(ePxMaterial::PLAYER_MATERIAL, .4f, .15f, 0);
	addMaterial(ePxMaterial::BOX_MATERIAL, .4f, .15f, 0);
	addMaterial(ePxMaterial::NOFRICTION_MATERIAL, 0.f, 0.f, 0);

}

void HysxEngine::initialize_PxFilterDatas()
{
	PxFilterData* TILE_LAYER = new PxFilterData;
	PxFilterData* TRIGGER_TILE_LAYER = new PxFilterData;
	PxFilterData* CHARACTER_LAYER = new PxFilterData;
	PxFilterData* NO_COLLIDE_LAYER = new PxFilterData;
	PxFilterData* COLLISION_LAYER = new PxFilterData;

	TILE_LAYER->word0 = ePxLayer::TILE_LAYER;
	TILE_LAYER->word1 = ePxLayer::CHARACTER_LAYER | /*ePxLayer::TILE_LAYER | */ePxLayer::TRIGGER_TILE_LAYER;
	//TILE_LAYER->word3 = ePxLayer::COLLISION_DETECTION_LAYER;

	TRIGGER_TILE_LAYER->word0 = ePxLayer::TRIGGER_TILE_LAYER;
	TRIGGER_TILE_LAYER->word1 = ePxLayer::CHARACTER_LAYER | ePxLayer::TILE_LAYER | ePxLayer::TRIGGER_TILE_LAYER;

	CHARACTER_LAYER->word0 = ePxLayer::CHARACTER_LAYER;
	CHARACTER_LAYER->word1 = ePxLayer::TILE_LAYER | ePxLayer::TRIGGER_TILE_LAYER;

	NO_COLLIDE_LAYER->word0 = ePxLayer::NO_COLLISION_LAYER;

	// 감지만을 위한 충돌 그룹
	COLLISION_LAYER->word3 = ePxLayer::COLLISION_DETECTION_LAYER;


	m_PxFilterDatas.insert(make_pair(ePxLayer::TILE_LAYER, TILE_LAYER));
	m_PxFilterDatas.insert(make_pair(ePxLayer::TRIGGER_TILE_LAYER, TRIGGER_TILE_LAYER));
	m_PxFilterDatas.insert(make_pair(ePxLayer::CHARACTER_LAYER, CHARACTER_LAYER));
	m_PxFilterDatas.insert(make_pair(ePxLayer::NO_COLLISION_LAYER, NO_COLLIDE_LAYER));
	m_PxFilterDatas.insert(make_pair(ePxLayer::COLLISION_DETECTION_LAYER, COLLISION_LAYER));
}

void HysxEngine::addMaterial(ePxMaterial ePxMaterialType, float staticFriction, float dynamicFriction, float restitution)
{
	if (m_PxMaterials[ePxMaterialType] == nullptr)
	{
		physx::PxMaterial* newMaterial = nullptr;
		newMaterial = m_pPhysics->createMaterial(staticFriction, dynamicFriction, restitution);
		//physx::PxMaterial* temp = nullptr;		

		m_PxMaterials[ePxMaterialType] = newMaterial;
	}
	else
	{
		OutputDebugStringA("같은 이름의 PhysicsMaterial(이)가 추가 되었습니다.\n");
	}
}

physx::PxMaterial* HysxEngine::findMaterial(ePxMaterial eMaterial)
{
	assert(m_PxMaterials[eMaterial] != nullptr);
	return m_PxMaterials[eMaterial];
}

physx::PxFilterData* HysxEngine::findPxFilterData(ePxLayer Layer)
{
	assert(m_PxFilterDatas[Layer] != nullptr);

	return m_PxFilterDatas[Layer];
}

void HysxEngine::makeCapsuleCollider(uint32_t ComponentID, unsigned int material, unsigned int physicsLayer, unsigned int actorType, bool Lock_lvX, bool Lock_lvY, bool Lock_lvZ, bool Lock_avX, bool Lock_avY, bool Lock_avZ, unsigned int shapeType, Vector3 Size, float weight, Vector3 Scale, DirectX::SimpleMath::Matrix transform, bool isCharModel, bool noGravity)
{
	float tempradius = Size.x * Scale.x;
	if (tempradius <= Size.z * Scale.z)
		tempradius = Size.z * Scale.z;
	//반지름 = 지름/2
	tempradius = tempradius / 2;

	// 상자의 절반 길이 = 전체높이/2 - 반지름;
	float tempboxheight = Size.y * Scale.y / 2 - tempradius;
	PxCapsuleGeometry capsule = PxCapsuleGeometry(tempradius, tempboxheight);

	physx::PxTransform pose = Physics::MatrixToPxTransform(transform); //Pose		생각해보니 이걸 왜 만들었지??? 
	physx::PxMaterial* tempMaterial = findMaterial((ePxMaterial)material);		//material
	physx::PxFilterData* tempPxFilterData = findPxFilterData((ePxLayer)physicsLayer);	//filterdata
	// 회전 축 설정 (여기서는 x축 주위로 회전하므로 PxVec3(1, 0, 0)을 사용)
	const PxReal rotationAngle = PxPi / 2.0f;
	PxVec3 rotationAxis(.0f, .0f, 1.0f);
	PxQuat rotationQuaternion(rotationAngle, rotationAxis);
	// 새로운 PxTransform 객체 생성 및 원래의 위치 정보 및 새로운 회전 정보 적용\

	if (isCharModel)
	{
		pose.p.y -= 0.6f;
	}


	PxTransform transformedTransform(pose.p, rotationQuaternion);
	switch (actorType)
	{
		case eActorType::STATIC_ACTORTYPE:
		{
			PxRigidStatic* actor = m_pPhysics->createRigidStatic(transformedTransform);
			PxShape* shape = PxRigidActorExt::createExclusiveShape(*actor, capsule, *tempMaterial);

			shape->setSimulationFilterData(*tempPxFilterData);
			shape->setContactOffset(1.0f);
			//shape->setRestOffset(0.5f);
			m_pScene->addActor(*actor);
			ActorState* curState = new ActorState;
			curState->collisionState = eCollisionState::ON_COLLISION_NONE;
			curState->myCompID = ComponentID;
			actor->userData = curState;
			m_CollisionStayLists.insert({ ComponentID, std::unordered_set<uint32_t>(ADJ_LIST_SIZE) });

			m_PhysicsActors.insert(std::make_pair(ComponentID, actor));

		}
		break;
		case eActorType::DYNAMIC_ACTORTYPE:
		{
			PxRigidDynamic* actor = m_pPhysics->createRigidDynamic(transformedTransform);
			PxShape* shape = PxRigidActorExt::createExclusiveShape(*actor, capsule, *tempMaterial);

			shape->setSimulationFilterData(*tempPxFilterData);
			shape->setContactOffset(1.0f);
			//shape->setRestOffset(0.5f);
			float tempdensity = weight / ((tempboxheight * tempboxheight * tempboxheight * 8) + ((4.0f / 3.0f) * tempradius * tempradius * tempradius * PxPi * PxPi * PxPi));
			PxRigidBodyExt::updateMassAndInertia(*actor, tempdensity);

			actor->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, noGravity);
			actor->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD, true);


			m_pScene->addActor(*actor);

			// LOCK 설정	
			actor->setRigidDynamicLockFlags(
				(PxRigidDynamicLockFlag::Enum)(Lock_lvX << 0) |
				(PxRigidDynamicLockFlag::Enum)(Lock_lvY << 1) |
				(PxRigidDynamicLockFlag::Enum)(Lock_lvZ << 2) |
				(PxRigidDynamicLockFlag::Enum)(Lock_avX << 3) |
				(PxRigidDynamicLockFlag::Enum)(Lock_avY << 4) |
				(PxRigidDynamicLockFlag::Enum)(Lock_avZ << 5));
			ActorState* curState = new ActorState;
			curState->collisionState = eCollisionState::ON_COLLISION_NONE;
			curState->myCompID = ComponentID;
			actor->userData = curState;
			m_CollisionStayLists.insert({ ComponentID, std::unordered_set<uint32_t>(ADJ_LIST_SIZE) });

			m_PhysicsActors.insert(std::make_pair(ComponentID, actor));
		}
		break;
		default:
		{}
		break;
	}
}
void HysxEngine::makeCapsuleCollision(uint32_t ComponentID, unsigned int eMaterial, Vector3 Size, Vector3 Scale, Vector3 extention)
{
	assert(Size.x * Scale.x / 2 + extention.x > 0.00001);
	assert(Size.y * Scale.y / 2 + extention.y > 0.00001);
	assert(Size.z * Scale.z / 2 + extention.z > 0.00001);

	//가로 세로 중에 큰값을 지름으로 설정.
	float tempradius = Size.x * Scale.x + extention.x;
	if (tempradius <= (Size.z * Scale.z + extention.z))
		tempradius = Size.z * Scale.z + extention.z;
	//반지름 = 지름/2
	tempradius = tempradius / 2;

	// 상자의 절반 길이 = 전체높이/2 - 반지름;
	float tempboxheight = Size.y * Scale.y / 2 + extention.y - tempradius;
	assert(Size.y * Scale.y / 2 + extention.y - tempradius > 0.00001);
	PxCapsuleGeometry capsule = PxCapsuleGeometry(tempradius, tempboxheight);

	auto currentActor = findPxActorbyCompID(ComponentID);
	physx::PxMaterial* tempMaterial = findMaterial((ePxMaterial)eMaterial);		//material
	PxShape* triggerShape = createTriggerShape(capsule, tempMaterial, true, FILTER_SHADER);
	triggerShape->setSimulationFilterData(*m_PxFilterDatas[ePxLayer::COLLISION_DETECTION_LAYER]);
	std::string* name = new std::string;
	*name = "trigger";
	triggerShape->userData = name;
	currentActor->attachShape(*triggerShape);

	//bool isTriggerShape = triggerShape->getFlags() & PxShapeFlag::eTRIGGER_SHAPE;
	//if (!isTriggerShape)
	//{
	//	triggerShape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
	//}
}
void HysxEngine::makeCylinderCollider(uint32_t ComponentID, unsigned int material, unsigned int physicsLayer, unsigned int actorType, bool Lock_lvX, bool Lock_lvY, bool Lock_lvZ, bool Lock_avX, bool Lock_avY, bool Lock_avZ, unsigned int shapeType, Vector3 Size, float weight, Vector3 Scale, DirectX::SimpleMath::Matrix transform, bool isCharModel, bool noGravity)
{
	//가로 세로 중에 큰값을 지름으로 설정.
	float tempradius = Size.x * Scale.x;
	if (tempradius <= Size.z * Scale.z)
		tempradius = Size.z * Scale.z;
	//반지름 = 지름/2
	tempradius = tempradius / 2;

	float tempheight = Size.y * Scale.y;
	std::shared_ptr <PxCustomGeometryExt::CylinderCallbacks> cylinder = std::make_shared<PxCustomGeometryExt::CylinderCallbacks>(tempheight, tempradius, 0, 0.f);

	physx::PxTransform pose = Physics::MatrixToPxTransform(transform); //Pose
	physx::PxMaterial* tempMaterial = findMaterial((ePxMaterial)material);		//material
	physx::PxFilterData* tempPxFilterData = findPxFilterData((ePxLayer)physicsLayer);	//filterdata
	// 회전 축 설정 (여기서는 x축 주위로 회전하므로 PxVec3(1, 0, 0)을 사용)
	const PxReal rotationAngle = PxPi / 2.0f;
	PxVec3 rotationAxis(.0f, .0f, 1.0f);
	PxQuat rotationQuaternion(rotationAngle, rotationAxis);

	if (isCharModel)
	{
		pose.p.y -= 0.6f;
	}

	// 새로운 PxTransform 객체 생성 및 원래의 위치 정보 및 새로운 회전 정보 적용
	PxTransform transformedTransform(pose.p, rotationQuaternion);


	switch (actorType)
	{
		case eActorType::STATIC_ACTORTYPE:
		{
			PxRigidStatic* actor = m_pPhysics->createRigidStatic(transformedTransform);
			PxShape* shape = PxRigidActorExt::createExclusiveShape(*actor, PxCustomGeometry(*cylinder), *tempMaterial);
			shape->setSimulationFilterData(*tempPxFilterData);
			shape->setContactOffset(1.0f);
			//shape->setRestOffset(0.5f);

			ActorState* curState = new ActorState;
			curState->collisionState = eCollisionState::ON_COLLISION_NONE;
			curState->myCompID = ComponentID;
			actor->userData = curState;
			m_CollisionStayLists.insert({ ComponentID, std::unordered_set<uint32_t>(ADJ_LIST_SIZE) });
			m_pScene->addActor(*actor);
			// actor들 관리할 자료형 있어야 될듯?
			m_PhysicsActors.insert(std::make_pair(ComponentID, actor));

			// 없으면 만들기
			if (m_cylinderGeometrys.find(ComponentID) != m_cylinderGeometrys.end())
			{
				m_cylinderGeometrys.insert({ ComponentID, std::list<std::shared_ptr<physx::PxCustomGeometryExt::CylinderCallbacks>>() });
			}

			// 추가
			m_cylinderGeometrys[ComponentID].emplace_back(cylinder);
		}
		break;
		case eActorType::DYNAMIC_ACTORTYPE:
		{
			PxRigidDynamic* actor = m_pPhysics->createRigidDynamic(transformedTransform);
			PxShape* shape = PxRigidActorExt::createExclusiveShape(*actor, PxCustomGeometry(*cylinder), *tempMaterial);
			shape->setSimulationFilterData(*tempPxFilterData);
			shape->setContactOffset(1.0f);
			//shape->setRestOffset(0.5f);
			float tempdensity = weight / (tempheight * PxPi * tempradius * tempradius);
			PxRigidBodyExt::updateMassAndInertia(*actor, tempdensity);

			ActorState* curState = new ActorState;
			curState->collisionState = eCollisionState::ON_COLLISION_NONE;
			curState->myCompID = ComponentID;

			actor->userData = curState;
			actor->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, noGravity);

			m_CollisionStayLists.insert({ ComponentID, std::unordered_set<uint32_t>(ADJ_LIST_SIZE) });


			m_pScene->addActor(*actor);
			// LOCK 설정	
			actor->setRigidDynamicLockFlags(
				(PxRigidDynamicLockFlag::Enum)(Lock_lvX << 0) |
				(PxRigidDynamicLockFlag::Enum)(Lock_lvY << 1) |
				(PxRigidDynamicLockFlag::Enum)(Lock_lvZ << 2) |
				(PxRigidDynamicLockFlag::Enum)(Lock_avX << 3) |
				(PxRigidDynamicLockFlag::Enum)(Lock_avY << 4) |
				(PxRigidDynamicLockFlag::Enum)(Lock_avZ << 5));
			// actor들 관리할 자료형 있어야 될듯?
			m_PhysicsActors.insert(std::make_pair(ComponentID, actor));

			// 없으면 만들기
			if (m_cylinderGeometrys.find(ComponentID) != m_cylinderGeometrys.end())
			{
				m_cylinderGeometrys.insert({ ComponentID, std::list<std::shared_ptr<physx::PxCustomGeometryExt::CylinderCallbacks>>() });
			}

			// 추가
			m_cylinderGeometrys[ComponentID].emplace_back(cylinder);
		}
		break;
		default:
		{}
		break;
	}





}
void HysxEngine::makeCylinderCollision(uint32_t ComponentID, unsigned int eMaterial, Vector3 Size, Vector3 Scale, Vector3 extention)
{
	//가로 세로 중에 큰값을 지름으로 설정.
	assert(Size.x * Scale.x + extention.x > 0.00001);
	assert(Size.y * Scale.y + extention.y * 2 > 0.00001);
	assert(Size.z * Scale.z + extention.z > 0.00001);
	float tempradius = Size.x * Scale.x + extention.x;
	if (tempradius <= (Size.z * Scale.z + extention.z))
		tempradius = Size.z * Scale.z + extention.z;
	//반지름 = 지름/2
	tempradius = tempradius / 2;

	float tempheight = Size.y * Scale.y + extention.y/* * 2*/;
	std::shared_ptr<PxCustomGeometryExt::CylinderCallbacks> cylinder = std::make_shared<PxCustomGeometryExt::CylinderCallbacks>(tempheight, tempradius, 0, 0.f);

	auto currentActor = findPxActorbyCompID(ComponentID);
	physx::PxMaterial* tempMaterial = findMaterial((ePxMaterial)eMaterial);		//material
	PxShape* triggerShape = createTriggerShape(PxCustomGeometry(*cylinder), tempMaterial, true, FILTER_SHADER);
	triggerShape->setSimulationFilterData(*m_PxFilterDatas[ePxLayer::COLLISION_DETECTION_LAYER]);
	std::string* name = new std::string;
	*name = "trigger";
	triggerShape->userData = name;
	currentActor->attachShape(*triggerShape);

	//m_CylinderTriggerGeometrys.emplace_back(cylinder);
	// 없으면 만들기
	if (m_cylinderGeometrys.find(ComponentID) != m_cylinderGeometrys.end())
	{
		m_cylinderGeometrys.insert({ ComponentID, std::list<std::shared_ptr<physx::PxCustomGeometryExt::CylinderCallbacks>>() });
	}

	// 추가
	m_cylinderGeometrys[ComponentID].emplace_back(cylinder);
	//cylinder

	//bool isTriggerShape = triggerShape->getFlags() & PxShapeFlag::eTRIGGER_SHAPE;
	//if (!isTriggerShape)
	//{
	//	triggerShape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
	//}

}

void HysxEngine::makeBoxCollider(uint32_t ComponentID, unsigned int material, unsigned int physicsLayer, unsigned int actorType, bool Lock_lvX, bool Lock_lvY, bool Lock_lvZ, bool Lock_avX, bool Lock_avY, bool Lock_avZ, unsigned int shapeType, Vector3 Size, float weight, Vector3 Scale, DirectX::SimpleMath::Matrix transform, bool isCharModel, bool noGravity)
{
	physx::PxTransform tempPose = Physics::MatrixToPxTransform(transform); //Pose
	const PxVec3 tempHalfSize = { (Size.x / 2 * Scale.x), (Size.y / 2 * Scale.y),(Size.z / 2 * Scale.z) };

	physx::PxMaterial* tempMaterial = findMaterial((ePxMaterial)material);		//material
	physx::PxFilterData* tempPxFilterData = findPxFilterData((ePxLayer)physicsLayer);	//filterdata
	const float tempWeight = weight;
	physx::PxMaterial* wallMaterial = findMaterial(ePxMaterial::NOFRICTION_MATERIAL);		//material


	PxBoxGeometry box = PxBoxGeometry(tempHalfSize);

	if (isCharModel)
	{
		tempPose.p.y -= 0.6f;
	}

	switch (actorType)
	{
		case eActorType::STATIC_ACTORTYPE:
		{
			PxVec3 wholesize = { tempHalfSize.x * 0.99f,tempHalfSize.y,tempHalfSize.z * 0.99f };
			PxVec3 wallsize = { tempHalfSize.x,tempHalfSize.y /** 0.99f*/,tempHalfSize.z };
			PxRigidStatic* tempActor = m_pPhysics->createRigidStatic(tempPose);

			{
				PxBoxGeometry tempbox = PxBoxGeometry(wallsize);
				PxShape* wholeshape = PxRigidActorExt::createExclusiveShape(*tempActor, tempbox, *findMaterial((ePxMaterial)material));
				wholeshape->setSimulationFilterData(*tempPxFilterData);
				wholeshape->setContactOffset(1.0f);
				//wholeshape->setRestOffset(0.5f);
				//tempActor->attachShape(*wholeshape);
				//wholeshape->release();
			}
			//{
			//	PxBoxGeometry tempbox = PxBoxGeometry(wallsize);

			//	PxShape* wallshape = PxRigidActorExt::createExclusiveShape(*tempActor, tempbox, *findMaterial(ePxMaterial::NOFRICTION_MATERIAL));
			//	wallshape->setSimulationFilterData(*tempPxFilterData);
			//	//tempActor->attachShape(*wallshape);
			//	//wallshape->release();

			//}
			ActorState* curState = new ActorState;
			curState->collisionState = eCollisionState::ON_COLLISION_NONE;
			curState->myCompID = ComponentID;

			tempActor->userData = curState;

			int num = tempActor->getNbShapes();
			m_CollisionStayLists.insert({ ComponentID, std::unordered_set<uint32_t>(ADJ_LIST_SIZE) });

			//// 필터링 셋팅!
			//shape->setSimulationFilterData(*tempPxFilterData);

			m_pScene->addActor(*tempActor);
			m_PhysicsActors.insert(std::make_pair(ComponentID, tempActor));
		}
		break;
		case eActorType::DYNAMIC_ACTORTYPE:
		{
			PxRigidDynamic* tempActor = m_pPhysics->createRigidDynamic(tempPose);

			PxShape* shape = PxRigidActorExt::createExclusiveShape(*tempActor, box, *tempMaterial);
			// 필터링 셋팅!
			shape->setSimulationFilterData(*tempPxFilterData);
			//무게 설정
			//tempActor->setMass(500);

			ActorState* curState = new ActorState;
			curState->collisionState = eCollisionState::ON_COLLISION_NONE;
			curState->myCompID = ComponentID;

			tempActor->userData = curState;

			m_CollisionStayLists.insert({ ComponentID, std::unordered_set<uint32_t>(ADJ_LIST_SIZE) });


			float density = tempWeight / (tempHalfSize.x * tempHalfSize.y * tempHalfSize.z * 8);
			PxRigidBodyExt::updateMassAndInertia(*tempActor, density);
			shape->setContactOffset(1.0f);
			//shape->setRestOffset(0.5f);

			int a = PxRigidDynamicLockFlag::Enum::eLOCK_LINEAR_X;
			int b = PxRigidDynamicLockFlag::Enum::eLOCK_LINEAR_Y;
			int c = PxRigidDynamicLockFlag::Enum::eLOCK_LINEAR_Z;
			int d = PxRigidDynamicLockFlag::Enum::eLOCK_ANGULAR_X;
			int e = PxRigidDynamicLockFlag::Enum::eLOCK_ANGULAR_Y;
			int f = PxRigidDynamicLockFlag::Enum::eLOCK_ANGULAR_Z;

			///Gravity 적용 한다 안한다.
			tempActor->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, noGravity);

			// LOCK 설정	
			tempActor->setRigidDynamicLockFlags(
				(PxRigidDynamicLockFlag::Enum)(Lock_lvX << 0) |
				(PxRigidDynamicLockFlag::Enum)(Lock_lvY << 1) |
				(PxRigidDynamicLockFlag::Enum)(Lock_lvZ << 2) |
				(PxRigidDynamicLockFlag::Enum)(Lock_avX << 3) |
				(PxRigidDynamicLockFlag::Enum)(Lock_avY << 4) |
				(PxRigidDynamicLockFlag::Enum)(Lock_avZ << 5));

			m_pScene->addActor(*tempActor);
			m_PhysicsActors.insert(std::make_pair(ComponentID, tempActor));
		}
		break;
		default:
			break;
	}
}
void HysxEngine::makeBoxCollision(uint32_t ComponentID, unsigned int material, Vector3 Size, Vector3 Scale, Vector3 extention)
{
	assert(Size.x * Scale.x / 2 + extention.x > 0.00001);
	assert(Size.y * Scale.y / 2 + extention.y > 0.00001);
	assert(Size.z * Scale.z / 2 + extention.z > 0.00001);
	const PxVec3 tempHalfSize = { Size.x * Scale.x / 2 + extention.x, Size.y * Scale.y / 2 + extention.y, Size.z * Scale.z / 2 + extention.z };
	PxBoxGeometry box = PxBoxGeometry(tempHalfSize);

	auto currentActor = findPxActorbyCompID(ComponentID);
	physx::PxMaterial* tempMaterial = findMaterial((ePxMaterial)material);		//material
	PxShape* triggerShape = createTriggerShape(box, tempMaterial, true, FILTER_SHADER);
	triggerShape->setSimulationFilterData(*m_PxFilterDatas[ePxLayer::COLLISION_DETECTION_LAYER]);
	std::string* name = new std::string;
	*name = "trigger";
	triggerShape->userData = name;
	currentActor->attachShape(*triggerShape);
}

void HysxEngine::makeGimmickBoxCollision(uint32_t ComponentID, unsigned int material, Vector3 Size, Vector3 Scale, Vector3 extention)
{
	assert(Size.x * Scale.x / 2 + extention.x > 0.00001);
	assert(Size.x * Scale.x / 2 - extention.x > 0.00001);
	assert(Size.y * Scale.y / 2 + extention.y > 0.00001);
	assert(Size.z * Scale.z / 2 + extention.z > 0.00001);
	assert(Size.z * Scale.z / 2 - extention.z > 0.00001);
	{
		const PxVec3 tempHalfSize = { Size.x * Scale.x / 2 - extention.x, Size.y * Scale.y / 2 + extention.y, Size.z * Scale.z / 2 + extention.z };
		PxBoxGeometry box = PxBoxGeometry(tempHalfSize);
		auto currentActor = findPxActorbyCompID(ComponentID);
		physx::PxMaterial* tempMaterial = findMaterial((ePxMaterial)material);		//material
		PxShape* triggerShape = createTriggerShape(box, tempMaterial, true, FILTER_SHADER);
		triggerShape->setSimulationFilterData(*m_PxFilterDatas[ePxLayer::COLLISION_DETECTION_LAYER]);
		std::string* name = new std::string;
		*name = "trigger";
		triggerShape->userData = name;
		currentActor->attachShape(*triggerShape);
	}

	{
		const PxVec3 tempHalfSize = { Size.x * Scale.x / 2 + extention.x, Size.y * Scale.y / 2 + extention.y, Size.z * Scale.z / 2 - extention.z };
		PxBoxGeometry box = PxBoxGeometry(tempHalfSize);
		auto currentActor = findPxActorbyCompID(ComponentID);
		physx::PxMaterial* tempMaterial = findMaterial((ePxMaterial)material);		//material
		PxShape* triggerShape = createTriggerShape(box, tempMaterial, true, FILTER_SHADER);
		triggerShape->setSimulationFilterData(*m_PxFilterDatas[ePxLayer::COLLISION_DETECTION_LAYER]);
		std::string* name = new std::string;
		*name = "trigger";
		triggerShape->userData = name;
		currentActor->attachShape(*triggerShape);
	}
}

void HysxEngine::addForce(uint32_t ComponentID, const physx::PxVec3 forcedir, float force, eForceType type)
{
	PxForceMode::Enum mode{};

	switch (type)
	{
		case eForceType::eFORCE:
			mode = PxForceMode::Enum::eFORCE;
			break;
		case eForceType::eIMPULSE:
			mode = PxForceMode::Enum::eIMPULSE;
			break;
		case eForceType::eACCELERATION:
			mode = PxForceMode::Enum::eACCELERATION;
			break;
		case eForceType::eVELOCITY_CHANGE:
			mode = PxForceMode::Enum::eVELOCITY_CHANGE;
			break;
	}

	if (findPxActorbyCompID(ComponentID)->getType() == PxActorType::eRIGID_DYNAMIC)
	{
		physx::PxVec3 direction = { forcedir.x,forcedir.y,-forcedir.z };
		direction = direction.getNormalized();

		auto GameObejectRigidDyamic = static_cast<PxRigidDynamic*>(findPxActorbyCompID(ComponentID));
		if (GameObejectRigidDyamic)
		{
			//크기가 1을 m가 아닌 cm로 취급을 해서 *100을 해줬음.
			//힘의 따라서 값을 새로 해줘야함.
			GameObejectRigidDyamic->addForce(force * direction, mode);
		}
	}
	else
	{
		string temp = std::to_string(ComponentID);
		OutputDebugStringA(temp.c_str());
		OutputDebugStringA(" StaticMesh에는 AddForce를 사용할 수 없습니다.\n");
	}
}

/// <summary>
/// 생성된 collider들을 포함된 actor들을 지울 함수
/// 나중에 추가해야함
/// 
/// 순서 지켜서 release 호출하기
/// 나중에 생긴거 먼저 메모리 해제
/// </summary>
void HysxEngine::finalize()
{
	m_CollisionStayLists.clear();

	while (!m_CollisionEnterDeque.empty())
	{
		m_CollisionEnterDeque.pop_front();
	}

	while (!m_CollisionExitDeque.empty())
	{
		m_CollisionExitDeque.pop_front();
	}

	/*for (auto& cylinder : m_CylinderTriggerGeometrys)
	{
		delete cylinder;
	}*/

	for (auto& actor : m_PhysicsActors)
	{
		unsigned int numShapes = actor.second->getNbShapes();
		std::vector<physx::PxShape*> shapes(numShapes);
		actor.second->getShapes(&shapes[0], numShapes);
		for (auto& shape : shapes)
		{
			delete static_cast<std::string*>(shape->userData);
		}
		delete static_cast<ActorState*>(actor.second->userData);
		PX_RELEASE(actor.second);
	}

	m_cylinderGeometrys.clear();

	for (auto& material : m_PxMaterials)
	{
		PX_RELEASE(material);
	}
	for (auto& filterData : m_PxFilterDatas)
	{
		delete filterData.second;
	}

	//delete m_pCollisionCallback;


	PX_RELEASE(m_pScene);

	PX_RELEASE(m_pDispatcher);

	PX_RELEASE(m_pPhysics);

#ifdef _DEBUG
	m_pPvd->disconnect();
	PX_RELEASE(m_pTransport);
	PX_RELEASE(m_pPvd);
#endif
	PX_RELEASE(m_pFoundation);

}

void HysxEngine::TestCode()
{


}

physx::PxShape* HysxEngine::createTriggerShape(const physx::PxGeometry& geometry, physx::PxMaterial* material, bool isExclusive, eTriggerImpl type)
{
	PxShape* shape = nullptr;

	if (type == REAL_TRIGGERS)
	{
		PxShapeFlags shapeFlags = PxShapeFlag::eVISUALIZATION | PxShapeFlag::eTRIGGER_SHAPE;
		shape = m_pPhysics->createShape(geometry, *material, isExclusive, shapeFlags);
	}

	else if (type == FILTER_SHADER)
	{
		PxShapeFlags shapeFlags = PxShapeFlag::eVISUALIZATION | PxShapeFlag::eSIMULATION_SHAPE;
		shape = m_pPhysics->createShape(geometry, *material, isExclusive, shapeFlags);

		// For this method to work, you need a way to mark shapes as triggers without using PxShapeFlag::eTRIGGER_SHAPE
		// (so that trigger-trigger pairs are reported), and without calling a PxShape function (so that the data is
		// available in a filter shader).
		//
		// One way is to reserve a special PxFilterData value/mask for triggers. It may not always be possible depending
		// on how you otherwise use the filter data).
		PxFilterData triggerFilterData(0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
		shape->setSimulationFilterData(triggerFilterData);
	}

	else/* if (type == FILTER_CALLBACK)*/
	{
		// We will have access to shape pointers in the filter callback so we just mark triggers in an arbitrary way here,
		// for example using the shape's userData.
		shape = m_pPhysics->createShape(geometry, *material, isExclusive);
		shape->userData = shape;	// null이 아니면 trigger
	}

	return shape;
}

void HysxEngine::updatingCollisionState()
{
	while (!m_CollisionEnterDeque.empty())
	{
		auto& actorPair = m_CollisionEnterDeque.front();
		m_CollisionEnterDeque.pop_front();

		uint32_t actorID0 = actorPair.first;
		uint32_t actorID1 = actorPair.second;

		auto iter0 = m_CollisionStayLists.find(actorID0);
		assert(iter0 != m_CollisionStayLists.end());
		iter0->second.insert(actorID1);

		auto iter1 = m_CollisionStayLists.find(actorID1);
		assert(iter1 != m_CollisionStayLists.end());
		iter1->second.insert(actorID0);
	}

	while (!m_CollisionExitDeque.empty())
	{
		auto& actorPair = m_CollisionExitDeque.front();
		m_CollisionExitDeque.pop_front();

		uint32_t actorID0 = actorPair.first;
		uint32_t actorID1 = actorPair.second;

		auto iter0 = m_CollisionStayLists.find(actorID0);
		assert(iter0 != m_CollisionStayLists.end());
		iter0->second.erase(actorID1);

		auto iter1 = m_CollisionStayLists.find(actorID1);
		assert(iter1 != m_CollisionStayLists.end());
		iter1->second.erase(actorID0);
	}
}

physx::PxRigidActor* HysxEngine::findPxActorbyCompID(uint32_t ComponentID)
{

	if (m_PhysicsActors.find(ComponentID) == m_PhysicsActors.end())
	{
		return nullptr;
	}
	return m_PhysicsActors[ComponentID];
}

uint32_t HysxEngine::findCompIDbyPxActor(physx::PxRigidActor* Actor)
{
	uint32_t key = 0;

	for (const auto& pair : m_PhysicsActors)
	{
		if (pair.second == Actor)
		{
			// pair.first is the key associated with temp
			uint32_t key = pair.first;
			// Do something with the key
			return key;
		}
	}
	//assert(key != 0);
	return 0;
}

#pragma endregion



/// Todo: RidgBodyCompID를 MaterialName 찾기.

