#include "pch.h"
#include "Sample.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_CrtSetBreakAlloc(5133);

	int result = 0;
	{
		entryPoint::Sample sample(hInstance, 1920, 1080, L"TestApp");

		if (!sample.Init())
		{
			return 0;
		}
		result = sample.Run();
	}

	//_CrtDumpMemoryLeaks();
	return result;
}