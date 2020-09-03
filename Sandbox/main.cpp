#include <windows.h>

//Include our SceneView API Definition file
#include <ApiDef.h>

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
VOID WINAPI Render();
BOOL WINAPI WindowResizing(WPARAM wParam, LPARAM lParam);
BOOL WINAPI WindowResize(WPARAM wParam, LPARAM lParam);

HWND sandboxWND;
BOOL shouldTerminate = FALSE;
BOOL isResizing = FALSE;

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

	//SetShaderLocation("D:\\HaloMCCEditor\\HaloMCCEditorSceneView\\shaders");
	SetShaderLocation("C:\\Users\\NASCARAdmin\\Documents\\GitHub\\MCCEditor\\HaloMCCEditorSceneView\\shaders");
	InitializeRendererFromWindow(sandboxWND, clientRect);
	//Shutdown();

	while (!shouldTerminate)
	{
		if (PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		if (!isResizing)
		{
			Render();
		}
		if (msg.message == WM_QUIT)
		{
			shouldTerminate = true;
		}
	}
	Shutdown();

	return 0;
}

BOOL WINAPI WindowResizing(WPARAM wParam, LPARAM lParam)
{
	isResizing = TRUE;
	return TRUE;
}

BOOL WINAPI WindowResize(WPARAM wParam, LPARAM lParam)
{

	if (sandboxWND == NULL)
	{
		return FALSE;
	}

	isResizing = FALSE;
	int width = LOWORD(lParam);
	int height = HIWORD(lParam);

	RECT newRect;
	GetClientRect(sandboxWND, &newRect);
	
	ResizeClientWindow(newRect);

	return TRUE;
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
	case WM_SIZE:
		WindowResize(wParam, lParam);
		break;
	case WM_SIZING:
		return WindowResizing(wParam, lParam);
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;

}