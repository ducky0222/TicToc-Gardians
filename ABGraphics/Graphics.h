#pragma once

namespace Bind
{
	class RenderTarget;
	class DepthStencil;
}

class Graphics
{
	friend class GraphicsResource;

public:
	Graphics(HWND hWnd, int width, int height, bool bUseImgui);
	~Graphics();
	Graphics(const Graphics&) = delete;
	Graphics& operator=(const Graphics&) = delete;

	void OnResize(UINT width, UINT height);

	void BeginFrame();
	void DrawIndexed(UINT count);
	void EndFrame();

	void SetProjection(Matrix proj);
	void SetView(Matrix view);
	void SetShadowProjection(Matrix proj) { mShadowProjection = proj; }
	void SetShadowView(Matrix view) { mShadowView = view; }

	inline void SetUseImgui(bool bUseImgui);

	inline float GetFov()
	{
		if (mProjection._41 == 0.0f && mProjection._42 == 0.0f && mProjection._44 != 0.0f)
		{
			return 0;
		}

		return 2.0f * atan(1.0f / mProjection._22);
	}
	inline float GetFarZ()
	{
		if (mProjection._41 == 0.0f && mProjection._42 == 0.0f && mProjection._44 != 0.0f)
		{
			return 1 / mProjection._33;
		}

		return  mProjection._34 / (mProjection._33 - 1.f);
	}
	inline Matrix GetProjection() const;
	inline Matrix GetView() const;
	inline Matrix GetViewProj() const;
	inline Matrix GetShadowProj() const { return mShadowProjection; }
	inline Matrix GetShadowView() const { return mShadowView; }
	inline Matrix GetShadowViewProj() const { return mShadowView * mShadowProjection; }

	inline UINT GetWidth() const;
	inline UINT GetHeight() const;
	inline std::shared_ptr<Bind::RenderTarget> GetRenderTarget() { return mpRenderTarget; }
	inline std::shared_ptr<Bind::RenderTarget> GetPostEffectRenderTarget() { return mpPostEffectRenderTarget; }
	inline std::shared_ptr<Bind::RenderTarget> GetBackBufferRenderTarget() { return mpBackBufferRenderTarget; }
	inline std::shared_ptr<Bind::DepthStencil> GetDepthStencil() { return mpDepthStncil; }
	ID3D11ShaderResourceView* GetRenderTargetSRV();
	inline bool GetUseImgui() const;

	auto GetDevice() { return mpDevice; }
	auto GetContext() { return mpContext; }

private:
	Microsoft::WRL::ComPtr<ID3D11Device> mpDevice;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> mpContext;
	Microsoft::WRL::ComPtr<IDXGISwapChain> mpSwapChain;
	std::shared_ptr<Bind::RenderTarget> mpRenderTarget;
	std::shared_ptr<Bind::RenderTarget> mpPostEffectRenderTarget;
	std::shared_ptr<Bind::RenderTarget> mpBackBufferRenderTarget;
	std::shared_ptr<Bind::DepthStencil> mpDepthStncil;

	Matrix mProjection;
	Matrix mView;
	Matrix mShadowProjection;
	Matrix mShadowView;

	UINT mWidth;
	UINT mHeight;

	bool mbUseImgui;
};

void Graphics::SetUseImgui(bool bUseImgui)
{
	mbUseImgui = bUseImgui;
}

Matrix Graphics::GetProjection() const
{
	return mProjection;
}
Matrix Graphics::GetView() const
{
	return mView;
}
Matrix Graphics::GetViewProj() const
{
	return mView * mProjection;
}

UINT Graphics::GetWidth() const
{
	return mWidth;
}
UINT Graphics::GetHeight() const
{
	return mHeight;
}

bool Graphics::GetUseImgui() const
{
	return mbUseImgui;
}
