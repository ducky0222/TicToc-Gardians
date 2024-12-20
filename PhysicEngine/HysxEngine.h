#pragma once
#include <physx/PxPhysicsAPI.h>
#include "PhysicsEnums.h"

class CollisionCallback;

/// <summary>
/// 물리 엔진
/// 좌표계가 y업 오른손 좌표계이므로
/// 렌더러와 데이터를 공유할 때 z값(위치)에 -를 곱해야됨
/// 
/// 회전은 z축 반전 필요
/// </summary>
/// 
/// 
/// 
class HysxEngine
{

#pragma region Final

private:
#pragma region Init_PxMembers
	physx::PxDefaultAllocator		m_Allocator;
	physx::PxDefaultErrorCallback	m_ErrorCallback;
	physx::PxFoundation* m_pFoundation = NULL;
	physx::PxPhysics* m_pPhysics = NULL;
	physx::PxDefaultCpuDispatcher* m_pDispatcher = NULL;
	physx::PxScene* m_pScene = NULL;
	physx::PxPvd* m_pPvd = NULL;
	physx::PxPvdTransport* m_pTransport = NULL;
	std::shared_ptr<CollisionCallback> m_pCollisionCallback = NULL;
#pragma endregion
#pragma region Final_members
	std::unordered_map<uint32_t, physx::PxRigidActor*>		m_PhysicsActors;
	array<physx::PxMaterial*, ePxMaterial::MATERIAL_END>	m_PxMaterials{};
	unordered_map<uint32_t, physx::PxFilterData*>		m_PxFilterDatas{};
	std::unordered_map<uint32_t, std::list<std::shared_ptr<physx::PxCustomGeometryExt::CylinderCallbacks>>> m_cylinderGeometrys;

				// myComp			// collidedComp, collisionState
	unordered_map<uint32_t, unordered_set<uint32_t>> m_CollisionStayLists;
	std::deque<pair<uint32_t ,uint32_t>> m_CollisionExitDeque;
	std::deque<pair<uint32_t, uint32_t>> m_CollisionEnterDeque;

#pragma endregion

public:
	HysxEngine();
	~HysxEngine();

#pragma region Initialize
	void Initialize();

#pragma endregion

	void Update(float deltaTime);

	//void AddPxRigidActor(uint32_t ComponentID);
	void AddPxRigidActor(uint32_t ComponentID, unsigned int material, unsigned int physicsLayer, unsigned int actorType, bool Lock_lvX, bool Lock_lvY, bool Lock_lvZ,  bool Lock_avX, bool Lock_avY, bool Lock_avZ, unsigned int shapeType, DirectX::SimpleMath::Vector3 Size, float weight, DirectX::SimpleMath::Vector3 Scale, DirectX::SimpleMath::Matrix transform, bool isCharModel, bool noGravity);
	//void ChangeStaticToDynamic(uint32_t ComponentID);
	void ChangeStaticToDynamic(uint32_t ComponentID, unsigned int material, unsigned int physicsLayer, unsigned int actorType, bool Lock_lvX, bool Lock_lvY, bool Lock_lvZ, bool Lock_avX, bool Lock_avY, bool Lock_avZ, unsigned int shapeType, DirectX::SimpleMath::Vector3 Size, float weight, DirectX::SimpleMath::Vector3 Scale, DirectX::SimpleMath::Matrix transform, bool isCharModel, bool noGravity);
	//void AttachCollision(uint32_t ComponentID, float extention, eShapeType type);
	void AttachCollision(uint32_t ComponentID, unsigned int type, unsigned int material, DirectX::SimpleMath::Vector3 Size, DirectX::SimpleMath::Vector3 Scale, DirectX::SimpleMath::Vector3 extention);
	void DetachCollision(uint32_t ComponentID);

	//void ChangeScale(uint32_t ComponentID, const DirectX::SimpleMath::Vector3 scale, float extension);
	void ChangeScale(uint32_t ComponentID, float Weight, DirectX::SimpleMath::Vector3 size, DirectX::SimpleMath::Vector3 currentscale, const DirectX::SimpleMath::Vector3 scale, DirectX::SimpleMath::Vector3 extension);

	//해당방향으로 힘을 더한다.
	//(RidgidComponentID | Force_Direction | Force_Power)
	void			AddForce(uint32_t ComponentID, const DirectX::SimpleMath::Vector3 forcedir, float force);
	void			AddForce(uint32_t ComponentID, const DirectX::SimpleMath::Vector3 Force);
	//uint32_t		RaycastFromActorPosition(uint32_t ComponentID, VSM::Vector3 dir, float MaxDistance);
	uint32_t		RaycastFromPosition_Nearest(VSM::Vector3 pos, VSM::Vector3 dir, float MaxDistance);
	std::vector<uint32_t>		RaycastFromPositions_CheckCollision(VSM::Vector3 pos, VSM::Vector3 dir, float MaxDistance);
	std::vector<uint32_t>		RaycastFromPositions(VSM::Vector3 pos, VSM::Vector3 dir, float MaxDistance);
	uint32_t		RaycastFromPosition_IgnoreID_Nearest(uint32_t GameobjectID, VSM::Vector3 pos, VSM::Vector3 dir, float MaxDistance);

	uint32_t		RaycastFromID_Offset_Nearest(uint32_t GameobjectID, VSM::Vector3 pos, VSM::Vector3 unitDir, float maxDistance);
	uint32_t		RaycastFromID_Nearest(uint32_t GameobjectID, VSM::Vector3 unitDir, float maxDistance);
	//std::vector<uint32_t> OverlapObject_FromID_OffSet

	//해당방향으로 충격을 더한다.
	//(RidgidComponentID | Impulse_Direction, Impulse_Power)
	void			AddImpulse(uint32_t ComponentID, const DirectX::SimpleMath::Vector3 forcedir, float force);
	//해당방향으로 속력을 더한다.
	//(RidgidComponentID | Velocity_Direction | Velocity_power)
	void			AddVelocity(uint32_t ComponentID, const DirectX::SimpleMath::Vector3 forcedir, float force);
	//해당방향으로 가속도를 더한다.
	//(RidgidComponentID | Acceleration_Direction | Acceleration_Power)
	void			AddAcceleration(uint32_t ComponentID, const DirectX::SimpleMath::Vector3 forcedir, float force);

	// x 에 적용된 속력을 반환한다.
	const float		GetCurrentSpeed_x(uint32_t ComponentID);
	// y 에 적용된 속력을 반환한다.
	const float		GetCurrentSpeed_y(uint32_t ComponentID);
	// z 에 적용된 속력을 반환한다.
	const float		GetCurrenSpeed_z(uint32_t ComponentID);

	// 현재의 속도를 반환.
	const DirectX::SimpleMath::Vector3	GetCurrentVelocity(uint32_t ComponentID);
	// x, z 의 정규화된 속도의 방향를 반환한다.
	const DirectX::SimpleMath::Vector3	GetCurrentVelocity_dir_xz(uint32_t ComponentID);
	// x, y,z 의 정규화된 속도의 방향를 반환한다.
	const DirectX::SimpleMath::Vector3	GetCurrentVelocity_dir_xyz(uint32_t ComponentID);

	// 위치값을 반환한다. (회전x)
	const DirectX::SimpleMath::Vector3	GetGolbalTranslation(uint32_t ComponentID);
	// 현재 움직이는 방향에 해당 속도를 적용시킨다.
	void			SetSpeedToCurrentVelocity_xz(uint32_t ComponentID, float velocity);
	// 현재 움직이는 방향을 전환시킨다.
	void			SetDirection_xz(uint32_t ComponentID, DirectX::SimpleMath::Vector3 dir);
	// 물체의 속력을 강제로 할당한다.
	void			SetVelocity(uint32_t ComponentID, DirectX::SimpleMath::Vector3 dir,float speed);
	void			SetVelocity(uint32_t ComponentID, DirectX::SimpleMath::Vector3 velocity);
	//위치값을 설정한다.
	void			SetGolbalTranslation(uint32_t ComponentID, DirectX::SimpleMath::Vector3 pose);
	void			SetGolbalTransform (uint32_t ComponentID, const DirectX::SimpleMath::Matrix& simpleMathMatrix);
	void			SetGolbalTransform(uint32_t ComponentID, const DirectX::SimpleMath::Vector3& pos, const DirectX::SimpleMath::Quaternion& quat);

	//최대 속력를 설정한다. (x y z 모두 합한 값이라... 애매함...)
	void			SetMaxSpeed(uint32_t ComponentID, float MaxVelocity);
	//현재 적용된 최대 속력을 가져온다.
	float			GetMaxSpeed(uint32_t ComponentID, float MaxVelocity);

	// 적용되어 있는 힘을 모두 제거한다.
	void			ClearForce(uint32_t ComponentID);

	/// Getter Setter
	std::unordered_map<uint32_t, physx::PxRigidActor*> GetPhysicsActors() const { return m_PhysicsActors; }
	
	bool HaveRigidActor(uint32_t GameobjectID);
	void ReleaseActor(uint32_t GameobjectID);
	void ReleaseAllActor();
	void ReleaseCallback();
	void SetCallback();
	void DeletePxScene();
	void CreatePxScene();
	void SetDamping(uint32_t gameobjectID, float force);
	void SetGravity(VSM::SimpleMath::Vector3 gravity);
	VSM::SimpleMath::Vector3 GetGravity();
	VSM::Quaternion GetQuaternion(uint32_t gameobjectID);
	
private:
	void initialize_PhysxMaterial(); 	//게임내에서 사용할 것들
	void initialize_PxFilterDatas();
	void addMaterial(ePxMaterial ePxMaterialType, float staticFriction, float dynamicFriction, float restitution);

	physx::PxMaterial* findMaterial(ePxMaterial eMaterial);
	physx::PxFilterData* findPxFilterData(ePxLayer Layer);

public:
	physx::PxRigidActor* findPxActorbyCompID(uint32_t ComponentID);
	uint32_t findCompIDbyPxActor(physx::PxRigidActor* Actor);

private:

	void makeCapsuleCollider(uint32_t ComponentID, unsigned int material, unsigned int physicsLayer, unsigned int actorType, bool Lock_lvX, bool Lock_lvY, bool Lock_lvZ, bool Lock_avX, bool Lock_avY, bool Lock_avZ, unsigned int shapeType,  DirectX::SimpleMath::Vector3 Size, float weight, DirectX::SimpleMath::Vector3 Scale, DirectX::SimpleMath::Matrix transform, bool isCharModel, bool noGravity);
	void makeCapsuleCollision(uint32_t ComponentID, unsigned int material, DirectX::SimpleMath::Vector3 Size, DirectX::SimpleMath::Vector3 Scale, DirectX::SimpleMath::Vector3 extention);

	void makeCylinderCollider(uint32_t ComponentID, unsigned int material, unsigned int physicsLayer, unsigned int actorType, bool Lock_lvX, bool Lock_lvY, bool Lock_lvZ, bool Lock_avX, bool Lock_avY, bool Lock_avZ, unsigned int shapeType,  DirectX::SimpleMath::Vector3 Size, float weight, DirectX::SimpleMath::Vector3 Scale, DirectX::SimpleMath::Matrix transform, bool isCharModel, bool noGravity);
	void makeCylinderCollision(uint32_t ComponentID, unsigned int material, DirectX::SimpleMath::Vector3 Size, DirectX::SimpleMath::Vector3 Scale, DirectX::SimpleMath::Vector3 extention);

	void makeBoxCollider(uint32_t ComponentID, unsigned int material, unsigned int physicsLayer, unsigned int actorType, bool Lock_lvX, bool Lock_lvY, bool Lock_lvZ, bool Lock_avX, bool Lock_avY, bool Lock_avZ, unsigned int shapeType, DirectX::SimpleMath::Vector3 Size, float weight, DirectX::SimpleMath::Vector3 Scale, DirectX::SimpleMath::Matrix transform, bool isCharModel, bool noGravity);
	void makeBoxCollision(uint32_t ComponentID, unsigned int material, DirectX::SimpleMath::Vector3 Size, DirectX::SimpleMath::Vector3 Scale, DirectX::SimpleMath::Vector3 extention);
	void makeGimmickBoxCollision(uint32_t ComponentID, unsigned int material, DirectX::SimpleMath::Vector3 Size, DirectX::SimpleMath::Vector3 Scale, DirectX::SimpleMath::Vector3 extention);

	void addForce(uint32_t ComponentID, const physx::PxVec3 forcedir, float force, eForceType type);
	void finalize();
	void TestCode();
	physx::PxShape* createTriggerShape(const physx::PxGeometry& geometry, physx::PxMaterial* material, bool isExclusive, eTriggerImpl type);

	void updatingCollisionState();


#pragma endregion
public:

	std::unordered_map<uint32_t, std::unordered_set<uint32_t>>& GetCollisionStayLists() { return m_CollisionStayLists; }
	std::deque<std::pair<uint32_t, uint32_t>>& GetCollisionExitDeque() { return m_CollisionExitDeque; }
	std::deque<std::pair<uint32_t, uint32_t>>& GetCollisionEnterDeque() { return m_CollisionEnterDeque; }
	physx::PxPhysics* GetPhysics() const { return m_pPhysics; }
};

namespace Physics
{
	HysxEngine* GetInstance();
	physx::PxFilterFlags CustomFilterShader(physx::PxFilterObjectAttributes attributes0, physx::PxFilterData filterData0,
		physx::PxFilterObjectAttributes attributes1, physx::PxFilterData filterData1,
		physx::PxPairFlags& pairFlags, const void* constantBlock, physx::PxU32 constantBlockSize);
	void Destroy();
	DirectX::SimpleMath::Matrix PxTransformToMatrix(const physx::PxTransform& pxTrans);
	DirectX::SimpleMath::Matrix PxTranstoTransform(const physx::PxTransform& pxTrans);
	physx::PxTransform MatrixToPxTransform(const DirectX::SimpleMath::Matrix& simpleMathMatrix);
}
///TODO : Transform 회전은 제외 시키고 


/// <summary>
/// Component 예시 - gameobjectID x CompononentID ,  -> GetOwner() -> GetComponent< Transform >() ; 
/// </summary>

struct ActorState
{
	uint32_t collisionState = 0;
	uint32_t myCompID = 0;
	uint32_t otherCompID = 0;
				//mycomp			상대			//상태 
	//unordered_map<uint32_t , std::pair<uint32_t, uint32_t>>
};