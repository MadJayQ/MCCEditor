#include <windows.h>

//Include our SceneView API Definition file
#include <ApiDef.h>

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

HWND sandboxWND;

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

    SetShaderLocation("C:\\Users\\NASCARAdmin\\Documents\\GitHub\\MCCEditor\\HaloMCCEditorSceneView\\shaders");
    InitializeRendererFromWindow(sandboxWND, clientRect);
    Shutdown();

    while (GetMessage(&msg, NULL, 0, 0) > 0)
        DispatchMessage(&msg);

    return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

    switch (message)
    {
    case WM_CLOSE:
        PostQuitMessage(0);
        break;
    case WM_PAINT:
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;

}