#pragma once

#include <inttypes.h>
#include <thumbcache.h>

namespace webpext
{
    class ThumbnailProvider final : public IThumbnailProvider, IInitializeWithStream
    {
    private:
        long ref_count_;
        IStream* stream_;
        ~ThumbnailProvider();
    public:
        ThumbnailProvider();
#pragma region IUnknown
        HRESULT STDMETHODCALLTYPE QueryInterface(const IID& riid, void** ppvObject) override;
        ULONG STDMETHODCALLTYPE AddRef() override;
        ULONG STDMETHODCALLTYPE Release() override;
#pragma endregion
#pragma region IThumbnailProvider
        HRESULT STDMETHODCALLTYPE GetThumbnail(UINT cx, HBITMAP* phbmp, WTS_ALPHATYPE* pdwAlpha) override;
        void CalcScaledBmpSize(const INT webp_width, const INT webp_height, const INT cx,
                               INT* scaled_width,
                               INT* scaled_height) const;
#pragma endregion
#pragma region IInitializeWithStream
        HRESULT Initialize(IStream* pstream, DWORD grfMode) override;
#pragma endregion
    };
}
