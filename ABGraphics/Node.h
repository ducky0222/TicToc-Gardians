#pragma once

#include "Graphics.h"
#include "IDrawable.h"

class Model;
class Mesh;
class TechniqueProbe;
class ModelProbe;
class AnimClip;

class Node
{
	friend class Model;

public:
	Node(int id, const std::string& name, std::vector<Mesh*> meshRef, const Matrix& transform);
	~Node() = default;

	void Submit(const Matrix& modelTransform, const Matrix& texTransform, const IDrawable::AlphaData& alphaData);

	void ApplyAnimRecursive(const AnimClip& animClip, float timePos);
	void ApplyAnimRecursive(const AnimClip& lhsAnimClip, const AnimClip& rhsAnimClip, float lhsTimePos, float rhsTimePos, float lhsWeight);

	int GetId() const { return mId; }
	const std::string& GetName() const { return mName; }
	const Matrix& GetTransform() const { return mTransform; }
	const Matrix& GetParentTaransform() const { return mParentTransform; }
	Matrix GetFinalTransform() const { return mTransform * mParentTransform; }

private:
	void submitNodeRecusive(const Matrix& parentTransform);
	void submitMeshRecusive(const Matrix& texTransform, const IDrawable::AlphaData& alphaData);
	void addChild(std::unique_ptr<Node> pChild);

private:
	int mId;
	std::string mName;
	std::vector<std::unique_ptr<Node>> mChildren;
	std::vector<Mesh*> mMeshRef;

	const Matrix mBindPoseTransform;
	Matrix mTransform; // 로컬
	Matrix mParentTransform; // 부모
};

class SkinnedMesh;

class SkinnedNode
{
	friend class SkinnedModel;

public:
	SkinnedNode(int id, const std::string& name, std::vector<SkinnedMesh*> skinnedMeshRef, const Matrix& transform);
	~SkinnedNode() = default;

	void Submit(const Matrix& modelTransform, const Matrix& texTransform, const IDrawable::AlphaData& alphaData);

	void ApplyAnimRecursive(const AnimClip& animClip, float timePos);
	void ApplyAnimRecursive(const AnimClip& lhsAnimClip, const AnimClip& rhsAnimClip, float lhsTimePos, float rhsTimePos, float lhsWeight);

	int GetId() const { return mId; }
	const std::string& GetName() const { return mName; }
	const Matrix& GetBindpose() const { return mBindPoseTransform; }
	const Matrix& GetTransform() const { return mTransform; }
	const Matrix& GetParentTaransform() const { return mParentTransform; }
	Matrix GetFinalTransform() const { return mTransform * mParentTransform; }

private:
	void submitNodeRecusive(Matrix parentTransform);
	void submitMeshRecusive(const Matrix& texTransform, const IDrawable::AlphaData& alphaData);
	void addChild(std::unique_ptr<SkinnedNode> pChild);

private:
	int mId;
	std::string mName;
	std::vector<std::unique_ptr<SkinnedNode>> mChildren;
	std::vector<SkinnedMesh*> mSkinnedMeshRef;

	const Matrix mBindPoseTransform;
	Matrix mTransform; // 로컬
	Matrix mParentTransform; // 부모
};