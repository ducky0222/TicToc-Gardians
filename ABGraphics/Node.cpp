#include "pch.h"

#include "Node.h"
#include "Mesh.h"
#include "Animation.h"

Node::Node(int id, const std::string& name, std::vector<Mesh*> meshRef, const Matrix& transform)
	: mId(id)
	, mBindPoseTransform(transform)
	, mTransform(transform)
	, mParentTransform(Matrix::Identity)
	, mMeshRef(std::move(meshRef))
	, mName(name)
{
}

void Node::Submit(const Matrix& modelTransform, const Matrix& texTransform, const IDrawable::AlphaData& alphaData)
{
	submitNodeRecusive(modelTransform);
	submitMeshRecusive(texTransform, alphaData);
}

void Node::ApplyAnimRecursive(const AnimClip& animClip, float timePos)
{
	const Keyframes* keyframes = nullptr;

	if (animClip.TryGetkeyframes(mName, &keyframes))
	{
		assert(keyframes != nullptr);
		mTransform = Keyframes::CreateMatrix(keyframes->Interpolate(timePos));
	}
	else
	{
		mTransform = mBindPoseTransform;
	}

	for (auto& child : mChildren)
	{
		child->ApplyAnimRecursive(animClip, timePos);
	}
}

void Node::ApplyAnimRecursive(const AnimClip& lhsAnimClip, const AnimClip& rhsAnimClip, float lhsTimePos, float rhsTimePos, float lhsWeight)
{
	const Keyframes* keyframes = nullptr;

	if (lhsAnimClip.TryGetkeyframes(mName, &keyframes))
	{
		assert(keyframes != nullptr);
		Keyframe lhsKeyframe = keyframes->Interpolate(lhsTimePos);

		if (rhsAnimClip.TryGetkeyframes(mName, &keyframes))
		{
			assert(keyframes != nullptr);
			Keyframe rhsKeyframe = keyframes->Interpolate(rhsTimePos);
			mTransform = Keyframes::CreateMatrix(Keyframes::Interpolate(lhsKeyframe, rhsKeyframe, lhsWeight));
		}
		else
		{
			mTransform = Keyframes::CreateMatrix(lhsKeyframe);
		}
	}

	for (auto& child : mChildren)
	{
		child->ApplyAnimRecursive(lhsAnimClip, rhsAnimClip, lhsTimePos, rhsTimePos, lhsWeight);
	}
}

void Node::addChild(std::unique_ptr<Node> pChild)
{
	assert(pChild != nullptr);

	mChildren.push_back(std::move(pChild));
}

void Node::submitNodeRecusive(const Matrix& parentTransform)
{
	mParentTransform = parentTransform;

	for (auto& child : mChildren)
	{
		child->submitNodeRecusive(GetFinalTransform());
	}
}

void Node::submitMeshRecusive(const Matrix& texTransform, const IDrawable::AlphaData& alphaData)
{
	for (auto& mesh : mMeshRef)
	{
		mesh->Submit(GetFinalTransform(), texTransform, alphaData);
	}

	for (auto& child : mChildren)
	{
		child->submitMeshRecusive(texTransform, alphaData);
	}
}


SkinnedNode::SkinnedNode(int id, const std::string& name, std::vector<SkinnedMesh*> skinnedMeshRef, const Matrix& transform)
	: mId(id)
	, mBindPoseTransform(transform)
	, mTransform(transform)
	, mParentTransform(Matrix::Identity)
	, mSkinnedMeshRef(std::move(skinnedMeshRef))
	, mName(name)
{

}

void SkinnedNode::Submit(const Matrix& modelTransform, const Matrix& texTransform, const IDrawable::AlphaData& alphaData)
{
	submitNodeRecusive(modelTransform);
	submitMeshRecusive(texTransform, alphaData);
}

void SkinnedNode::ApplyAnimRecursive(const AnimClip& animClip, float timePos)
{
	const Keyframes* keyframes = nullptr;

	if (animClip.TryGetkeyframes(mName, &keyframes))
	{
		assert(keyframes != nullptr);
		mTransform = Keyframes::CreateMatrix(keyframes->Interpolate(timePos));
	}
	else
	{
		mTransform = mBindPoseTransform;
	}

	for (auto& child : mChildren)
	{
		child->ApplyAnimRecursive(animClip, timePos);
	}
}

void SkinnedNode::ApplyAnimRecursive(const AnimClip& lhsAnimClip, const AnimClip& rhsAnimClip, float lhsTimePos, float rhsTimePos, float lhsWeight)
{
	const Keyframes* keyframes = nullptr;

	if (lhsAnimClip.TryGetkeyframes(mName, &keyframes))
	{
		assert(keyframes != nullptr);
		Keyframe lhsKeyframe = keyframes->Interpolate(lhsTimePos);

		if (rhsAnimClip.TryGetkeyframes(mName, &keyframes))
		{
			assert(keyframes != nullptr);
			Keyframe rhsKeyframe = keyframes->Interpolate(rhsTimePos);
			mTransform = Keyframes::CreateMatrix(Keyframes::Interpolate(lhsKeyframe, rhsKeyframe, lhsWeight));
		}
		else
		{
			mTransform = Keyframes::CreateMatrix(lhsKeyframe);
		}
	}

	for (auto& child : mChildren)
	{
		child->ApplyAnimRecursive(lhsAnimClip, rhsAnimClip, lhsTimePos, rhsTimePos, lhsWeight);
	}
}

void SkinnedNode::submitNodeRecusive(Matrix parentTransform)
{
	mParentTransform = parentTransform;

	for (auto& child : mChildren)
	{
		child->submitNodeRecusive(GetFinalTransform());
	}
}

void SkinnedNode::submitMeshRecusive(const Matrix& texTransform, const IDrawable::AlphaData& alphaData)
{
	for (auto& skinnedMesh : mSkinnedMeshRef)
	{
		skinnedMesh->Submit(GetFinalTransform(), texTransform, alphaData);
	}

	for (auto& child : mChildren)
	{
		child->submitMeshRecusive(texTransform, alphaData);
	}
}

void SkinnedNode::addChild(std::unique_ptr<SkinnedNode> pChild)
{
	assert(pChild != nullptr);

	mChildren.push_back(std::move(pChild));
}
