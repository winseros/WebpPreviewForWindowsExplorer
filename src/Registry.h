#pragma once

#include <string>
#include <Windows.h>

namespace webpext
{
    using namespace std;

    class Registry {
    public:
        static HRESULT registerServer(const string& pathToDll);

        static HRESULT unregisterServer();

    private:
        static void registerServerImpl(const string& pathToDll);

        static void unregisterServerImpl();

        static void setRegistryKeyValue(const string& key, const string& value, const string& name = "");

        static void removeRegistryKey(const string& key);

        class RegistryException : public exception {
        public:
            static RegistryException fromLStatus(const string& registryKey, LSTATUS status);

            RegistryException(string registryKey, const string& systemError, LSTATUS status);

            LSTATUS status() const;

            const string& registryKey() const;

        private:
            string registryKey_;
            LSTATUS status_;

            static string getSystemMessage(LSTATUS status);
        };
    };
}
