#pragma once

#include "Graphics.h"
#include "ForwardRenderGraph.h"
#include "FrameLight.h"

#include "Model.h"
#include "Animation.h"
#include "GeometryGenerator.h"
#include "Camera.h"
#include "eModelType.h"

enum class eProjectionType
{
	Perspective,
	Orthographic
};

class RenderManager
{
	// 지워질 함수들, 새로 만드는 데서는 해당 함수 쓰지 마세유
public:
	bool SubmitModel(std::string key);
	bool SubmitSkinnedModel(std::string key);
	bool SubmitGeometryModel(std::string key);

	bool ApplyModelTransform(std::string key, const Matrix& trnasform);
	bool ApplySkinnedModelTransform(std::string key, const Matrix& transform);
	bool ApplyGeomodelTransform(std::string key, const Matrix& transform);

public:
	static RenderManager& GetInstance();

	void Init(HWND hwnd, UINT width, UINT height, bool bUseImgui);
	void Destroy();
	void OnResize(UINT width, UINT height);

	ID3D11ShaderResourceView* GetBackBufferSRV();

	void BeginRender();
	void Excute();
	void EndRender();

	bool Submit(eModelType modelType, std::string key);

	bool Create(eModelType modelType, std::string key, std::string path);
	bool CreateModel(std::string key, std::string modelPath);
	bool CreateSkinnedModel(std::string key, std::string modelPath);
	bool CreateUIModel(std::string key, std::string texturePath);
	bool CreateUI3DModel(std::string key, std::string texturePath);
	bool CreateAnimation(std::string key, std::string animPath);
	bool CreateGeometryModel(std::string key, const GeometryGenerator::MeshData& meshData);
	bool CreateCutSceneModel(std::string key, const std::vector<std::string>& texturePaths);
	bool CreateBackgroundModel(std::string key, std::string texturePath);

	bool ApplyTransform(eModelType modelType, std::string key, const Matrix& transform);
	bool ApplyTexTransform(eModelType modelType, std::string key, Vector2 size, Vector2 translate, float rotationInRadian);
	bool ApplyAlpha(eModelType modelType, std::string key, bool bUseAlpha, float alpha);

	bool ApplyModelAnimation(std::string key, std::string aniName, std::string cilpName, float timePos, bool bIsRecursive);
	bool ApplySkinnedModelAnimation(std::string key, std::string aniName, float timePos, bool bIsRecursive);
	bool ApplySkinnedModelAnimation(std::string key, std::string aniName, std::string cilpName, float timePos, bool bIsRecursive);
	bool ApplySkinnedModelBlendAnimation(std::string key, std::string aniName0, std::string aniName1, float timePos0, float timePos1, float weight, bool bIsRecursive0, bool bIsRecursive1);
	bool ApplySkinnedModelBlendAnimation(std::string key, std::string aniName0, std::string cilpName0, std::string aniName1, std::string cilpName1, float timePos0, float timePos1, float weight, bool bIsRecursive0, bool bIsRecursive1);
	bool ApplyUILayer(std::string key, size_t layer);
	bool ApplyUI3DLookVector(std::string key, const Vector3& cameraPos);
	bool ApplyUI3DPosition(std::string key, const Vector3& position);
	bool ApplyUI3DSize(std::string key, const Vector2& size);
	bool ApplyUI3DLayer(std::string key, size_t layer);

	Matrix MakeUITransform(Vector2 topLeft, Vector2 size, float rotation);
	void ConvertMousePosWindowsToViewPort(int* inOutScreenX, int* inOutScreenY);

	void ResetSkymap() { mRenderGraph->ResetSkymap(); }
	void ResetIBL() { mRenderGraph->ResetIBL(); }
	void SetSkymap(const std::string& path) { mRenderGraph->SetSkymap(*mGraphics, path); }
	void SetIBL(const std::string& diffuse, const std::string& specular, const std::string& brdfLUT) { mRenderGraph->SetIBL(*mGraphics, diffuse, specular, brdfLUT); }
	void SetShadowRadius(float radius) { mShadowRadius = radius; }
	bool SetCutSceneIndex(std::string key, size_t index, bool bIsRecursive);
	void SetEditorViewPort(Vector2 size, Vector2 pos);
	void SetUICanvasSize(const Vector2& UICanvasSize) { mUICanvasSize = UICanvasSize; }

	FrameLight* GetFrameLight() { return mFrameLight.get(); }
	Camera* GetCamera() { return mCamera.get(); }
	Camera* GetShadowCamera() { return mShadowCamera.get(); }
	ID3D11Device* GetDevice() { assert(mGraphics != nullptr); return mGraphics->GetDevice().Get(); }
	ID3D11DeviceContext* GetContext() { assert(mGraphics != nullptr); return mGraphics->GetContext().Get(); }
	const std::map<std::string, std::unique_ptr<AnimResource>>& GetAnimations() const { return mAnimations; }
	bool GetBoundingBox(eModelType modelType, std::string key, DirectX::BoundingBox* outBoundingBox) const;
	bool GetBoundingSphere(eModelType modelType, std::string key, DirectX::BoundingSphere* outBoundingSphere) const;
	DirectX::SimpleMath::Ray GetPickingRayInWorldViewPort(int screenX, int screenY, bool bUseOrtho = false) const;
	DirectX::SimpleMath::Ray GetPickingRayInWorldWindows(int screenX, int screenY, bool bUseOrtho = false) const;
	DirectX::SimpleMath::Ray GetPickingRayInWorld(int screenX, int screenY, int screenSizeX, int screenSizeY, bool bUseOrtho = false) const;

	Vector2 GetEditorViewPortSize() const { return mEditorViewPortSize; }
	Vector2 GetEditorViewPortPos() const { return mEditorViewPortPos; }
	Vector2 GetUICanvasSize() const { return mUICanvasSize; }

private:
	std::unique_ptr<Graphics> mGraphics;
	std::unique_ptr<Rgph::ForwardRenderGraph> mRenderGraph;
	std::unique_ptr<FrameLight> mFrameLight;

	std::map<std::string, std::unique_ptr<Model>> mModels;
	std::map<std::string, std::unique_ptr<SkinnedModel>> mSkinnedModels;
	std::map<std::string, std::unique_ptr<GeometryModel>> mGeometryModels;
	std::map<std::string, std::unique_ptr<UIModel>> mUIModels;
	std::map<std::string, std::unique_ptr<UI3DModel>> mUI3DModels;
	std::map<std::string, std::unique_ptr<AnimResource>> mAnimations;
	std::map<std::string, std::unique_ptr<CutSceneModel>> mCutSceneModels;
	std::map<std::string, std::unique_ptr<BackgroundModel>> mBackgroundModels;

	// 카메라 포함하는 형태로 변경 예정
	eProjectionType mProjectionType;
	std::unique_ptr<Camera> mCamera;
	std::unique_ptr<Camera> mShadowCamera;
	float mShadowRadius = 250.f;

	Vector2 mEditorViewPortSize;
	Vector2 mEditorViewPortPos;

	Vector2 mUICanvasSize{ 1920, 1080 };
};


//template <typename ...Args>
//bool RenderManager::Create(eModelType modelType, std::string key, Args... args)
//{
	//switch (modelType)
	//{
	//constexpr case eModelType::Basic:
	//	return CreateModel(key, args...);
	//	break;
	//constexpr case eModelType::Skinned:
	//	return CreateSkinnedModel(key, args...);
	//	break;
	//constexpr case eModelType::Geometry:
	//	return CreateGeometryModel(key, args...);
	//	break;
	//constexpr case eModelType::UI:
	//	return CreateUIModel(key, args...);
	//	break;
	//constexpr default:
	//	assert(false);
	//	break;
	//}

//	return false;
//}

