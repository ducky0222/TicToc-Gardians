#pragma once

#include <directxtk/SpriteBatch.h>
#include <directxtk/SpriteFont.h>

/// <summary>
/// SpriteFont를 사용해서 글씨를 그리자
/// </summary>
class FontManager
{
public:
	static FontManager& GetInstance()
	{
		static FontManager instance;
		return instance;
	}

#pragma region Non SingleTon
	// Begin 함수 설정을 바꾸려면 이거
	FontManager(ID3D11Device* device, ID3D11DeviceContext* context, ID3D11RasterizerState* rs, ID3D11DepthStencilState* ds);
	FontManager(ID3D11Device* device, ID3D11DeviceContext* context);
#pragma endregion

#pragma region SingleTon
	FontManager() = default;
	~FontManager() = default;
	void Init(ID3D11Device* device, ID3D11DeviceContext* context);
	void Destroy();
#pragma endregion

	void BeginRender(SpriteSortMode mode = DX11::SpriteSortMode_Deferred);
	void Render(float x, float y, DirectX::SimpleMath::Vector4 color, std::wstring text);
	void Render(std::string text, DirectX::SimpleMath::Vector2 pos, DirectX::SimpleMath::Vector4 color, float rot = 0.0f, float scale = 1.0f);
	void Render(const char* text, DirectX::SimpleMath::Vector2 pos, DirectX::SimpleMath::Vector4 color, float rot = 0.0f, float scale = 1.0f);
	void EndRender();

private:
	DirectX::SpriteBatch* m_spriteBatch = nullptr;
	DirectX::SpriteFont* m_spriteFont = nullptr;


	// 그냥 사용하면 뎁스 문제가 생긴다.
	ID3D11RasterizerState* m_rasterizerState = nullptr;
	ID3D11DepthStencilState* m_depthStencilState = nullptr;

	ID3D11DeviceContext* m_context = nullptr;
	ID3D11Device* m_device = nullptr;
};

