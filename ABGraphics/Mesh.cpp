#include "pch.h"

#include "Node.h"
#include "Mesh.h"
#include "Surface.h"
#include "DynamicConstant.h"
#include "LayoutCodex.h"
#include "Stencil.h"
#include "Material.h"
#include "BindableCommon.h"
#include "Technique.h"
#include "Step.h"
#include "TransformCBufBone.h"
#include "TransformCbuf.h"
#include "TransformCbufShadow.h"

static DirectX::SimpleMath::Matrix convertMatrix(const aiMatrix4x4& aiMatrix)
{
	DirectX::SimpleMath::Matrix result;

	for (int i = 0; i < 4; ++i)
	{
		result.m[i][0] = aiMatrix[i][0];
		result.m[i][1] = aiMatrix[i][1];
		result.m[i][2] = aiMatrix[i][2];
		result.m[i][3] = aiMatrix[i][3];
	}

	return result;
}

// Mesh는 생성을 IDrawable에게 맡긴다.
Mesh::Mesh(Graphics& graphics, const aiMesh& mesh, Material material)
	: IDrawable(eDrawableType::Mesh)
	, mName(mesh.mName.C_Str())
{
	assert(!mesh.HasBones());

	parseVertices(mesh);
	parseIndices(mesh);

	Dvtx::VertexLayout vertexLayout;
	vertexLayout.Append(Dvtx::VertexLayout::Position3D)
		.Append(Dvtx::VertexLayout::Texture2D)
		.Append(Dvtx::VertexLayout::Normal)
		.Append(Dvtx::VertexLayout::Tangent);

	Dvtx::VertexBuffer vertices{ vertexLayout };

	for (auto& vertex : mVertices)
	{
		vertices.EmplaceBack(vertex.Position, vertex.UV, vertex.Normal, vertex.Tangent);
	}

	// 바인더블 생성
	using namespace Bind;
	pVertices = VertexBuffer::Create(graphics, mName + material.GetName(), vertices);
	pIndices = IndexBuffer::Create(graphics, mName + material.GetName(), mIndices);
	pTopology = Topology::Create(graphics);

	auto vertexShader = VertexShader::Create(graphics, "../Resources/Shader/Model_VS.hlsl");
	auto vertexShadowShader = VertexShader::Create(graphics, "../Resources/Shader/ModelShadow_VS.hlsl");
	auto vertexSsaoShader = VertexShader::Create(graphics, "../Resources/Shader/SsaoDepthNormalBasic_VS.hlsl");
	auto inputLayout = InputLayout::Create(graphics, vertexLayout, vertexShader->GetBytecode());

	{
		Technique tech("shadow");

		{
			Step step("shadowPass");

			step.AddBindable(vertexShadowShader);
			step.AddBindable(inputLayout);
			step.AddBindable(std::make_shared<TransformCbufShadow>(graphics, 0u));

			tech.AddStep(step);
		}

		AddTechnique(tech);
	}
	{
		Technique tech("ssao");

		{
			Step step("ssaoDepthNormal");

			step.AddBindable(vertexSsaoShader);
			step.AddBindable(inputLayout);
			step.AddBindable(std::make_shared<TransformCbuf>(graphics, 0u));

			tech.AddStep(step);
		}

		AddTechnique(tech);
	}

	// 메테리얼의 스탭에 맞춰서 VS 바인더블 정보 추가
	for (Technique& tech : material.GetTechniques())
	{
		if (tech.GetName() == "RenderTechnique")
		{
			for (Step& step : tech.GetSteps())
			{
				step.AddBindable(vertexShader);
				step.AddBindable(inputLayout);
				step.AddBindable(std::make_shared<TransformCbuf>(graphics, 0u));
			}
		}

		AddTechnique(tech);
	}
}

void Mesh::Submit(const Matrix& modelTransform, const Matrix& texTransform, const IDrawable::AlphaData& alphaData)
{
	InstanceData instanceData;
	instanceData.Transform = modelTransform;
	instanceData.TexTransform = texTransform;
	instanceData.AlphaData = alphaData;

	if (alphaData.bUsed)
	{
		if (mHasAlphaInstanceDataArr.size() == 0)
		{
			IDrawable::Submit();
		}

		mHasAlphaInstanceDataArr.push_back(instanceData);
	}
	else
	{
		if (mInstanceDataArr.size() == 0)
		{
			IDrawable::Submit();
		}

		mInstanceDataArr.push_back(instanceData);
	}
}

void Mesh::Reset()
{
	mInstanceDataArr.clear();
	mHasAlphaInstanceDataArr.clear();
}

void Mesh::parseVertices(const aiMesh& mesh)
{
	for (UINT i = 0; i < mesh.mNumVertices; ++i)
	{
		VertexType vertex;

		if (mesh.HasPositions())
		{
			vertex.Position.x = mesh.mVertices[i].x;
			vertex.Position.y = mesh.mVertices[i].y;
			vertex.Position.z = mesh.mVertices[i].z;
		}

		if (mesh.HasNormals())
		{
			vertex.Normal.x = mesh.mNormals[i].x;
			vertex.Normal.y = mesh.mNormals[i].y;
			vertex.Normal.z = mesh.mNormals[i].z;
		}

		if (mesh.HasTangentsAndBitangents())
		{
			vertex.Tangent.x = mesh.mTangents[i].x;
			vertex.Tangent.y = mesh.mTangents[i].y;
			vertex.Tangent.z = mesh.mTangents[i].z;
		}

		if (mesh.HasTextureCoords(0))
		{
			vertex.UV.x = (float)mesh.mTextureCoords[0][i].x;
			vertex.UV.y = (float)mesh.mTextureCoords[0][i].y;
		}

		mVertices.push_back(vertex);
	}
}

void Mesh::parseIndices(const aiMesh& mesh)
{
	mIndices.reserve(mesh.mNumFaces * 3);

	for (unsigned int i = 0; i < mesh.mNumFaces; i++)
	{
		const auto& face = mesh.mFaces[i];
		assert(face.mNumIndices == 3);

		mIndices.push_back(face.mIndices[0]);
		mIndices.push_back(face.mIndices[1]);
		mIndices.push_back(face.mIndices[2]);
	}
}

SkinnedMesh::SkinnedMesh(Graphics& graphics, const aiMesh& mesh, Material material)
	: IDrawable(eDrawableType::SkinnedMesh)
	, mName(mesh.mName.C_Str())
{
	// assert(mesh.HasBones());

	parseVertices(mesh);
	parseIndices(mesh);
	parseBones(mesh);

	Dvtx::VertexLayout vertexLayout;
	vertexLayout.Append(Dvtx::VertexLayout::Position3D)
		.Append(Dvtx::VertexLayout::Texture2D)
		.Append(Dvtx::VertexLayout::Normal)
		.Append(Dvtx::VertexLayout::Tangent)
		.Append(Dvtx::VertexLayout::BoneIndices)
		.Append(Dvtx::VertexLayout::BoneWeights);

	Dvtx::VertexBuffer vertices{ vertexLayout };

	for (auto& vertex : mVertices)
	{
		vertices.EmplaceBack(vertex.Position, vertex.UV, vertex.Normal, vertex.Tangent, vertex.Indices, vertex.Weights);
	}

	using namespace Bind;
	pVertices = VertexBuffer::Create(graphics, mName + material.GetName() + "Skinned", vertices);
	pIndices = IndexBuffer::Create(graphics, mName + material.GetName() + "Skinned", mIndices);
	pTopology = Topology::Create(graphics);

	auto vertexShader = VertexShader::Create(graphics, "../Resources/Shader/ModelSkinned_VS.hlsl");
	auto vertexShadowShader = VertexShader::Create(graphics, "../Resources/Shader/ModelSkinnedShadow_VS.hlsl");
	auto vertexSsaoShader = VertexShader::Create(graphics, "../Resources/Shader/SsaoDepthNormalSkinned_VS.hlsl");
	auto inputLayout = InputLayout::Create(graphics, vertexLayout, vertexShader->GetBytecode());

	{
		Technique tech("ShadowMap");

		{
			Step step("shadowPass");

			step.AddBindable(vertexShadowShader);
			step.AddBindable(inputLayout);
			step.AddBindable(std::make_shared<TransformCbufShadow>(graphics, 0u));
			step.AddBindable(std::make_shared<TransformCBufBone>(graphics, 1u));

			tech.AddStep(step);
		}

		AddTechnique(tech);
	}
	{
		Technique tech("ssao");

		{
			Step step("ssaoDepthNormal");

			step.AddBindable(vertexSsaoShader);
			step.AddBindable(inputLayout);
			step.AddBindable(std::make_shared<TransformCbuf>(graphics, 0u));
			step.AddBindable(std::make_shared<TransformCBufBone>(graphics, 1u));

			tech.AddStep(step);
		}

		AddTechnique(tech);
	}

	// 메테리얼의 스탭에 맞춰서 VS 바인더블 정보 추가
	for (Technique& tech : material.GetTechniques())
	{
		if (tech.GetName() == "RenderTechnique")
		{
			for (Step& step : tech.GetSteps())
			{
				step.AddBindable(vertexShader);
				step.AddBindable(inputLayout);
				step.AddBindable(std::make_shared<TransformCbuf>(graphics, 0u));
				step.AddBindable(std::make_shared<TransformCBufBone>(graphics, 1u));
			}
		}

		AddTechnique(tech);
	}
}

void SkinnedMesh::Submit(const Matrix& modelTransform, const Matrix& texTransform, const IDrawable::AlphaData& alphaData)
{
	InstanceData instanceData;
	instanceData.DrawableInstanceData.Transform = modelTransform;
	instanceData.DrawableInstanceData.AlphaData = alphaData;
	instanceData.DrawableInstanceData.TexTransform = texTransform;

	for (size_t i = 0; i < mBones.size(); ++i)
	{
		// 본 공간으로의 행렬 * 노드의 현재 애니메이션으로 변환된 행렬
		instanceData.BonePallet[i] = (mBones[i].OffsetMatrix * mBones[i].NodePtr->GetFinalTransform()).Transpose();
	}

	if (alphaData.bUsed)
	{
		if (mHasAlphaInstanceDataArr.size() == 0)
		{
			IDrawable::Submit();
		}

		mHasAlphaInstanceDataArr.push_back(instanceData);
	}
	else
	{
		if (mInstanceDataArr.size() == 0)
		{
			IDrawable::Submit();
		}

		mInstanceDataArr.push_back(instanceData);
	}
}

void SkinnedMesh::Reset()
{
	mInstanceDataArr.clear();
	mHasAlphaInstanceDataArr.clear();
}

void SkinnedMesh::parseVertices(const aiMesh& mesh)
{
	for (UINT i = 0; i < mesh.mNumVertices; ++i)
	{
		VertexType vertex;

		if (mesh.HasPositions())
		{
			vertex.Position.x = mesh.mVertices[i].x;
			vertex.Position.y = mesh.mVertices[i].y;
			vertex.Position.z = mesh.mVertices[i].z;
		}

		if (mesh.HasNormals())
		{
			vertex.Normal.x = mesh.mNormals[i].x;
			vertex.Normal.y = mesh.mNormals[i].y;
			vertex.Normal.z = mesh.mNormals[i].z;
		}

		if (mesh.HasTangentsAndBitangents())
		{
			vertex.Tangent.x = mesh.mTangents[i].x;
			vertex.Tangent.y = mesh.mTangents[i].y;
			vertex.Tangent.z = mesh.mTangents[i].z;
		}

		if (mesh.HasTextureCoords(0))
		{
			vertex.UV.x = (float)mesh.mTextureCoords[0][i].x;
			vertex.UV.y = (float)mesh.mTextureCoords[0][i].y;
		}

		mVertices.push_back(vertex);
	}
}

void SkinnedMesh::parseIndices(const aiMesh& mesh)
{
	mIndices.reserve(mesh.mNumFaces * 3);

	for (unsigned int i = 0; i < mesh.mNumFaces; i++)
	{
		const auto& face = mesh.mFaces[i];
		assert(face.mNumIndices == 3);

		mIndices.push_back(face.mIndices[0]);
		mIndices.push_back(face.mIndices[1]);
		mIndices.push_back(face.mIndices[2]);
	}
}

void SkinnedMesh::parseBones(const aiMesh& mesh)
{
	mBones.reserve(mesh.mNumBones);

	for (UINT i = 0; i < mesh.mNumBones; ++i)
	{
		aiBone* curBone = mesh.mBones[i];

		Bone bone;
		bone.Name = curBone->mName.C_Str();
		bone.OffsetMatrix = convertMatrix(curBone->mOffsetMatrix).Transpose();

		mBones.push_back(bone);

		for (UINT j = 0; j < curBone->mNumWeights; ++j)
		{
			aiVertexWeight curVertexWeight = curBone->mWeights[j];

			unsigned int vertexID = curVertexWeight.mVertexId;
			float vertexWeight = curVertexWeight.mWeight;

			int* indexPtr = &mVertices[vertexID].Indices.x;
			float* weightPtr = &mVertices[vertexID].Weights.x;

			for (UINT k = 0; k < 4; ++k)
			{
				if (*indexPtr == VertexType::INVALID_INDEX)
				{
					*indexPtr = i;
					*weightPtr = vertexWeight;

					break;
				}

				++indexPtr;
				++weightPtr;
			}
		}
	}
}
