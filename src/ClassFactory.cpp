#include <Shlwapi.h>
#include <new>

#include "ClassFactory.h"
#include "ThumbnailProvider.h"
#include "dllmain.h"

namespace webpext
{
    ClassFactory::ClassFactory(): ref_count_{1}
    {
        IncreaseDllRefCount();
    }

    ClassFactory::~ClassFactory()
    {
        DecreaseDllRefCount();
    }

#pragma region IUnknown
    HRESULT ClassFactory::QueryInterface(const IID& riid, void** ppvObject)
    {
        static const QITAB qit[]{QITABENT(ClassFactory, IClassFactory), {0}};
        return QISearch(this, qit, riid, ppvObject);
    }

    ULONG ClassFactory::AddRef()
    {
        return InterlockedIncrement(&ref_count_);
    }

    ULONG ClassFactory::Release()
    {
        const ULONG ref = InterlockedDecrement(&ref_count_);
        if (ref == 0)
            delete this;
        return ref;
    }
#pragma endregion

#pragma region IClassFactory
    HRESULT ClassFactory::LockServer(BOOL fLock)
    {
        if (fLock)
            IncreaseDllRefCount();
        else
            DecreaseDllRefCount();
        return S_OK;
    }

    HRESULT ClassFactory::CreateInstance(IUnknown* pUnkOuter, const IID& riid, void** ppvObject)
    {
        HRESULT hr = CLASS_E_NOAGGREGATION;
        if (pUnkOuter == nullptr)
        {
            hr = E_OUTOFMEMORY;
            auto* thp = new (std::nothrow) ThumbnailProvider();
            if (thp != nullptr)
            {
                hr = thp->QueryInterface(riid, ppvObject);
                thp->Release();
            }
        }
        return hr;
    }
#pragma endregion
}
