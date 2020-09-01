#include "include/Common.h"

HANDLE g_hModuleSemaphore;

constexpr const wchar_t* g_wszSemaphoreName = L"SCENE_VIEW_SEMAPHORE";

BOOLEAN WINAPI EditorSceneViewDllMain(IN HINSTANCE hDllHandle,
    IN DWORD nReason,
    IN LPVOID Reserved);

//////////////////////////////////////////////////////////////////////

BOOLEAN WINAPI DllMain(IN HINSTANCE hDllHandle,
    IN DWORD     nReason,
    IN LPVOID    Reserved)
{
    BOOLEAN bSuccess = TRUE;


    //  Perform global initialization.

    switch (nReason)
    {
    case DLL_PROCESS_ATTACH:

        //  For optimization.

        DisableThreadLibraryCalls(hDllHandle);

        break;

    case DLL_PROCESS_DETACH:

        break;
    }


    //  Perform type-specific initialization.

    if (EditorSceneViewDllMain(hDllHandle, nReason, Reserved))
    {
        bSuccess = TRUE;
    }
    else
    {
        bSuccess = FALSE;
    }

    return bSuccess;

}

BOOLEAN WINAPI EditorSceneViewDllMain(IN HINSTANCE hDllHandle,
    IN DWORD nReason,
    IN LPVOID Reserved)
{
    switch (nReason)
    {
    case DLL_PROCESS_ATTACH:

        g_hModuleSemaphore = CreateSemaphoreW(NULL, 0, 1, g_wszSemaphoreName);

        if (g_hModuleSemaphore == NULL) return FALSE;

        //  Set initial count to 1.

        if (GetLastError() != ERROR_ALREADY_EXISTS)
            ReleaseSemaphore(g_hModuleSemaphore, 1, NULL);

        break;

    case DLL_PROCESS_DETACH:

        if (g_hModuleSemaphore != NULL)
        {
            CloseHandle(g_hModuleSemaphore);
            g_hModuleSemaphore = NULL;
        }

        break;
    }
    return TRUE;

}
