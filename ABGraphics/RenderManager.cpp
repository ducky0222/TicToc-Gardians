#include "pch.h"
#include "RenderManager.h"

#include "BindableResourceManager.h"
#include "BindableCommon.h"
#include "IDrawable.h"

#pragma region MyRegion


bool RenderManager::SubmitModel(std::string key)
{
	auto find = mModels.find(key);

	if (find == mModels.end())
	{
		return false;
	}

	find->second->Submit();

	return true;
}

bool RenderManager::SubmitSkinnedModel(std::string key)
{
	auto find = mSkinnedModels.find(key);

	if (find == mSkinnedModels.end())
	{
		return false;
	}

	find->second->Submit();

	return true;
}

bool RenderManager::SubmitGeometryModel(std::string key)
{
	auto find = mGeometryModels.find(key);

	if (find == mGeometryModels.end())
	{
		return false;
	}

	find->second->Submit();

	return true;
}

bool RenderManager::ApplyModelTransform(std::string key, const Matrix& transform)
{
	auto modelFinded = mModels.find(key);

	if (modelFinded == mModels.end())
	{
		return false;
	}

	modelFinded->second->SetTransform(transform);

	return true;
}
bool RenderManager::ApplySkinnedModelTransform(std::string key, const Matrix& transform)
{
	auto modelFinded = mSkinnedModels.find(key);

	if (modelFinded == mSkinnedModels.end())
	{
		return false;
	}

	modelFinded->second->SetTransform(transform);

	return true;
}
bool RenderManager::ApplyGeomodelTransform(std::string key, const Matrix& transform)
{
	auto modelFinded = mGeometryModels.find(key);

	if (modelFinded == mGeometryModels.end())
	{
		return false;
	}

	modelFinded->second->SetTransform(transform);

	return true;
}

#pragma endregion

RenderManager& RenderManager::GetInstance()
{
	static RenderManager instance;
	return instance;
}

Matrix RenderManager::MakeUITransform(Vector2 topLeft, Vector2 size, float rotation)
{
	//return
	//	UIModel::MakeTransform(topLeft, size, rotation)
	//	* Matrix::CreateTranslation(-(int)(mGraphics->GetWidth()), mGraphics->GetHeight(), 0)
	//	* Matrix::CreateScale(1.f / mGraphics->GetWidth(), 1.f / mGraphics->GetHeight(), 0);
	return
		UIModel::MakeTransform(topLeft, size, rotation)
		* Matrix::CreateTranslation(-(int)(mUICanvasSize.x), mUICanvasSize.y, 0)
		* Matrix::CreateScale(1.f / mUICanvasSize.x, 1.f / mUICanvasSize.y, 0);
}

void RenderManager::ConvertMousePosWindowsToViewPort(int* inOutScreenX, int* inOutScreenY)
{
	*inOutScreenX -= mEditorViewPortPos.x;
	*inOutScreenY -= mEditorViewPortPos.y;
}

void RenderManager::Init(HWND hwnd, UINT width, UINT height, bool bUseImgui)
{
	mGraphics = std::make_unique<Graphics>(hwnd, width, height, bUseImgui);
	mFrameLight = std::make_unique<FrameLight>(*mGraphics);
	mRenderGraph = std::make_unique<Rgph::ForwardRenderGraph>(*mGraphics, mFrameLight.get());

	mCamera = std::make_unique<Camera>();
	mShadowCamera = std::make_unique<Camera>();
}

void RenderManager::Destroy()
{
	mModels.clear();
	mAnimations.clear();

	mGraphics = nullptr;
	mRenderGraph = nullptr;
	mFrameLight = nullptr;

	Bind::ResourceManager::Get().destroy();
}

void RenderManager::OnResize(UINT width, UINT height)
{
	width = max(width, 200);
	height = max(height, 200);

	mGraphics->OnResize(width, height);
	mRenderGraph->OnResize(*mGraphics, width, height);
}

ID3D11ShaderResourceView* RenderManager::GetBackBufferSRV()
{
	return mGraphics->GetRenderTargetSRV();
}

void RenderManager::BeginRender()
{
	mCamera->UpdateViewMatrix();
	mShadowCamera->UpdateViewMatrix();

	auto data = mFrameLight->GetCBData();
	Vector3 lightDir = data.DirectionLights[0].Direction;
	Vector3 lightPos = -1.0f * mShadowRadius * lightDir;
	Vector3 targetPos = { 0, 0, 0 };
	Vector3 up = { 0.0f, 1.0f, 0.0f };

	mShadowCamera->SetLens(3.14 * 0.25, 1, 1, mShadowRadius * 2);
	mShadowCamera->LookAt(lightPos, targetPos, up);

	mGraphics->SetView(mCamera->GetView());
	mGraphics->SetProjection(mCamera->GetProj());
	mGraphics->SetShadowView(mShadowCamera->GetView());
	mGraphics->SetShadowProjection(mShadowCamera->GetProj());

	const Matrix& view = mGraphics->GetView();

	mFrameLight->Bind(*mGraphics);

	mGraphics->BeginFrame();
}

void RenderManager::Excute()
{
	mRenderGraph->Execute(*mGraphics);

	if (mGraphics->GetUseImgui())
	{
		mFrameLight->SpawnControlWindow();
	}
}

void RenderManager::EndRender()
{
	mGraphics->EndFrame();
	mRenderGraph->Reset();

	auto resetMap = [](auto& modelMap)
		{
			for (auto& model : modelMap)
			{
				model.second->Reset();
			}
		};

	resetMap(mModels);
	resetMap(mSkinnedModels);
	resetMap(mGeometryModels);
	resetMap(mUIModels);
	resetMap(mUI3DModels);
}

bool RenderManager::Submit(eModelType modelType, std::string key)
{
	auto submit = [](auto& map, std::string key)
		{
			auto find = map.find(key);

			if (find == map.end())
			{
				return false;
			}

			find->second->Submit();

			return true;
		};

	switch (modelType)
	{
	case eModelType::Basic:
		return submit(mModels, key);
	case eModelType::Skinned:
		return submit(mSkinnedModels, key);
	case eModelType::Geometry:
		return submit(mGeometryModels, key);
	case eModelType::UI:
		return submit(mUIModels, key);
	case eModelType::UI3D:
		return submit(mUI3DModels, key);
	case eModelType::CutScene:
		return submit(mCutSceneModels, key);
	case eModelType::Background:
		return submit(mBackgroundModels, key);
	default:
		assert(false);
		break;
	}

	return false;
}

bool RenderManager::Create(eModelType modelType, std::string key, std::string path)
{
	switch (modelType)
	{
	case eModelType::Basic:
		return CreateModel(key, path);
		break;
	case eModelType::Skinned:
		return CreateSkinnedModel(key, path);
		break;
	case eModelType::Geometry:
		assert(false);
		break;
	case eModelType::UI:
		return CreateUIModel(key, path);
		break;
	case eModelType::UI3D:
		return CreateUI3DModel(key, path);
	case eModelType::Animation:
		return CreateAnimation(key, path);
	case eModelType::Background:
		return CreateBackgroundModel(key, path);
	default:
		assert(false);
		break;
	}

	return false;
}
bool RenderManager::CreateModel(std::string key, std::string modelPath)
{
	auto find = mModels.find(key);

	if (find != mModels.end())
	{
		return false;
	}

	mModels.insert({ key, std::make_unique<Model>(*mGraphics, modelPath) });
	mModels[key]->LinkTechniques(*mRenderGraph);
}
bool RenderManager::CreateSkinnedModel(std::string key, std::string modelPath)
{
	auto find = mSkinnedModels.find(key);

	if (find != mSkinnedModels.end())
	{
		return false;
	}

	mSkinnedModels.insert({ key, std::make_unique<SkinnedModel>(*mGraphics, modelPath) });
	mSkinnedModels[key]->LinkTechniques(*mRenderGraph);
}
bool RenderManager::CreateGeometryModel(std::string key, const GeometryGenerator::MeshData& meshData)
{
	auto find = mGeometryModels.find(key);

	if (find != mGeometryModels.end())
	{
		return false;
	}

	mGeometryModels.insert({ key, std::make_unique<GeometryModel>(*mGraphics, meshData) });
	mGeometryModels[key]->LinkTechniques(*mRenderGraph);
}

bool RenderManager::CreateUIModel(std::string key, std::string texturePath)
{
	auto find = mUIModels.find(key);

	if (find != mUIModels.end())
	{
		return false;
	}

	mUIModels.insert({ key, std::make_unique<UIModel>(*mGraphics, texturePath) });
	mUIModels[key]->LinkTechniques(*mRenderGraph);
}

bool RenderManager::CreateUI3DModel(std::string key, std::string texturePath)
{
	auto find = mUI3DModels.find(key);

	if (find != mUI3DModels.end())
	{
		return false;
	}

	mUI3DModels.insert({ key, std::make_unique<UI3DModel>(*mGraphics, texturePath) });
	mUI3DModels[key]->LinkTechniques(*mRenderGraph);
}

bool RenderManager::CreateAnimation(std::string key, std::string animPath)
{
	auto find = mAnimations.find(key);

	if (find != mAnimations.end())
	{
		return false;
	}

	mAnimations.insert({ key, std::make_unique<AnimResource>(animPath) });
}
bool RenderManager::CreateCutSceneModel(std::string key, const std::vector<std::string>& texturePaths)
{
	auto find = mCutSceneModels.find(key);

	if (find != mCutSceneModels.end())
	{
		return false;
	}

	mCutSceneModels.insert({ key, std::make_unique<CutSceneModel>(*mGraphics, texturePaths) });
	mCutSceneModels[key]->LinkTechniques(*mRenderGraph);
}
bool RenderManager::CreateBackgroundModel(std::string key, std::string texturePath)
{
	auto find = mBackgroundModels.find(key);

	if (find != mBackgroundModels.end())
	{
		return false;
	}

	mBackgroundModels.insert({ key, std::make_unique<BackgroundModel>(*mGraphics, texturePath) });
	mBackgroundModels[key]->LinkTechniques(*mRenderGraph);
}


bool RenderManager::ApplyTransform(eModelType modelType, std::string key, const Matrix& transform)
{
	auto applyTransform = [](auto& map, const std::string& key, const Matrix& transform)
		{
			auto find = map.find(key);

			if (find == map.end())
			{
				return false;
			}

			find->second->SetTransform(transform);

			return true;
		};

	switch (modelType)
	{
	case eModelType::Basic:
		return applyTransform(mModels, key, transform);
	case eModelType::Skinned:
		return applyTransform(mSkinnedModels, key, transform);
	case eModelType::Geometry:
		return applyTransform(mGeometryModels, key, transform);
	case eModelType::UI:
		return applyTransform(mUIModels, key, transform);
	default:
		assert(false);
		break;
	}

	return false;
}

bool RenderManager::ApplyAlpha(eModelType modelType, std::string key, bool bUseAlpha, float alpha)
{
	auto applyAlpha = [](auto& map, const std::string& key, const IDrawable::AlphaData& alphaData)
		{
			auto find = map.find(key);

			if (find == map.end())
			{
				return false;
			}

			find->second->SetAlphaData(alphaData);

			return true;
		};

	IDrawable::AlphaData alphaData;
	alphaData.Alpha = alpha;
	alphaData.bUsed = bUseAlpha;

	switch (modelType)
	{
	case eModelType::Basic:
		return applyAlpha(mModels, key, alphaData);
	case eModelType::Skinned:
		return applyAlpha(mSkinnedModels, key, alphaData);
	case eModelType::Geometry:
		return applyAlpha(mGeometryModels, key, alphaData);
	case eModelType::UI:
		return applyAlpha(mUIModels, key, alphaData);
	case eModelType::UI3D:
		return applyAlpha(mUI3DModels, key, alphaData);
	case eModelType::CutScene:
		return applyAlpha(mCutSceneModels, key, alphaData);
	case eModelType::Background:
		return applyAlpha(mBackgroundModels, key, alphaData);
	default:
		assert(false);
		break;
	}

	return false;
}

bool RenderManager::ApplyTexTransform(eModelType modelType, std::string key, Vector2 size, Vector2 translate, float rotationInRadian)
{
	auto applyTexTransform = [](auto& map, const std::string& key, const Matrix& transform)
		{
			auto find = map.find(key);

			if (find == map.end())
			{
				return false;
			}

			find->second->SetTexTransform(transform);

			return true;
		};

	Matrix texTransform;
	texTransform =
		Matrix::CreateScale(size.x, size.y, 0)
		* Matrix::CreateTranslation(translate.x, translate.y, 0);

	switch (modelType)
	{
	case eModelType::Basic:
		return applyTexTransform(mModels, key, texTransform);
	case eModelType::Skinned:
		return applyTexTransform(mSkinnedModels, key, texTransform);
	case eModelType::Geometry:
		return applyTexTransform(mGeometryModels, key, texTransform);
	case eModelType::UI:
		return applyTexTransform(mUIModels, key, texTransform);
	case eModelType::UI3D:
		return applyTexTransform(mUI3DModels, key, texTransform);
	default:
		assert(false);
		break;
	}

	return false;
}

bool RenderManager::ApplyModelAnimation(std::string key, std::string aniName, std::string cilpName, float timePos, bool bIsRecursive)
{
	auto modelFinded = mModels.find(key);

	if (modelFinded == mModels.end())
	{
		return false;
	}

	auto animClipsFinded = mAnimations.find(aniName);

	if (animClipsFinded == mAnimations.end())
	{
		return false;
	}

	auto clips = animClipsFinded->second->GetAnimClips();
	auto animFinded = clips.find(cilpName);

	if (animFinded == clips.end())
	{
		return false;
	}

	modelFinded->second->ApplyAnimation(animFinded->second, timePos, bIsRecursive);

	return true;
}
bool RenderManager::ApplySkinnedModelAnimation(std::string key, std::string aniName, float timePos, bool bIsRecursive)
{
	auto modelFinded = mSkinnedModels.find(key);

	if (modelFinded == mSkinnedModels.end())
	{
		return false;
	}

	auto animClipsFinded = mAnimations.find(aniName);

	if (animClipsFinded == mAnimations.end())
	{
		return false;
	}

	auto clips = animClipsFinded->second->GetAnimClips();

	if (clips.empty())
	{
		return false;
	}

	auto animFinded = clips.begin();

	modelFinded->second->ApplyAnimation(animFinded->second, timePos, bIsRecursive);

	return true;
}
bool RenderManager::ApplySkinnedModelAnimation(std::string key, std::string aniName, std::string cilpName, float timePos, bool bIsRecursive)
{
	auto modelFinded = mSkinnedModels.find(key);

	if (modelFinded == mSkinnedModels.end())
	{
		return false;
	}

	auto animClipsFinded = mAnimations.find(aniName);

	if (animClipsFinded == mAnimations.end())
	{
		return false;
	}

	auto clips = animClipsFinded->second->GetAnimClips();
	auto animFinded = clips.find(cilpName);

	if (animFinded == clips.end())
	{
		return false;
	}

	modelFinded->second->ApplyAnimation(animFinded->second, timePos, bIsRecursive);

	return true;
}
bool RenderManager::ApplySkinnedModelBlendAnimation(std::string key, std::string aniName0, std::string aniName1, float timePos0, float timePos1, float weight, bool bIsRecursive0, bool bIsRecursive1)
{
	auto modelFinded = mSkinnedModels.find(key);

	if (modelFinded == mSkinnedModels.end())
	{
		return false;
	}

	auto animClipsFinded0 = mAnimations.find(aniName0);
	auto animClipsFinded1 = mAnimations.find(aniName1);

	if (animClipsFinded0 == mAnimations.end() || animClipsFinded1 == mAnimations.end())
	{
		return false;
	}

	auto clips0 = animClipsFinded0->second->GetAnimClips();
	auto clips1 = animClipsFinded1->second->GetAnimClips();

	if (clips0.empty() || clips1.empty())
	{
		return false;
	}

	auto animFinded0 = clips0.begin();
	auto animFinded1 = clips1.begin();


	modelFinded->second->ApplyAnimation(animFinded0->second, animFinded1->second, timePos0, timePos1, weight, bIsRecursive0, bIsRecursive1);

	return true;
}
bool RenderManager::ApplySkinnedModelBlendAnimation(std::string key, std::string aniName0, std::string cilpName0, std::string aniName1, std::string cilpName1, float timePos0, float timePos1, float weight, bool bIsRecursive0, bool bIsRecursive1)
{
	auto modelFinded = mSkinnedModels.find(key);

	if (modelFinded == mSkinnedModels.end())
	{
		return false;
	}

	auto animClipsFinded0 = mAnimations.find(aniName0);
	auto animClipsFinded1 = mAnimations.find(aniName1);

	if (animClipsFinded0 == mAnimations.end() || animClipsFinded1 == mAnimations.end())
	{
		return false;
	}

	auto clips0 = animClipsFinded0->second->GetAnimClips();
	auto clips1 = animClipsFinded1->second->GetAnimClips();
	auto animFinded0 = clips0.find(cilpName0);
	auto animFinded1 = clips1.find(cilpName1);

	if (animFinded0 == clips0.end() || animFinded1 == clips1.end())
	{
		return false;
	}

	modelFinded->second->ApplyAnimation(animFinded0->second, animFinded1->second, timePos0, timePos1, weight, bIsRecursive0, bIsRecursive1);

	return true;
}

bool RenderManager::ApplyUILayer(std::string key, size_t layer)
{
	auto find = mUIModels.find(key);

	if (find == mUIModels.end())
	{
		return false;
	}

	find->second->SetLayer(layer);

	return true;
}
bool RenderManager::ApplyUI3DLookVector(std::string key, const Vector3& lookVector)
{
	auto find = mUI3DModels.find(key);

	if (find == mUI3DModels.end())
	{
		return false;
	}

	find->second->SetLookVector(lookVector);

	return true;
}
bool RenderManager::ApplyUI3DPosition(std::string key, const Vector3& position)
{
	auto find = mUI3DModels.find(key);

	if (find == mUI3DModels.end())
	{
		return false;
	}

	find->second->SetPosition(position);

	return true;
}

bool RenderManager::ApplyUI3DSize(std::string key, const Vector2& size)
{
	auto find = mUI3DModels.find(key);

	if (find == mUI3DModels.end())
	{
		return false;
	}

	find->second->SetSize(size);

	return true;
}

bool RenderManager::ApplyUI3DLayer(std::string key, size_t layer)
{
	auto find = mUI3DModels.find(key);

	if (find == mUI3DModels.end())
	{
		return false;
	}

	find->second->SetLayer(layer);

	return true;
}

bool RenderManager::SetCutSceneIndex(std::string key, size_t index, bool bIsRecursive)
{
	auto find = mCutSceneModels.find(key);

	if (find == mCutSceneModels.end())
	{
		return false;
	}

	if (bIsRecursive)
	{
		index %= find->second->GetCapacity();
	}
	else
	{
		index = min(index, find->second->GetCapacity() - 1);
	}

	find->second->SetIndex(index);
}

bool RenderManager::GetBoundingBox(eModelType modelType, std::string key, DirectX::BoundingBox* outBoundingBox) const
{
	auto getBoundingBox = [](auto& map, const std::string& key, DirectX::BoundingBox* outBoundingBox)
		{
			auto find = map.find(key);

			if (find == map.end())
			{
				return false;
			}

			*outBoundingBox = find->second->GetBoundingBox();
			return true;
		};

	switch (modelType)
	{
	case eModelType::Basic:
		return getBoundingBox(mModels, key, outBoundingBox);
	case eModelType::Skinned:
		return getBoundingBox(mSkinnedModels, key, outBoundingBox);
	case eModelType::Geometry:
		return getBoundingBox(mGeometryModels, key, outBoundingBox);
	default:
		assert(false);
		break;
	}

	return false;
}
bool RenderManager::GetBoundingSphere(eModelType modelType, std::string key, DirectX::BoundingSphere* outBoundingSphere) const
{
	auto getBoundingSphere = [](auto& map, const std::string& key, DirectX::BoundingSphere* outBoundingSphere)
		{
			auto find = map.find(key);

			if (find == map.end())
			{
				return false;
			}

			*outBoundingSphere = find->second->GetBoundingSphere();
			return true;
		};

	switch (modelType)
	{
	case eModelType::Basic:
		return getBoundingSphere(mModels, key, outBoundingSphere);
	case eModelType::Skinned:
		return getBoundingSphere(mSkinnedModels, key, outBoundingSphere);
	case eModelType::Geometry:
		return getBoundingSphere(mGeometryModels, key, outBoundingSphere);
	default:
		assert(false);
		break;
	}

	return false;
}

DirectX::SimpleMath::Ray RenderManager::GetPickingRayInWorldViewPort(int screenX, int screenY, bool bUseOrtho) const
{
	return GetPickingRayInWorld(screenX, screenY, mEditorViewPortSize.x, mEditorViewPortSize.y, bUseOrtho);
}
DirectX::SimpleMath::Ray RenderManager::GetPickingRayInWorldWindows(int screenX, int screenY, bool bUseOrtho) const
{
	return GetPickingRayInWorld(screenX, screenY, mGraphics->GetWidth(), mGraphics->GetHeight(), bUseOrtho);
}

DirectX::SimpleMath::Ray RenderManager::GetPickingRayInWorld(int screenX, int screenY, int screenSizeX, int screenSizeY, bool bUseOrtho) const
{
	DirectX::SimpleMath::Ray ray;

	Matrix proj = mCamera->GetProj();
	Matrix invView;
	mCamera->GetView().Invert(invView);

	if (bUseOrtho)
	{
		float ndcX = ((2.0f * screenX) / screenSizeX) - 1.0f;
		float ndcY = 1.0f - ((2.0f * screenY) / screenSizeY);
		float ndcZ = 1.0f;

		Matrix invProj;
		proj.Invert(invProj);

		Vector3 rayOrigin = { ndcX, ndcY, 0.0f };
		Vector3 rayDir = { ndcX, ndcY, 1.0f };

		rayOrigin = Vector3::Transform(rayOrigin, invProj);
		rayOrigin = Vector3::Transform(rayOrigin, invView);

		rayDir = Vector3::TransformNormal(rayDir, invProj);
		rayDir = Vector3::TransformNormal(rayDir, invView);

		rayDir.Normalize();

		ray = { rayOrigin, rayDir };
	}
	else
	{
		float vx = (+2.0f * screenX / screenSizeX - 1.0f) / proj(0, 0);
		float vy = (-2.0f * screenY / screenSizeY + 1.0f) / proj(1, 1);

		Vector3 rayOrigin = { 0.0f, 0.0f, 0.0f };
		Vector3 rayDir = { vx, vy, 1.0f };

		rayOrigin = Vector3::Transform(rayOrigin, invView);
		rayDir = Vector3::TransformNormal(rayDir, invView);
		rayDir.Normalize();

		ray = { rayOrigin, rayDir };
	}

	return ray;
}

void RenderManager::SetEditorViewPort(Vector2 size, Vector2 pos)
{
	mEditorViewPortSize = size;
	mEditorViewPortPos = pos;
}
