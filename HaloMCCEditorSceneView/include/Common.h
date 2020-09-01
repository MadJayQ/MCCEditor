#pragma once

#include <Windows.h>

#include <string>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

//This semaphore prevents the rendering module from being loaded multiple times
extern HANDLE g_hModuleSemaphore;