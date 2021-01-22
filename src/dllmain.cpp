#include "dllmain.h"
#include <ShlObj.h>
#include <new>
#include "ClassFactory.h"

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

HINSTANCE g_hInstDll{nullptr};

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

#define SZ_CLSID_WEBPTHUMBHANDLER     L"{C39D7E2A-FC3C-4DEE-BE80-380065FC2C2A}"
#define SZ_WEBPTHUMBHANDLER           L"Webp Thumbnail Handler"
static const GUID CLSID_WebpThumbProvider =
    {0xc39d7e2a, 0xfc3c, 0x4dee, {0xbe, 0x80, 0x38, 0x0, 0x65, 0xfc, 0x2c, 0x2a}};

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

struct REGISTRY_ENTRY
{
    HKEY hkeyRoot;
    PCWSTR pszKeyName;
    PCWSTR pszValueName;
    PCWSTR pszData;
};

HRESULT CreateRegKeyAndSetValue(const REGISTRY_ENTRY* pRegistryEntry)
{
    HKEY hKey;
    HRESULT hr = HRESULT_FROM_WIN32(RegCreateKeyExW(pRegistryEntry->hkeyRoot, pRegistryEntry->pszKeyName,
                                                    0, NULL, REG_OPTION_NON_VOLATILE, KEY_SET_VALUE, NULL, &hKey,
                                                    NULL));
    if (SUCCEEDED(hr))
    {
        hr = HRESULT_FROM_WIN32(RegSetValueExW(hKey, pRegistryEntry->pszValueName, 0, REG_SZ,
                                               (LPBYTE)pRegistryEntry->pszData,
                                               (static_cast<DWORD>(wcslen(pRegistryEntry->pszData)) + 1) * sizeof(WCHAR
                                               )));
        RegCloseKey(hKey);
    }
    return hr;
}

STDAPI DllRegisterServer()
{
    HRESULT hr;

    WCHAR szModuleName[MAX_PATH];

    if (!GetModuleFileNameW(g_hInstDll, szModuleName, ARRAYSIZE(szModuleName)))
    {
        hr = HRESULT_FROM_WIN32(GetLastError());
    }
    else
    {
        // List of registry entries we want to create
        const REGISTRY_ENTRY rgRegistryEntries[] =
        {
            {
                HKEY_CURRENT_USER,
                L"Software\\Classes\\CLSID\\" SZ_CLSID_WEBPTHUMBHANDLER,
                NULL,
                SZ_WEBPTHUMBHANDLER
            },
            {
                HKEY_CURRENT_USER,
                L"Software\\Classes\\CLSID\\" SZ_CLSID_WEBPTHUMBHANDLER L"\\InProcServer32",
                NULL,
                szModuleName
            },
            {
                HKEY_CURRENT_USER,
                L"Software\\Classes\\CLSID\\" SZ_CLSID_WEBPTHUMBHANDLER L"\\InProcServer32",
                L"ThreadingModel",
                L"Apartment"
            },
            {
                HKEY_CURRENT_USER,
                L"Software\\Classes\\.webp\\ShellEx\\{e357fccd-a995-4576-b01f-234630154e96}",
                NULL,
                SZ_CLSID_WEBPTHUMBHANDLER
            }
        };

        hr = S_OK;
        for (auto i = 0; i < ARRAYSIZE(rgRegistryEntries) && SUCCEEDED(hr); i++)
        {
            hr = CreateRegKeyAndSetValue(&rgRegistryEntries[i]);
        }
    }
    if (SUCCEEDED(hr))
    {
        SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, NULL, NULL);
    }
    return hr;
}

STDAPI DllUnregisterServer()
{
    HRESULT hr = S_OK;

    const PCWSTR rgpszKeys[] =
    {
        L"Software\\Classes\\CLSID\\" SZ_CLSID_WEBPTHUMBHANDLER,
        L"Software\\Classes\\.webp\\ShellEx\\{e357fccd-a995-4576-b01f-234630154e96}"
    };

    for (int i = 0; i < ARRAYSIZE(rgpszKeys) && SUCCEEDED(hr); i++)
    {
        hr = HRESULT_FROM_WIN32(RegDeleteTreeW(HKEY_CURRENT_USER, rgpszKeys[i]));
        if (hr == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND))
        {
            hr = S_OK;
        }
    }
    return hr;
}
