#pragma once

#include "include/Common.h"

#ifdef SCENEVIEWLIB_EXPORTS
#define SCENEVIEW_API __declspec(dllexport)
#else
#define SCENEVIEW_API __declspec(dllimport)
#endif

extern "C" SCENEVIEW_API int InitializeRendererFromWindow(HWND targetWnd, RECT rect);
extern "C" SCENEVIEW_API void Shutdown();