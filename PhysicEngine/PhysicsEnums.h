#pragma once

enum ePxLayer
{
	TILE_LAYER = (1 << 0),
	TRIGGER_TILE_LAYER = (1 << 1),
	CHARACTER_LAYER = (1 << 2),
	NO_COLLISION_LAYER = (1 << 3),
	COLLISION_DETECTION_LAYER = (1 << 4),
	LAYER_END = (1 << 5)
};

enum ePxMaterial
{
	FLOOR_MATERIAL = 0,
	PLAYER_MATERIAL = 1,
	BOX_MATERIAL = 2,
	NOFRICTION_MATERIAL = 3,
	MATERIAL_END = 4
};

enum eActorType : unsigned
{
	STATIC_ACTORTYPE = 0,
	DYNAMIC_ACTORTYPE = 1
};

enum class eForceType
{
	eFORCE,
	eIMPULSE,
	eVELOCITY_CHANGE,
	eACCELERATION
};

enum eColliderShape : unsigned
{
	BOX_COLLIDER = 0,
	CAPSULE_COLLIDER = 1,
	CYLINDER_COLLIDER = 2,
};

enum eCollisionShape : unsigned
{
	BOX_COLLISION = 0,
	CAPSULE_COLLISION = 1,
	CYLINDER_COLLISION = 2,
	GIMMICK_BOX_COLLISION = 3
};

enum eCollisionState
{
	ON_COLLISION_NONE = 0,
	ON_COLLISION_ENTER = 1,
	ON_COLLISION_END = 2,
	ON_COLLISION_STAY = 3
};

enum eTriggerImpl
{
	// Uses built-in triggers (PxShapeFlag::eTRIGGER_SHAPE).
	REAL_TRIGGERS,

	// Emulates triggers using a filter shader. Needs one reserved value in PxFilterData.
	FILTER_SHADER,

	// Emulates triggers using a filter callback. Doesn't use PxFilterData but needs user-defined way to mark a shape as a trigger.
	FILTER_CALLBACK,
};

enum eBeltDir
{
	SOUTHEAST,
	SOUTHWEST,
	NORTHEAST,
	NORTHWEST
};