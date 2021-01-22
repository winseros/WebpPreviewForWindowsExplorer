#pragma once

#include <Unknwnbase.h>

namespace webpext
{
    class ClassFactory final : public IClassFactory
    {
    private:
        long ref_count_;
        ~ClassFactory();
    public:
        ClassFactory();
        ClassFactory(ClassFactory&) = default;
        ClassFactory(ClassFactory&&) = default;
        ClassFactory& operator=(ClassFactory const&) = default;
        ClassFactory& operator=(ClassFactory&&) = default;

#pragma region IUnknown
        HRESULT STDMETHODCALLTYPE QueryInterface(const IID& riid, void** ppvObject) override;
        ULONG STDMETHODCALLTYPE AddRef() override;
        ULONG STDMETHODCALLTYPE Release() override;
#pragma endregion

#pragma region IClassFacotory
        HRESULT STDMETHODCALLTYPE CreateInstance(IUnknown* pUnkOuter, const IID& riid, void** ppvObject) override;
        HRESULT STDMETHODCALLTYPE LockServer(BOOL fLock) override;
#pragma endregion
    };
}
