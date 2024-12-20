#pragma once

#include "DebugDraw.h"
#include "Camera.h"

class DebugRenderManager
{
public:
	struct Ring
	{
		DirectX::SimpleMath::Vector3 Pos;
		DirectX::SimpleMath::Vector3 XAxis;
		DirectX::SimpleMath::Vector3 YAxis;
		DirectX::SimpleMath::Color Color;
	};

public:
	static DebugRenderManager& Get()
	{
		static DebugRenderManager instance;
		return instance;
	}

	void Init(ID3D11Device* device, ID3D11DeviceContext* context, const Camera* mainCamera)
	{
		mDevice = device;
		mContext = context;
		mMainCamera = mainCamera;
		debugDraw::Initialize(device, context);
	}
	void Destroy()
	{
		debugDraw::Uninitialize();
	}

	void Submit(const DirectX::BoundingBox& boundingBoxInWorld, DirectX::SimpleMath::Color color = { 0, 1, 0, 1 })
	{
		mBoundingBoxes.push_back({ boundingBoxInWorld, color });
	}

	void Submit(const Ring& ring)
	{
		mRings.push_back({ ring });
	}

	void Submit(const DirectX::BoundingSphere& boundingSphereInWorld, DirectX::SimpleMath::Color color = { 0, 1, 0, 1 })
	{
		mBoundingSpheres.push_back({ boundingSphereInWorld, color });
	}

	void Submit(const DirectX::BoundingOrientedBox& obb, DirectX::SimpleMath::Color color = { 0, 1, 0, 1 })
	{
		mOBBs.push_back({ obb, color });
	}

	void Submit(const DirectX::SimpleMath::Ray& rayInWorld, DirectX::SimpleMath::Color color = { 0, 1, 0, 1 })
	{
		mRayArr.push_back({ rayInWorld, color });
	}

	void BeginRender()
	{
		mContext->OMSetBlendState(debugDraw::g_States->Opaque(), nullptr, 0xFFFFFFFF);
		//md3dDevice->RSSetState(debugDraw::g_States->CullNone());

		debugDraw::g_BatchEffect->Apply(mContext);
		debugDraw::g_BatchEffect->SetView(mMainCamera->GetView());
		debugDraw::g_BatchEffect->SetProjection(mMainCamera->GetProj());

		mContext->IASetInputLayout(debugDraw::g_pBatchInputLayout.Get());

		debugDraw::g_Batch->Begin();
	}

	void Excute()
	{
		if (mbIsActive)
		{
			for (const auto& boundingBox : mBoundingBoxes)
			{
				debugDraw::Draw(debugDraw::g_Batch.get(), boundingBox.first, boundingBox.second);
			}
			for (const auto& obb : mOBBs)
			{
				debugDraw::Draw(debugDraw::g_Batch.get(), obb.first, obb.second);
			}
			for (const auto& ring : mRings)
			{
				debugDraw::DrawRing(debugDraw::g_Batch.get(), ring.Pos, ring.XAxis, ring.YAxis, ring.Color);
			}
			for (const auto& sphere : mBoundingSpheres)
			{
				debugDraw::Draw(debugDraw::g_Batch.get(), sphere.first, sphere.second);
			}
			for (const auto& ray : mRayArr)
			{
				debugDraw::DrawRay(debugDraw::g_Batch.get(), ray.first.position, ray.first.direction, false, ray.second);
			}
		}
	}

	void EndRender()
	{
		debugDraw::g_Batch->End();
		mBoundingBoxes.clear();
		mOBBs.clear();
		mRings.clear();
		mBoundingSpheres.clear();
		mRayArr.clear();
	}

	void SetActive(bool bIsActive) { mbIsActive = bIsActive; }

	bool GetActive() const { return mbIsActive; }

private:
	ID3D11Device* mDevice;
	ID3D11DeviceContext* mContext;
	const Camera* mMainCamera;

	std::vector<std::pair<DirectX::BoundingSphere, DirectX::SimpleMath::Color>> mBoundingSpheres;
	std::vector<std::pair<DirectX::BoundingOrientedBox, DirectX::SimpleMath::Color>> mOBBs;
	std::vector < std::pair<DirectX::BoundingBox, DirectX::SimpleMath::Color>> mBoundingBoxes;
	std::vector<std::pair<DirectX::SimpleMath::Ray, DirectX::SimpleMath::Color>> mRayArr;
	std::vector<Ring> mRings;

	bool mbIsActive{ true };
};

