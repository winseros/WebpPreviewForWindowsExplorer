#include "Registry.h"
#include <ShlObj.h>
#include <Windows.h>
#include "dllmain.h"

namespace webpext
{
    HRESULT Registry::registerServer(const string& pathToDll)
    {
        try
        {
            registerServerImpl(pathToDll);
            SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, NULL, NULL);
            return S_OK;
        }
        catch (const RegistryException& ex)
        {
            return HRESULT_FROM_WIN32(ex.status());
        }
    }

    HRESULT Registry::unregisterServer()
    {
        try
        {
            unregisterServerImpl();
            SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, NULL, NULL);
            return S_OK;
        }
        catch (const RegistryException& ex)
        {
            return HRESULT_FROM_WIN32(ex.status());
        }
    }

    void Registry::registerServerImpl(const string& pathToDll)
    {
        setRegistryKeyValue("Software\\Classes\\.webp\\ShellEx\\{e357fccd-a995-4576-b01f-234630154e96}",
                            WEBP_SHELL_CLSID);

        setRegistryKeyValue("Software\\Classes\\CLSID\\" WEBP_SHELL_CLSID,
                            WEBP_PROJECT_NAME);
        setRegistryKeyValue("Software\\Classes\\CLSID\\" WEBP_SHELL_CLSID "\\InprocServer32",
                            pathToDll);
        setRegistryKeyValue("Software\\Classes\\CLSID\\" WEBP_SHELL_CLSID "\\InprocServer32",
                            "Apartment",
                            "ThreadingModel");
    }

    void Registry::unregisterServerImpl()
    {
        removeRegistryKey("Software\\Classes\\CLSID\\" WEBP_SHELL_CLSID);
    }

#define HKEY_CLASS HKEY_CURRENT_USER

    void Registry::setRegistryKeyValue(const string& key, const string& value, const string& name)
    {
        HKEY reg;
        LSTATUS ls = RegCreateKeyEx(HKEY_CLASS,
                                    key.data(),
                                    0,
                                    NULL,
                                    REG_OPTION_NON_VOLATILE,
                                    KEY_SET_VALUE | KEY_WOW64_64KEY,
                                    NULL,
                                    &reg,
                                    NULL);

        if (ls != ERROR_SUCCESS)
            throw RegistryException::fromLStatus(key, ls);

        ls = RegSetKeyValue(reg, NULL, name.empty() ? NULL : name.data(), REG_SZ, value.data(),
                            static_cast<DWORD>(value.length()));
        RegCloseKey(reg);

        if (ls != ERROR_SUCCESS)
            throw RegistryException::fromLStatus(key, ls);
    }

    void Registry::removeRegistryKey(const string& key)
    {
        const LSTATUS ls = RegDeleteTree(HKEY_CLASS, key.data());

        if (ls != ERROR_SUCCESS)
            throw RegistryException::fromLStatus(key, ls);
    }

    Registry::RegistryException Registry::RegistryException::fromLStatus(const string& registryKey, LSTATUS status)
    {
        const string systemError = getSystemMessage(status);
        return {registryKey, systemError, status};
    }

    Registry::RegistryException::
    RegistryException(string registryKey, const string& systemError, LSTATUS status)
        : exception(systemError.data()),
          registryKey_(std::move(registryKey)),
          status_(status)
    {
    }

    LSTATUS Registry::RegistryException::status() const
    {
        return status_;
    }

    const string& Registry::RegistryException::registryKey() const
    {
        return registryKey_;
    }

    string Registry::RegistryException::getSystemMessage(LSTATUS status)
    {
        TCHAR buffer[2048];
        const DWORD size = sizeof buffer / sizeof TCHAR;

        const DWORD number = FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,
                                           NULL,
                                           status,
                                           LANG_USER_DEFAULT,
                                           buffer,
                                           size,
                                           NULL);
        return {buffer, number};
    }
}
