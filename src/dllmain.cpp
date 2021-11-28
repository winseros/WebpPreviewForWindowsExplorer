#include "dllmain.h"
#include <ShlObj.h>
#include <new>
#include <filesystem>
#include "ClassFactory.h"
#include "Registry.h"

namespace webpext
{
    long g_dllRefCount = 0;

    void IncreaseDllRefCount()
    {
        InterlockedIncrement(&g_dllRefCount);
    }

    void DecreaseDllRefCount()
    {
        InterlockedDecrement(&g_dllRefCount);
    }
}

HINSTANCE g_hInstDll = nullptr;

STDAPI DllMain(
    HINSTANCE hInstDll,
    DWORD fdwReason,
    LPVOID lpReserved)
{
    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH:
        g_hInstDll = hInstDll;
        DisableThreadLibraryCalls(hInstDll);
        break;
    case DLL_PROCESS_DETACH:
        g_hInstDll = nullptr;
        break;
    }
    return TRUE;
}

using namespace webpext;

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, void** ppv)
{
    HRESULT hr = CLASS_E_CLASSNOTAVAILABLE;
    if (IsEqualCLSID(CLSID_WebpThumbProvider, rclsid))
    {
        hr = E_OUTOFMEMORY;
        auto* pClassFactory = new(std::nothrow) ClassFactory();
        if (pClassFactory)
        {
            hr = pClassFactory->QueryInterface(riid, ppv);
            pClassFactory->Release();
        }
    }

    return hr;
}

STDAPI DllCanUnloadNow()
{
    return g_dllRefCount > 0 ? S_FALSE : S_OK;
}

STDAPI DllRegisterServer() {
    using namespace std::filesystem;

    HRESULT hr;

    TCHAR buf[MAX_PATH];
    if (GetModuleFileName(g_hInstDll, buf, MAX_PATH)) {
        const path dllPath(buf);
        hr = Registry::registerServer(dllPath.string());
    } else {
        hr = HRESULT_FROM_WIN32(GetLastError());
    }

    return hr;
}

STDAPI DllUnregisterServer() {
    return Registry::unregisterServer();
}
