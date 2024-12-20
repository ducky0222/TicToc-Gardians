#include "pch.h"
#include "FontManager.h"

#pragma region ���ڿ� ��ƿ
#include <cwchar>
#include <cerrno>

static std::wstring StringToWstring(const std::string& str);
static errno_t CharArrayToWcharArray(const char* charArray, wchar_t* wcharArray, size_t size);
#pragma endregion

#pragma region Non SingleTon
FontManager::FontManager(ID3D11Device* device, ID3D11DeviceContext* context, ID3D11RasterizerState* rs, ID3D11DepthStencilState* ds)
{
	m_spriteBatch = new DirectX::SpriteBatch(context);

	std::wstring fileName = L"../Resources/Font/DNFBitBit.spritefont";

	m_spriteFont = new DirectX::SpriteFont(device, fileName.c_str());
	m_spriteFont->SetLineSpacing(14.0f);

	/// ���������� device context�� device���� �޾ƿͼ� �� �� �����ϰ� �����µ�
	/// �޾ƿ��� �Ŷ� �׳� �������� �Ŷ� �ٸ�����
	/// ���� ������ �̻������°� �ƴ��� Ȯ��

	// solidDesc.FillMode = D3D11_FILL_SOLID;
	//solidDesc.CullMode = D3D11_CULL_NONE;
	//solidDesc.FrontCounterClockwise = false;
	//solidDesc.DepthClipEnable = true;
	m_rasterizerState = rs;

	//equalsDesc.DepthEnable = true;
	//equalsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	//equalsDesc.DepthFunc = D3D11_COMPARISON_LESS;
	m_depthStencilState = ds;
}

FontManager::FontManager(ID3D11Device* device, ID3D11DeviceContext* context)
{
	m_spriteBatch = new DirectX::SpriteBatch(context);

	std::wstring fileName = L"../Resources/Font/myfile.spritefont";

	m_spriteFont = new DirectX::SpriteFont(device, fileName.c_str());
	m_spriteFont->SetLineSpacing(14.0f);
}
#pragma endregion

#pragma region SingleTon

void FontManager::Init(ID3D11Device* device, ID3D11DeviceContext* context)
{
	m_device = device;
	m_context = context;
	m_spriteBatch = new DirectX::SpriteBatch(m_context);

	std::wstring fileName = L"../Resources/Font/myfile.spritefont";

	m_spriteFont = new DirectX::SpriteFont(m_device, fileName.c_str());
	m_spriteFont->SetLineSpacing(14.0f);
}

void FontManager::Destroy()
{
	delete m_spriteFont;	m_spriteFont = nullptr;
	delete m_spriteBatch;	m_spriteBatch = nullptr;
}

#pragma endregion

/// <summary>
/// graphics�� begin render ����
/// </summary>
/// <param name="mode"></param>
void FontManager::BeginRender(SpriteSortMode mode /*= DX11::SpriteSortMode_Deferred*/)
{
	m_spriteBatch->Begin(mode);
}

// string�̳� char��
/// �׳� �׸��� ���� ���� ȣ��
void FontManager::Render(float x, float y, DirectX::SimpleMath::Vector4 color, std::wstring text)
{
	Vector2 origin = m_spriteFont->MeasureString(text.c_str()) / 2.0f;
	m_spriteFont->DrawString(m_spriteBatch, text.c_str(), DirectX::SimpleMath::Vector2(x, y), color, 0.0f, origin);
}

void FontManager::Render(std::string text, DirectX::SimpleMath::Vector2 pos, DirectX::SimpleMath::Vector4 color, float rot /*= 0.0f*/, float scale /*= 1.0f*/)
{
	std::wstring currentText = StringToWstring(text);
	Vector2 origin = m_spriteFont->MeasureString(currentText.c_str()) / 2.0f;
	m_spriteFont->DrawString(m_spriteBatch, currentText.c_str(), pos, color, rot, origin, scale);
}

void FontManager::Render(const char* text, DirectX::SimpleMath::Vector2 pos, DirectX::SimpleMath::Vector4 color, float rot /*= 0.0f*/, float scale /*= 1.0f*/)
{
	wchar_t* currentText = nullptr;
	errno_t err = CharArrayToWcharArray(text, currentText, sizeof(text));

	if (err == 0)
	{
		if (currentText)
		{
			Vector2 origin = m_spriteFont->MeasureString(currentText) / 2.0f;
			m_spriteFont->DrawString(m_spriteBatch, currentText, pos, color, rot, origin, scale);
		}
	}
#ifdef _DEBUG
	else
	{
		OutputDebugStringA("char to wchar failed");
	}
#endif
}

/// <summary>
/// graphics�� present ����
/// </summary>
void FontManager::EndRender()
{
	m_spriteBatch->End();
}

static std::wstring StringToWstring(const std::string& str)
{
	if (str.empty())
		return L"";

	int size = MultiByteToWideChar(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), NULL, 0);
	std::wstring result(size, 0);
	MultiByteToWideChar(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), result.data(), size);
	return result;
}

static errno_t CharArrayToWcharArray(const char* charArray, wchar_t* wcharArray, size_t size)
{
	size_t convertedChars = 0;
	errno_t err = mbstowcs_s(&convertedChars, wcharArray, size, charArray, _TRUNCATE);
	return err;
}