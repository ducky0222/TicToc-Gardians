#include "pch.h"

#include "Model.h"
#include "Node.h"
#include "Mesh.h"
#include "Material.h"
#include "Animation.h"
#include "BindableCommon.h"
#include "DynamicConstant.h"

Model::Model(Graphics& graphics, const std::string& pathString)
{
	Assimp::Importer importer;
	importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, 0);
	unsigned int importFlags = aiProcess_Triangulate |
		aiProcess_GenNormals |
		aiProcess_GenUVCoords |
		aiProcess_CalcTangentSpace |
		aiProcess_ConvertToLeftHanded |
		aiProcess_GenBoundingBoxes;

	const aiScene* pScene = importer.ReadFile(pathString, importFlags);

	if (pScene == nullptr)
	{
		assert(false);
	}

	// 재질 생성
	std::vector<Material> materials;
	materials.reserve(pScene->mNumMaterials);

	for (size_t i = 0; i < pScene->mNumMaterials; ++i)
	{
		materials.push_back(Material(graphics, *pScene->mMaterials[i], pathString));
	}

	Vector3 min;
	Vector3 max;

	// 매쉬 생성
	for (size_t i = 0; i < pScene->mNumMeshes; ++i)
	{
		aiMesh& mesh = *pScene->mMeshes[i];
		unsigned int materialIndex = mesh.mMaterialIndex;
		Material& material = materials[mesh.mMaterialIndex];

		Vector3 meshMin = { mesh.mAABB.mMin.x, mesh.mAABB.mMin.y, mesh.mAABB.mMin.z };
		Vector3 meshMax = { mesh.mAABB.mMax.x,  mesh.mAABB.mMax.y,  mesh.mAABB.mMax.z };

		min = Vector3::Min(min, meshMin);
		max = Vector3::Max(max, meshMax);

		mMeshes.push_back(std::make_unique<Mesh>(graphics, mesh, material));
	}

	DirectX::BoundingBox::CreateFromPoints(mBoundingBox, min, max);
	DirectX::BoundingSphere::CreateFromBoundingBox(mBoundingSphere, mBoundingBox);

	// 노드 생성
	int nextId = 0;
	mRoot = parseNodeRecursive(nextId, *pScene->mRootNode);
}

void Model::Submit()
{
	mRoot->Submit(mTransform, mTexTransform, mAlphaData);
}

void Model::Reset()
{
	for (auto& mesh : mMeshes)
	{
		mesh->Reset();
	}
}

void Model::ApplyAnimation(const AnimClip& animClip, float timePos, bool bIsRecursive)
{
	if (bIsRecursive)
	{
		timePos = fmod(timePos, animClip.GetDuration());
	}

	mRoot->ApplyAnimRecursive(animClip, timePos);
}

void Model::ApplyAnimation(const AnimClip& lhsAnimClip, const AnimClip& rhsAnimClip, float lhsTimePos, float rhsTimePos, float weight, bool bIsRecursive)
{
	if (bIsRecursive)
	{
		lhsTimePos = fmod(lhsTimePos, lhsAnimClip.GetDuration());
		rhsTimePos = fmod(rhsTimePos, rhsAnimClip.GetDuration());
	}

	mRoot->ApplyAnimRecursive(lhsAnimClip, rhsAnimClip, lhsTimePos, rhsTimePos, weight);
}


void Model::LinkTechniques(Rgph::RenderGraph& renderGraph)
{
	for (auto& mesh : mMeshes)
	{
		mesh->LinkTechniques(renderGraph);
	}
}

std::unique_ptr<Node> Model::parseNodeRecursive(int& nextId, const aiNode& node)
{
	Matrix transform = (reinterpret_cast<const Matrix*>(&node.mTransformation)->Transpose());

	std::vector<Mesh*> meshes;
	meshes.reserve(node.mNumMeshes);

	for (size_t i = 0; i < node.mNumMeshes; ++i)
	{
		const unsigned int MESH_INDEX = node.mMeshes[i];
		Mesh* mesh = mMeshes[MESH_INDEX].get();

		meshes.push_back(mesh);
	}

	auto pNode = std::make_unique<Node>(nextId++, node.mName.C_Str(), std::move(meshes), transform);

	for (size_t i = 0; i < node.mNumChildren; i++)
	{
		pNode->addChild(parseNodeRecursive(nextId, *node.mChildren[i]));
	}

	return pNode;
}

SkinnedModel::SkinnedModel(Graphics& graphics, const std::string& pathString)
{
	Assimp::Importer importer;
	importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, 0);
	unsigned int importFlags = aiProcess_Triangulate |
		aiProcess_GenNormals |
		aiProcess_GenUVCoords |
		aiProcess_CalcTangentSpace |
		aiProcess_LimitBoneWeights |
		aiProcess_ConvertToLeftHanded |
		aiProcess_GenBoundingBoxes;

	const aiScene* pScene = importer.ReadFile(pathString, importFlags);

	if (pScene == nullptr)
	{
		assert(false);
	}

	// 재질 생성
	std::vector<Material> materials;
	materials.reserve(pScene->mNumMaterials);

	for (size_t i = 0; i < pScene->mNumMaterials; ++i)
	{
		materials.push_back(Material(graphics, *pScene->mMaterials[i], pathString));
	}

	Vector3 min;
	Vector3 max;

	// 매쉬 생성
	for (size_t i = 0; i < pScene->mNumMeshes; ++i)
	{
		aiMesh& mesh = *pScene->mMeshes[i];
		unsigned int materialIndex = mesh.mMaterialIndex;
		Material& material = materials[mesh.mMaterialIndex];

		Vector3 meshMin = { mesh.mAABB.mMin.x, mesh.mAABB.mMin.y, mesh.mAABB.mMin.z };
		Vector3 meshMax = { mesh.mAABB.mMax.x,  mesh.mAABB.mMax.y,  mesh.mAABB.mMax.z };

		min = Vector3::Min(min, meshMin);
		max = Vector3::Max(max, meshMax);

		mSkinnedMeshes.push_back(std::make_unique<SkinnedMesh>(graphics, mesh, material));
	}

	DirectX::BoundingBox::CreateFromPoints(mBoundingBox, min, max);
	DirectX::BoundingSphere::CreateFromBoundingBox(mBoundingSphere, mBoundingBox);

	// 노드 생성
	int nextId = 0;
	mRoot = parseNodeRecursive(nextId, *pScene->mRootNode);
	linkBoneNode();
}

void SkinnedModel::LinkTechniques(Rgph::RenderGraph& renderGraph)
{
	for (auto& skinnedMesh : mSkinnedMeshes)
	{
		skinnedMesh->LinkTechniques(renderGraph);
	}
}

void SkinnedModel::Submit()
{
	mRoot->Submit(mTransform, mTexTransform, mAlphaData);
}

void SkinnedModel::Reset()
{
	for (auto& skinnedMesh : mSkinnedMeshes)
	{
		skinnedMesh->Reset();
	}
}

void SkinnedModel::ApplyAnimation(const AnimClip& animClip, float timePos, bool bIsRecursive)
{
	if (bIsRecursive)
	{
		timePos = fmod(timePos, animClip.GetDuration());
	}

	mRoot->ApplyAnimRecursive(animClip, timePos);
}

void SkinnedModel::ApplyAnimation(const AnimClip& lhsAnimClip, const AnimClip& rhsAnimClip, float lhsTimePos, float rhsTimePos, float weight, bool bIsRecursive0, bool bIsRecursive1)
{
	if (bIsRecursive0)
	{
		lhsTimePos = fmod(lhsTimePos, lhsAnimClip.GetDuration());
	}
	if (bIsRecursive1)
	{
		rhsTimePos = fmod(rhsTimePos, rhsAnimClip.GetDuration());
	}
	weight = min(weight, 1.f);

	mRoot->ApplyAnimRecursive(lhsAnimClip, rhsAnimClip, lhsTimePos, rhsTimePos, weight);
}

std::unique_ptr<SkinnedNode> SkinnedModel::parseNodeRecursive(int& nextId, const aiNode& node)
{
	Matrix transform = (reinterpret_cast<const Matrix*>(&node.mTransformation)->Transpose());

	std::vector<SkinnedMesh*> meshes;
	meshes.reserve(node.mNumMeshes);

	for (size_t i = 0; i < node.mNumMeshes; ++i)
	{
		const unsigned int MESH_INDEX = node.mMeshes[i];
		SkinnedMesh* mesh = mSkinnedMeshes[MESH_INDEX].get();

		meshes.push_back(mesh);
	}

	auto pNode = std::make_unique<SkinnedNode>(nextId++, node.mName.C_Str(), std::move(meshes), transform);

	for (size_t i = 0; i < node.mNumChildren; i++)
	{
		pNode->addChild(parseNodeRecursive(nextId, *node.mChildren[i]));
	}

	return pNode;
}

void SkinnedModel::linkBoneNode()
{
	std::map<std::string, const SkinnedNode*> nodeMap;
	nodeRecursive(mRoot.get(), &nodeMap);

	for (auto& skinnedMesh : mSkinnedMeshes)
	{
		for (SkinnedMesh::Bone& bone : skinnedMesh->mBones)
		{
			auto find = nodeMap.find(bone.Name);
			assert(find != nodeMap.end());

			bone.NodePtr = find->second;
		}
	}
}

void SkinnedModel::nodeRecursive(SkinnedNode* node, std::map<std::string, const SkinnedNode*>* outNodeMap)
{
	outNodeMap->insert({ node->GetName(), node });

	for (auto& child : node->mChildren)
	{
		nodeRecursive(child.get(), outNodeMap);
	}
}

GeometryModel::GeometryModel(Graphics& graphics, const GeometryGenerator::MeshData& meshData)
	: IDrawable(eDrawableType::Geometry)
	, mMeshData(meshData)
{
	Dvtx::VertexLayout vertexLayout;
	vertexLayout.Append(Dvtx::VertexLayout::Position3D)
		.Append(Dvtx::VertexLayout::Texture2D)
		.Append(Dvtx::VertexLayout::Normal)
		.Append(Dvtx::VertexLayout::Tangent);

	Dvtx::VertexBuffer vertices{ vertexLayout };

	Vector3 min;
	Vector3 max;

	for (auto& vertex : meshData.Vertices)
	{
		vertices.EmplaceBack(vertex.Position, vertex.UV, vertex.Normal, vertex.TangentU);

		min = Vector3::Min(min, vertex.Position);
		max = Vector3::Max(max, vertex.Position);
	}

	DirectX::BoundingBox::CreateFromPoints(mBoundingBox, min, max);
	DirectX::BoundingSphere::CreateFromBoundingBox(mBoundingSphere, mBoundingBox);

	using namespace Bind;
	pVertices = std::make_shared<VertexBuffer>(graphics, vertices);
	pIndices = std::make_shared<IndexBuffer>(graphics, meshData.Indices);
	pTopology = Topology::Create(graphics);

	auto vertexShader = VertexShader::Create(graphics, "../Resources/Shader/Model_VS.hlsl");
	auto vertexShadowShader = VertexShader::Create(graphics, "../Resources/Shader/ModelShadow_VS.hlsl");
	auto vertexSsaoShader = VertexShader::Create(graphics, "../Resources/Shader/SsaoDepthNormalBasic_VS.hlsl");
	auto inputLayout = InputLayout::Create(graphics, vertexLayout, vertexShader->GetBytecode());

	{
		Technique map("ShadowMap");

		{
			Step step("shadowPass");

			step.AddBindable(vertexShadowShader);
			step.AddBindable(inputLayout);
			step.AddBindable(std::make_shared<TransformCbufShadow>(graphics, 0u));

			map.AddStep(step);
		}

		AddTechnique(map);
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
	{
		Technique map("RenderTechnique");

		{
			Step step("renderPass");

			step.AddBindable(vertexShader);
			step.AddBindable(inputLayout);
			step.AddBindable(std::make_shared<TransformCbuf>(graphics, 0u));
			step.AddBindable(std::make_shared<PixelShader>(graphics, "../Resources/Shader/Model_PS.hlsl"));

			Dcb::RawLayout PSLayout;
			PSLayout.Add<Dcb::Bool>("useDiffuse")
				.Add<Dcb::Bool>("useNormal")
				.Add<Dcb::Bool>("useSpecular")
				.Add<Dcb::Bool>("useAlpha");
			Dcb::Buffer PSBuffer{ std::move(PSLayout) };

			PSBuffer["useDiffuse"] = false;
			PSBuffer["useNormal"] = false;
			PSBuffer["useSpecular"] = false;
			PSBuffer["useAlpha"] = false;

			step.AddBindable(std::make_unique<Bind::CachingPixelConstantBufferEx>(graphics, std::move(PSBuffer), 1u));
			step.AddBindable(std::make_shared<PixelCBufAlpha>(graphics, 2));

			map.AddStep(step);
		}

		AddTechnique(map);
	}
	{
		Technique map("HasAlphaRenderTechnique");

		{
			Step step("hasAlphaRenderPass");

			step.AddBindable(vertexShader);
			step.AddBindable(inputLayout);
			step.AddBindable(std::make_shared<TransformCbuf>(graphics, 0u));
			step.AddBindable(std::make_shared<PixelShader>(graphics, "../Resources/Shader/Model_PS.hlsl"));

			Dcb::RawLayout PSLayout;
			PSLayout.Add<Dcb::Bool>("useDiffuse")
				.Add<Dcb::Bool>("useNormal")
				.Add<Dcb::Bool>("useSpecular")
				.Add<Dcb::Bool>("useAlpha");
			Dcb::Buffer PSBuffer{ std::move(PSLayout) };

			PSBuffer["useDiffuse"] = false;
			PSBuffer["useNormal"] = false;
			PSBuffer["useSpecular"] = false;
			PSBuffer["useAlpha"] = false;

			step.AddBindable(std::make_unique<Bind::CachingPixelConstantBufferEx>(graphics, std::move(PSBuffer), 1u));
			step.AddBindable(std::make_shared<PixelCBufAlpha>(graphics, 2));

			map.AddStep(step);
		}

		AddTechnique(map);
	}
}

void GeometryModel::Submit()
{
	InstanceData instanceData;
	instanceData.Transform = mTransform;
	instanceData.AlphaData = mAlphaData;

	if (mAlphaData.bUsed)
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

void GeometryModel::Reset()
{
	mInstanceDataArr.clear();
	mHasAlphaInstanceDataArr.clear();
}

UIModel::UIModel(Graphics& graphics, const std::string& texturePath)
	: IDrawable(eDrawableType::UI)
{
	Dvtx::VertexLayout vertexLayout;
	vertexLayout.Append(Dvtx::VertexLayout::Position2D)
		.Append(Dvtx::VertexLayout::Texture2D);

	Dvtx::VertexBuffer vertices{ vertexLayout };

	vertices.EmplaceBack(Vector2(0, 0), Vector2(0, 0));
	vertices.EmplaceBack(Vector2(1, 0), Vector2(1, 0));
	vertices.EmplaceBack(Vector2(0, -1), Vector2(0, 1));
	vertices.EmplaceBack(Vector2(1, -1), Vector2(1, 1));

	using namespace Bind;
	std::vector<UINT> indices = { 0, 1, 2, 1, 3, 2 };
	pVertices = std::make_shared<VertexBuffer>(graphics, vertices);
	pIndices = std::make_shared<IndexBuffer>(graphics, indices);
	pTopology = Topology::Create(graphics);

	auto vertexShader = VertexShader::Create(graphics, "../Resources/Shader/UI_VS.hlsl");
	auto pixelShader = PixelShader::Create(graphics, "../Resources/Shader/UI_PS.hlsl");
	auto inputLayout = InputLayout::Create(graphics, vertexLayout, vertexShader->GetBytecode());
	auto texture = Texture::Create(graphics, texturePath, 0);

	// 동적 정점 버퍼랑 텍스처만 바인딩 대상
	// 그냥 랜더와 투명도 랜더에 연결해두기
	{
		Technique map("RenderTechnique");

		{
			Step step("UIRenderPass");

			step.AddBindable(vertexShader);
			step.AddBindable(inputLayout);
			step.AddBindable(pixelShader);
			step.AddBindable(texture);
			step.AddBindable(std::make_shared<TransformCbufUI>(graphics));
			step.AddBindable(std::make_shared<PixelCBufAlpha>(graphics));

			map.AddStep(step);
		}

		AddTechnique(map);
	}
	{
		Technique map("HasAlphaRenderTechnique");

		{
			Step step("hasAlphaUIRenderPass");

			step.AddBindable(vertexShader);
			step.AddBindable(inputLayout);
			step.AddBindable(pixelShader);
			step.AddBindable(texture);
			step.AddBindable(std::make_shared<TransformCbufUI>(graphics));
			step.AddBindable(std::make_shared<PixelCBufAlpha>(graphics));

			map.AddStep(step);
		}

		AddTechnique(map);
	}
}

void UIModel::Submit()
{
	InstanceData instanceData;
	instanceData.Transform = mTransform;
	instanceData.TexTransform = mTexTransform;
	instanceData.AlphaData = mAlphaData;
	instanceData.Layer = mLayer;

	if (mAlphaData.bUsed)
	{
		if (mHasAlphaInstanceDataArr[mLayer].size() == 0)
		{
			IDrawable::Submit();
		}

		mHasAlphaInstanceDataArr[mLayer].push_back(instanceData);
	}
	else
	{
		if (mInstanceDataArr[mLayer].size() == 0)
		{
			IDrawable::Submit();
		}

		mInstanceDataArr[mLayer].push_back(instanceData);
	}
}

void UIModel::Reset()
{
	for (size_t i = 0; i < UI_MAX_LAYER; ++i)
	{
		mInstanceDataArr[i].clear();
	}
	for (size_t i = 0; i < UI_MAX_LAYER; ++i)
	{
		mHasAlphaInstanceDataArr[i].clear();
	}
}

UI3DModel::UI3DModel(Graphics& graphics, const std::string& texturePath)
	: IDrawable(eDrawableType::UI3D)
{
	using namespace Bind;

	Dvtx::VertexLayout vertexLayout;
	vertexLayout.Append(Dvtx::VertexLayout::Position2D)
		.Append(Dvtx::VertexLayout::Texture2D);

	Dvtx::VertexBuffer vertices{ vertexLayout };

	vertices.EmplaceBack(Vector2(0, 0), Vector2(0, 0));
	vertices.EmplaceBack(Vector2(1, 0), Vector2(1, 0));
	vertices.EmplaceBack(Vector2(0, -1), Vector2(0, 1));
	vertices.EmplaceBack(Vector2(1, -1), Vector2(1, 1));

	using namespace Bind;
	std::vector<UINT> indices = { 0, 1, 2, 1, 3, 2 };
	pVertices = std::make_shared<VertexBuffer>(graphics, vertices);
	pIndices = std::make_shared<IndexBuffer>(graphics, indices);
	pTopology = Topology::Create(graphics);

	auto vertexShader = VertexShader::Create(graphics, "../Resources/Shader/UI3D_VS.hlsl");
	auto inputLayout = InputLayout::Create(graphics, vertexLayout, vertexShader->GetBytecode());
	auto geoShader = GeometryShader::Create(graphics, "../Resources/Shader/UI3D_GS.hlsl");
	auto pixelShader = PixelShader::Create(graphics, "../Resources/Shader/UI3D_PS.hlsl");
	auto texture = Texture::Create(graphics, texturePath, 0);

	// 동적 정점 버퍼랑 텍스처만 바인딩 대상
	// 그냥 랜더와 투명도 랜더에 연결해두기
	{
		Technique map("RenderTechnique");

		{
			Step step("UI3DRenderPass");

			step.AddBindable(vertexShader);
			step.AddBindable(inputLayout);
			step.AddBindable(geoShader);
			step.AddBindable(pixelShader);
			step.AddBindable(texture);
			step.AddBindable(std::make_shared<GeometryCBufUI3D>(graphics));
			step.AddBindable(std::make_shared<PixelCBufAlpha>(graphics));

			map.AddStep(step);
		}

		AddTechnique(map);
	}
	{
		Technique map("AlphaRenderTechnique");

		{
			Step step("hasAlphaUI3DRenderPass");

			step.AddBindable(vertexShader);
			step.AddBindable(inputLayout);
			step.AddBindable(geoShader);
			step.AddBindable(pixelShader);
			step.AddBindable(texture);
			step.AddBindable(std::make_shared<GeometryCBufUI3D>(graphics));
			step.AddBindable(std::make_shared<PixelCBufAlpha>(graphics));

			map.AddStep(step);
		}

		AddTechnique(map);
	}
}

void UI3DModel::Submit()
{
	InstanceData instanceData;
	instanceData.TexTransform = mTexTransform;
	instanceData.LookVector = mLookVector;
	instanceData.AlphaData = mAlphaData;
	instanceData.Position = mPosition;
	instanceData.Size = mSize;
	instanceData.Rotation = mRotation;
	instanceData.Layer = mLayer;

	if (mAlphaData.bUsed)
	{
		if (mHasAlphaInstanceDataArr[mLayer].size() == 0)
		{
			IDrawable::Submit();
		}

		mHasAlphaInstanceDataArr[mLayer].push_back(instanceData);
	}
	else
	{
		if (mInstanceDataArr[mLayer].size() == 0)
		{
			IDrawable::Submit();
		}

		mInstanceDataArr[mLayer].push_back(instanceData);
	}
}

void UI3DModel::Reset()
{
	for (size_t i = 0; i < UI_MAX_LAYER; ++i)
	{
		mInstanceDataArr[i].clear();
	}
	for (size_t i = 0; i < UI_MAX_LAYER; ++i)
	{
		mHasAlphaInstanceDataArr[i].clear();
	}
}

CutSceneModel::CutSceneModel(Graphics& graphics, const std::vector<std::string>& texturePaths)
	: mCapacity(texturePaths.size())
	, mIndex(0u)
{
	using namespace Bind;

	Dvtx::VertexLayout lay;
	lay.Append(Dvtx::VertexLayout::Position2D);
	Dvtx::VertexBuffer bufFull{ lay };
	bufFull.EmplaceBack(Vector2{ -1, 1 });
	bufFull.EmplaceBack(Vector2{ 1, 1 });
	bufFull.EmplaceBack(Vector2{ -1, -1 });
	bufFull.EmplaceBack(Vector2{ 1, -1 });
	pVertices = Bind::VertexBuffer::Create(graphics, "$Full", std::move(bufFull));

	std::vector<UINT> indices =
	{
		0,1,2,
		1,3,2
	};
	pIndices = Bind::IndexBuffer::Create(graphics, "$Full123", std::move(indices));
	pTopology = Bind::Topology::Create(graphics);

	auto vs = Bind::VertexShader::Create(graphics, "../Resources/Shader/Fullscreen_VS.hlsl");

	{
		Technique map("RenderTechnique");

		{
			Step step("cutSceneRenderPass");

			step.AddBindable(std::make_shared<TextureArray>(graphics, texturePaths, 0));
			step.AddBindable(std::make_shared<PixelCBufCutScene>(graphics));
			step.AddBindable(Bind::InputLayout::Create(graphics, lay, vs->GetBytecode()));
			step.AddBindable(std::move(vs));

			map.AddStep(step);
		}

		AddTechnique(map);
	}
}

BackgroundModel::BackgroundModel(Graphics& graphics, const std::string& texturePath)
{
	using namespace Bind;

	Dvtx::VertexLayout lay;
	lay.Append(Dvtx::VertexLayout::Position2D);
	Dvtx::VertexBuffer bufFull{ lay };
	bufFull.EmplaceBack(Vector2{ -1, 1 });
	bufFull.EmplaceBack(Vector2{ 1, 1 });
	bufFull.EmplaceBack(Vector2{ -1, -1 });
	bufFull.EmplaceBack(Vector2{ 1, -1 });
	pVertices = Bind::VertexBuffer::Create(graphics, "$Full", std::move(bufFull));

	std::vector<UINT> indices =
	{
		0,1,2,
		1,3,2
	};
	pIndices = Bind::IndexBuffer::Create(graphics, "$Full123", std::move(indices));
	pTopology = Bind::Topology::Create(graphics);

	auto vs = Bind::VertexShader::Create(graphics, "../Resources/Shader/Fullscreen_VS.hlsl");

	// 텍스처 배열 
	auto texture = Texture::Create(graphics, texturePath, 0);

	{
		Technique map("RenderTechnique");

		{
			Step step("backgroundRenderPass");

			step.AddBindable(texture);
			step.AddBindable(Bind::InputLayout::Create(graphics, lay, vs->GetBytecode()));
			step.AddBindable(std::move(vs));

			map.AddStep(step);
		}

		AddTechnique(map);
	}
}

ParticleModel::ParticleModel(Graphics& graphics, const std::string& texturePath)
{
	using namespace Bind;

	Dvtx::VertexLayout vertexLayout;
	vertexLayout.Append(Dvtx::VertexLayout::Position3D)
		.Append(Dvtx::VertexLayout::Velocity)
		.Append(Dvtx::VertexLayout::QuadSize)
		.Append(Dvtx::VertexLayout::Age);

	Dvtx::VertexBuffer vertices{ vertexLayout };

	vertices.EmplaceBack(Vector2(0, 0), Vector2(0, 0));
	vertices.EmplaceBack(Vector2(1, 0), Vector2(1, 0));
	vertices.EmplaceBack(Vector2(0, -1), Vector2(0, 1));
	vertices.EmplaceBack(Vector2(1, -1), Vector2(1, 1));

	using namespace Bind;
	std::vector<UINT> indices = { 0, 1, 2, 1, 3, 2 };
	mInitVB = std::make_shared<VertexBuffer>(graphics, vertices);
	mSOVB = std::make_shared<VertexBuffer>(graphics, vertices);
	mDrawVB = std::make_shared<VertexBuffer>(graphics, vertices);
	pVertices = mInitVB;
	pIndices = std::make_shared<IndexBuffer>(graphics, indices);
	pTopology = Topology::Create(graphics);

	auto vertexShader = VertexShader::Create(graphics, "../Resources/Shader/UI_VS.hlsl");
	auto inputLayout = InputLayout::Create(graphics, vertexLayout, vertexShader->GetBytecode());
	auto geoShader = GeometryShader::Create(graphics, "../Resources/Shader/UI3D_GS.hlsl");
	auto pixelShader = PixelShader::Create(graphics, "../Resources/Shader/UI3D_PS.hlsl");
	auto texture = Texture::Create(graphics, texturePath, 0);

	// 동적 정점 버퍼랑 텍스처만 바인딩 대상
	// 그냥 랜더와 투명도 랜더에 연결해두기
	{
		Technique map("RenderTechnique");

		{
			Step step("UI3DRenderPass");

			step.AddBindable(vertexShader);
			step.AddBindable(inputLayout);
			step.AddBindable(geoShader);
			step.AddBindable(pixelShader);
			step.AddBindable(texture);
			step.AddBindable(std::make_shared<GeometryCBufUI3D>(graphics));
			step.AddBindable(std::make_shared<PixelCBufAlpha>(graphics));

			map.AddStep(step);
		}

		AddTechnique(map);
	}
	{
		Technique map("AlphaRenderTechnique");

		{
			Step step("hasAlphaUI3DRenderPass");

			step.AddBindable(vertexShader);
			step.AddBindable(inputLayout);
			step.AddBindable(geoShader);
			step.AddBindable(pixelShader);
			step.AddBindable(texture);
			step.AddBindable(std::make_shared<GeometryCBufUI3D>(graphics));
			step.AddBindable(std::make_shared<PixelCBufAlpha>(graphics));

			map.AddStep(step);
		}

		AddTechnique(map);
	}
}

void ParticleModel::Submit()
{

}

void ParticleModel::Reset()
{

}