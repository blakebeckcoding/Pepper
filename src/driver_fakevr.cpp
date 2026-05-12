DLL entry point, it exports HmdDriverFactory()

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <openvr_driver.h>
#include "FakeVRProvider.h"

#static instance
static FakeVRProvider g_provider;

#hmd driver factory
extern "C" __declspec(dllexport)
void* HmdDriverFactory(const char* pInterfaceName, int* pReturnCode)
{
    if (!strcmp(pInterfaceName, vr::IServerTrackedDeviceProvider_Version))
    {
        return &g_provider;
    }

    if (pReturnCode)
        *pReturnCode = vr::VRInitError_Init_InterfaceNotFound;
    return nullptr;
}

#standard dll entry
BOOL WINAPI DllMain(HINSTANCE, DWORD fdwReason, LPVOID)
{
    (void)fdwReason;
    return TRUE;
}
