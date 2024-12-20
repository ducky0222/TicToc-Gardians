#include "pch.h"

#include "Graphics.h"
#include "RenderTarget.h"
#include "DepthStencil.h"
#include "Util.h"

Graphics::Graphics(HWND hWnd, int width, int height, bool bUseImgui)
	: mWidth(width)
	, mHeight(height)
	, mbUseImgui(bUseImgui)
{
	UINT createDeviceFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)  
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_FEATURE_LEVEL featureLevel;
	HRESULT hr = D3D11CreateDevice(
		0,                 // default adapter
		D3D_DRIVER_TYPE_HARDWARE,
		0,                 // no software device
		createDeviceFlags,
		0, 0,              // default feature level array
		D3D11_SDK_VERSION,
		mpDevice.GetAddressOf(),
		&featureLevel,
		mpContext.GetAddressOf());

	if (FAILED(hr))
	{
		MessageBox(0, L"D3D11CreateDevice Failed.", 0, 0);
	}

	if (featureLevel != D3D_FEATURE_LEVEL_11_0)
	{
		MessageBox(0, L"Direct3D Feature Level 11 unsupported.", 0, 0);
	}

	DXGI_SWAP_CHAIN_DESC sd;
	sd.BufferDesc.Width = mWidth;
	sd.BufferDesc.Height = mHeight;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;

	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = 1;
	sd.OutputWindow = hWnd;
	sd.Windowed = true;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags = 0;

	IDXGIDevice* dxgiDevice = 0;
	mpDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice);

	IDXGIAdapter* dxgiAdapter = 0;
	dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&dxgiAdapter);

	IDXGIFactory* dxgiFactory = 0;
	dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&dxgiFactory);

	dxgiFactory->CreateSwapChain(mpDevice.Get(), &sd, &mpSwapChain);

	ReleaseCOM(dxgiDevice);
	ReleaseCOM(dxgiAdapter);
	ReleaseCOM(dxgiFactory);

	// 후면버퍼를 참조로하는 RenderTarget 생성
	Microsoft::WRL::ComPtr<ID3D11Texture2D> pBackBuffer;
	hr = mpSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), &pBackBuffer);
	mpBackBufferRenderTarget = std::shared_ptr<Bind::OutputOnlyRenderTarget>(new Bind::OutputOnlyRenderTarget(*this, pBackBuffer.Get()));
	mpDepthStncil = std::make_shared<Bind::OutputOnlyDepthStencil>(*this);
	mpRenderTarget = std::make_shared<Bind::ShaderInputRenderTarget>(*this, mWidth, mHeight, 0, DXGI_FORMAT_R16G16B16A16_FLOAT);
	mpPostEffectRenderTarget = std::make_shared<Bind::ShaderInputRenderTarget>(*this, mWidth, mHeight, 0, DXGI_FORMAT_R16G16B16A16_FLOAT);

	// 뷰포트 설정
	D3D11_VIEWPORT vp = CD3D11_VIEWPORT();
	vp.TopLeftX = 0.0f;
	vp.TopLeftY = 0.0f;
	vp.Width = static_cast<float>(width);
	vp.Height = static_cast<float>(height);
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	mpContext->RSSetViewports(1u, &vp);

	if (bUseImgui)
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking

		ImGui::StyleColorsDark();

		ImGui_ImplWin32_Init(hWnd);
		ImGui_ImplDX11_Init(mpDevice.Get(), mpContext.Get());
	}
}

Graphics::~Graphics()
{
	if (mbUseImgui)
	{
		ImGui_ImplDX11_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
	}
}

void Graphics::OnResize(UINT width, UINT height)
{
	assert(mpDevice);
	assert(mpContext);
	assert(mpSwapChain);

	mWidth = width;
	mHeight = height;

	mpRenderTarget->Release();
	mpPostEffectRenderTarget->Release();
	mpBackBufferRenderTarget->Release();

	HRESULT hr = mpSwapChain->ResizeBuffers(0, mWidth, mHeight, DXGI_FORMAT_R8G8B8A8_UNORM, 0);
	Microsoft::WRL::ComPtr<ID3D11Texture2D> pBackBuffer;
	hr = mpSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), &pBackBuffer);

	mpRenderTarget->OnResize(*this, mWidth, mHeight);
	mpPostEffectRenderTarget->OnResize(*this, mWidth, mHeight);
	mpBackBufferRenderTarget->OnResize(*this, pBackBuffer.Get());
	mpDepthStncil->OnResize(*this);

	D3D11_VIEWPORT vp = CD3D11_VIEWPORT();
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	vp.Width = static_cast<float>(mWidth);
	vp.Height = static_cast<float>(mHeight);
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;

	mpContext->RSSetViewports(1, &vp);
}

void Graphics::BeginFrame()
{
	if (mbUseImgui)
	{
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
	}
}
void Graphics::DrawIndexed(UINT count)
{
	mpContext->DrawIndexed(count, 0u, 0u);
}
void Graphics::EndFrame()
{
	if (mbUseImgui)
	{
		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	}

	mpSwapChain->Present(1u, 0u);
}

void Graphics::SetProjection(Matrix proj)
{
	mProjection = proj;
}
void Graphics::SetView(Matrix view)
{
	mView = view;
}

ID3D11ShaderResourceView* Graphics::GetRenderTargetSRV()
{
	return dynamic_pointer_cast<Bind::ShaderInputRenderTarget>(mpPostEffectRenderTarget)->GetSRV();
}
//void Graphics::SetIsEnableImgui(bool bIsEnableImgui)
//{
//
//}

//bool Graphics::IsEnableImgui()
//{
//	
//}
