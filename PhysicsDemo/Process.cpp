#include "pch.h"
#include "Process.h"
#include "PhysicsEngine.h"

using namespace DirectX;
using namespace Microsoft::WRL;
using Vector2 = DirectX::SimpleMath::Vector2;
using Vector3 = DirectX::SimpleMath::Vector3;
using Vector4 = DirectX::SimpleMath::Vector4;
using Matrix = DirectX::SimpleMath::Matrix;
using Quaternion = DirectX::SimpleMath::Quaternion;

#include "../ABGraphics/Camera.h"
#include "../ABGraphics/RenderManager.h"

Process::Process(HINSTANCE hInst, std::string_view title, int width, int height, bool bImGuiInit)
	: GameProcess(hInst, title, width, height, bImGuiInit)
{
	Physics::GetInstance()->Initialize();

	// »ý¼º
}
Process::~Process()
{

}

void Process::Update()
{

}

void Process::Render(Renderer* renderer)
{

}

void Process::OnImGuiRender()
{

}
