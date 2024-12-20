#pragma once

#include "Graphics.h"
#include "Node.h"
#include "Mesh.h"
#include "GeometryGenerator.h"

namespace Rgph
{
	class RenderGraph;
	class Job;
}

class Model
{
public:
	Model(Graphics& graphics, const std::string& pathString);
	~Model() = default;

	void LinkTechniques(Rgph::RenderGraph& renderGraph);
	void Submit();
	void Reset();

	void ApplyAnimation(const AnimClip& animClip, float timePos, bool bIsRecursive);
	void ApplyAnimation(const AnimClip& lhsAnimClip, const AnimClip& rhsAnimClip, float lhsTimePos, float rhsTimePos, float weight, bool bIsRecursive);

	void SetTransform(const Matrix& transform) { mTransform = transform; }
	void SetTexTransform(Matrix transform) { mTexTransform = transform; }
	void SetAlphaData(IDrawable::AlphaData alphaData) { mAlphaData = alphaData; }

	const Matrix& GetTransform() const { return mTransform; }
	const Matrix& GetTexTransform() const { return mTexTransform; }
	const IDrawable::AlphaData& GetAlphaData() const { return mAlphaData; }
	const std::vector<std::unique_ptr<Mesh>>& GetMeshes() const { return mMeshes; }
	const DirectX::BoundingBox& GetBoundingBox() const { return mBoundingBox; }
	const DirectX::BoundingSphere& GetBoundingSphere() const { return mBoundingSphere; }

private:
	std::unique_ptr<Node> parseNodeRecursive(int& nextId, const aiNode& node);

private:
	mutable Matrix mTransform;
	mutable Matrix mTexTransform;
	mutable IDrawable::AlphaData mAlphaData = { 1.f, false };

	std::unique_ptr<Node> mRoot;
	std::vector<std::unique_ptr<Mesh>> mMeshes;

	DirectX::BoundingBox mBoundingBox;
	DirectX::BoundingSphere mBoundingSphere;
};

class SkinnedModel
{
public:
	SkinnedModel(Graphics& graphics, const std::string& pathString);
	~SkinnedModel() = default;

	void LinkTechniques(Rgph::RenderGraph& renderGraph);
	void Submit();
	void Reset();

	void ApplyAnimation(const AnimClip& animClip, float timePos, bool bIsRecursive);
	void ApplyAnimation(const AnimClip& lhsAnimClip, const AnimClip& rhsAnimClip, float lhsTimePos, float rhsTimePos, float weight, bool bIsRecursive0, bool bIsRecursive1);

	void SetTransform(const Matrix& transform) { mTransform = transform; }
	void SetTexTransform(Matrix transform) { mTexTransform = transform; }
	void SetAlphaData(IDrawable::AlphaData alphaData) { mAlphaData = alphaData; }

	const Matrix& GetTransform() const { return mTransform; }
	const Matrix& GetTexTransform() const { return mTexTransform; }
	const IDrawable::AlphaData& GetAlphaData() const { return mAlphaData; }
	const std::vector<std::unique_ptr<SkinnedMesh>>& GetMeshes() const { return mSkinnedMeshes; }
	const DirectX::BoundingBox& GetBoundingBox() const { return mBoundingBox; }
	const DirectX::BoundingSphere& GetBoundingSphere() const { return mBoundingSphere; }

private:
	std::unique_ptr<SkinnedNode> parseNodeRecursive(int& nextId, const aiNode& node);
	void linkBoneNode();
	void nodeRecursive(SkinnedNode* node, std::map<std::string, const SkinnedNode*>* outNodeMap);

private:
	mutable Matrix mTransform;
	mutable Matrix mTexTransform;
	mutable IDrawable::AlphaData mAlphaData = { 1.f, false };

	std::unique_ptr<SkinnedNode> mRoot;
	std::vector<std::unique_ptr<SkinnedMesh>> mSkinnedMeshes;

	DirectX::BoundingBox mBoundingBox;
	DirectX::BoundingSphere mBoundingSphere;
};

class GeometryModel : public IDrawable
{
	friend class Rgph::Job;

public:
	GeometryModel(Graphics& graphics, const GeometryGenerator::MeshData& meshData);
	~GeometryModel() = default;

	virtual void Submit() override;
	void Reset();

	const DirectX::BoundingBox& GetBoundingBox() const { return mBoundingBox; }
	const DirectX::BoundingSphere& GetBoundingSphere() const { return mBoundingSphere; }

private:
	GeometryGenerator::MeshData mMeshData;

	std::vector<InstanceData> mInstanceDataArr;
	std::vector<InstanceData> mHasAlphaInstanceDataArr;

	DirectX::BoundingBox mBoundingBox;
	DirectX::BoundingSphere mBoundingSphere;
};

// 처리 순서로 인해서 알파를 켜야 레이어가 제대로 적용되네
// 이거 어캐 처리해야하지
class UIModel : public IDrawable
{
	friend class Rgph::Job;

public:
	struct InstanceData
	{
		Matrix Transform;
		Matrix TexTransform;
		AlphaData AlphaData;
		size_t Layer = 0; // 높을수록 위에 그려짐 0 ~ 4까지 지원
	};

public:
	UIModel(Graphics& graphics, const std::string& texturePath);

	virtual void Submit();
	void Reset();

	// 해당 함수로 트랜스폼 적용
	static Matrix MakeTransform(Vector2 topLeft, Vector2 size, float rotation)
	{
		return Matrix::CreateScale(size.x * 2, size.y * 2, 0)
			* Matrix::CreateTranslation(-size.x, size.y, 0)
			* Matrix::CreateRotationZ(rotation)
			* Matrix::CreateTranslation(size.x, -size.y, 0)
			* Matrix::CreateTranslation(topLeft.x * 2, -topLeft.y * 2, 0);
	}

	void SetLayer(size_t layer) { mLayer = min(layer, (size_t)UI_MAX_LAYER - 1); }

	size_t GetLayer() const { return mLayer; }

private:
	size_t mLayer;

	std::array<std::vector<InstanceData>, UI_MAX_LAYER> mInstanceDataArr;
	std::array<std::vector<InstanceData>, UI_MAX_LAYER> mHasAlphaInstanceDataArr;
};

class UI3DModel : public IDrawable
{
	friend class Rgph::Job;

public:
	struct InstanceData
	{
		Matrix TexTransform;
		AlphaData AlphaData;
		Vector3 LookVector;
		Vector3 Position;
		Vector2 Size;
		float Rotation;
		size_t Layer = 0; // 높을수록 위에 그려짐 0 ~ 4까지 지원
	};

public:
	UI3DModel(Graphics& graphics, const std::string& texturePath);
	~UI3DModel() = default;

	virtual void Submit();
	void Reset();

	void SetLookVector(const Vector3& lookVector) { mLookVector = lookVector; }
	void SetPosition(const Vector3& position) { mPosition = position; }
	void SetSize(const Vector2& size) { mSize = size; }
	void SetRotation(float rotation) { mRotation = rotation; }
	void SetLayer(size_t layer) { mLayer = min(layer, (size_t)UI_MAX_LAYER - 1); }

	const Vector3& GetLookVector() const { return mLookVector; }
	const Vector3& GetPosition() const { return mPosition; }
	const Vector2& GetSize() const { return mSize; }
	float GetRotation() const { return mRotation; }
	size_t GetLayer() const { return mLayer; }

private:
	Vector3 mLookVector;
	Vector3 mPosition;
	Vector2 mSize;
	float mRotation;
	size_t mLayer;

	std::array<std::vector<InstanceData>, UI_MAX_LAYER> mInstanceDataArr;
	std::array<std::vector<InstanceData>, UI_MAX_LAYER> mHasAlphaInstanceDataArr;
};


class CutSceneModel : public IDrawable
{
public:
	CutSceneModel(Graphics& graphics, const std::vector<std::string>& texturePaths);
	~CutSceneModel() = default;

	void SetIndex(size_t index) { if (mCapacity > index) { mIndex = index; } }

	size_t GetIndex() const { return mIndex; }
	size_t GetCapacity() const { return mCapacity; }

private:
	size_t mIndex;
	size_t mCapacity;
};

class BackgroundModel : public IDrawable
{
public:
	BackgroundModel(Graphics& graphics, const std::string& texturePath);
	~BackgroundModel() = default;
};

// Particle Model은 하나의 이미터 타입?
class ParticleModel : public IDrawable
{
public:
	struct InstanceData
	{
		Vector3 Position;
	};

public:
	ParticleModel(Graphics& graphics, const std::string& texturePath);
	~ParticleModel() = default;

	virtual void Submit();
	void Reset();

	void SetPosition(const Vector3& position) { mPosition = position; }

	const Vector3& GetPosition() const { return mPosition; }

private:
	Vector3 mPosition;

	std::shared_ptr<Bind::VertexBuffer> mInitVB;
	std::shared_ptr<Bind::VertexBuffer> mSOVB;
	std::shared_ptr<Bind::VertexBuffer> mDrawVB;
};