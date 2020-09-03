#include <windows.h>

//Include our SceneView API Definition file
#include <ApiDef.h>

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
VOID WINAPI Render();

HWND sandboxWND;
BOOL shouldTerminate = FALSE;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{

	MSG msg = { 0 };
	WNDCLASS wc = { 0 };
	wc.lpfnWndProc = WndProc;
	wc.hInstance = hInstance;
	wc.hbrBackground = (HBRUSH)(COLOR_BACKGROUND);
	wc.lpszClassName = L"minwindowsapp";
	if (!RegisterClass(&wc))
		return 1;

	sandboxWND = CreateWindow(wc.lpszClassName,
		L"SANDBOX",
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		0, 0, 1920, 1080, 0, 0, hInstance, NULL);

	if (sandboxWND == NULL)
		return 2;

	RECT clientRect;
	GetClientRect(sandboxWND, &clientRect);

	SetShaderLocation("D:\\HaloMCCEditor\\HaloMCCEditorSceneView\\shaders");
	InitializeRendererFromWindow(sandboxWND, clientRect);
	//Shutdown();

	while (!shouldTerminate)
	{
		if(PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		Render();
		if (msg.message == WM_QUIT)
		{
			shouldTerminate = true;
		}
	}
	Shutdown();

	return 0;
}

VOID WINAPI Render()
{
	//PROFILEPUSH("Frame render start")
	BeginFrame();
	EndFrame();
	//PROFILEPOP()
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

	switch (message)
	{
	case WM_CLOSE:
		shouldTerminate = true;
		PostQuitMessage(0);
		break;
	case WM_PAINT:
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;

}