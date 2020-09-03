#pragma once

#include "Common.h"

#ifdef SCENEVIEWLIB_EXPORTS
#define SCENEVIEW_API __declspec(dllexport)
#else
#define SCENEVIEW_API __declspec(dllimport)
#endif

extern "C" SCENEVIEW_API int InitializeRendererFromWindow(HWND targetWnd, RECT rect);
extern "C" SCENEVIEW_API void Shutdown();
extern "C" SCENEVIEW_API void SetShaderLocation(const std::string & shaderLocation);

extern "C" SCENEVIEW_API void BeginFrame();
extern "C" SCENEVIEW_API void EndFrame();

extern "C" SCENEVIEW_API void ResizeClientWindow(RECT newRect);