#pragma once

#include "Graphics.h"
#include "IDrawable.h"
#include "Vertex.h"
#include "LightHelper.h"

namespace Rgph
{
	class Job;
}

class Mesh : public IDrawable
{
	friend class Rgph::Job;

#pragma region struct
public:
	struct VertexType
	{
		Vector3 Position = { 0, 0, 0 };
		Vector2 UV = { 0, 0 };
		Vector3 Normal = { 0, 0, 0 };
		Vector3 Tangent = { 0, 0, 0 };
	};

#pragma endregion

public:
	Mesh(Graphics& graphics, const aiMesh& mesh, Material material);
	virtual ~Mesh() = default;

	void Submit(const Matrix& modelTransform, const Matrix& texTransform, const IDrawable::AlphaData& alphaData);
	void Reset();

	const std::vector<VertexType>& GetVertexType() const { return mVertices; }
	const std::vector<UINT> GetIndices() const { return mIndices; }

private:
	void parseVertices(const aiMesh& mesh);
	void parseIndices(const aiMesh& mesh);

private:
	std::string mName;
	std::vector<VertexType> mVertices;
	std::vector<UINT> mIndices;

	std::vector<InstanceData> mInstanceDataArr;
	std::vector<InstanceData> mHasAlphaInstanceDataArr;
};

class SkinnedNode;

class SkinnedMesh : public IDrawable
{
	friend class SkinnedModel;
	friend class Rgph::Job;

#pragma region struct
public:
	struct Bone
	{
		Matrix OffsetMatrix;
		std::string Name;
		const SkinnedNode* NodePtr;
	};

	struct VertexType
	{
		enum { INVALID_INDEX = -1 };

		Vector3 Position = { 0, 0, 0 };
		Vector2 UV = { 0, 0 };
		Vector3 Normal = { 0, 0, 0 };
		Vector3 Tangent = { 0, 0, 0 };
		BoneIndex Indices = { INVALID_INDEX, INVALID_INDEX, INVALID_INDEX, INVALID_INDEX };
		Vector4 Weights = { 0.f, 0.f, 0.f, 0.f };
	};

	struct InstanceData
	{
		IDrawable::InstanceData DrawableInstanceData;
		Matrix BonePallet[300];
	};
#pragma endregion

public:
	SkinnedMesh(Graphics& graphics, const aiMesh& mesh, Material material);
	virtual ~SkinnedMesh() = default;

	void Submit(const Matrix& modelTransform, const Matrix& texTransform, const IDrawable::AlphaData& alphaData);
	void Reset();

	const std::vector<VertexType>& GetVertexType() const { return mVertices; }
	const std::vector<UINT> GetIndices() const { return mIndices; }
	const std::vector<Bone>& GetBones() const { return mBones; }
	const Matrix* GetMatrix() const { return mBonePallet; }

private:
	void parseVertices(const aiMesh& mesh);
	void parseIndices(const aiMesh& mesh);
	void parseBones(const aiMesh& mesh);

private:
	Matrix mBonePallet[300];
	std::string mName;
	std::vector<VertexType> mVertices;
	std::vector<UINT> mIndices;
	std::vector<Bone> mBones;

	std::vector<InstanceData> mInstanceDataArr;
	std::vector<InstanceData> mHasAlphaInstanceDataArr;
};