#include "ThumbnailProvider.h"
#include <Shlwapi.h>
#include <memory>
#include <wingdi.h>
#include "WebpReader.h"

namespace webpext
{
    using namespace std;

    ThumbnailProvider::ThumbnailProvider() :
        ref_count_{1},
        stream_{nullptr}
    {
    }

    ThumbnailProvider::~ThumbnailProvider()
    {
        if (stream_)
            stream_->Release();
    }

#pragma region IUnknown

    HRESULT ThumbnailProvider::QueryInterface(const IID& riid, void** ppvObject)
    {
        static const QITAB qit[]{
            QITABENT(ThumbnailProvider, IInitializeWithStream),
            QITABENT(ThumbnailProvider, IThumbnailProvider),
            {0}
        };
        return QISearch(this, qit, riid, ppvObject);
    }

    ULONG ThumbnailProvider::AddRef()
    {
        return InterlockedIncrement(&ref_count_);
    }

    ULONG ThumbnailProvider::Release()
    {
        const long ref = InterlockedDecrement(&ref_count_);
        if (ref == 0)
            delete this;
        return ref;
    }

#pragma endregion

#pragma region IThumbnailProvider

    HRESULT ThumbnailProvider::GetThumbnail(UINT cx, HBITMAP* phbmp, WTS_ALPHATYPE* pdwAlpha)
    {
        STATSTG stat;
        HRESULT hr = stream_->Stat(&stat, STATFLAG::STATFLAG_NONAME);
        if (SUCCEEDED(hr))
        {
            const unique_ptr<uint8_t[]> data = make_unique<uint8_t[]>(stat.cbSize.QuadPart);
            ULONG bytes_read;
            hr = stream_->Read(data.get(), static_cast<ULONG>(stat.cbSize.QuadPart), &bytes_read);
            if (SUCCEEDED(hr))
            {
                const WebpReader reader;
                INT webp_width, webp_height;
                BOOLEAN webp_alpha;
                hr = reader.ReadWebpHeader(data.get(), bytes_read, &webp_width, &webp_height, &webp_alpha);

                INT scaled_width, scaled_height;
                CalcScaledBmpSize(webp_width, webp_height, cx, &scaled_width, &scaled_height);

                if (SUCCEEDED(hr))
                {
                    BITMAPINFO bmi;
                    bmi.bmiHeader.biSize = sizeof(bmi.bmiHeader);
                    bmi.bmiHeader.biHeight = -scaled_height;
                    bmi.bmiHeader.biWidth = scaled_width;
                    bmi.bmiHeader.biPlanes = 1;
                    bmi.bmiHeader.biBitCount = 32;
                    bmi.bmiHeader.biCompression = BI_RGB;
                    *pdwAlpha = webp_alpha ? WTSAT_ARGB : WTSAT_RGB;

                    TBYTE* bytes;
                    HBITMAP bmp = CreateDIBSection(nullptr, &bmi, DIB_RGB_COLORS,
                                                         reinterpret_cast<void**>(&bytes),
                                                         nullptr, 0);

                    hr = bmp ? S_OK : E_OUTOFMEMORY;
                    if (SUCCEEDED(hr))
                    {
                        WebpReadInfo ri
                        {
                            data.get(),
                            bytes_read,
                            scaled_width,
                            scaled_height,
                            bytes
                        };

                        hr = reader.ReadAsBitmap(&ri);
                        if (SUCCEEDED(hr))
                        {
                            *phbmp = bmp;
                        }
                    }
                }
            }
        }

        return hr;
    }

    void ThumbnailProvider::CalcScaledBmpSize(const INT webp_width, const INT webp_height,
                                              const INT cx, INT* scaled_width, INT* scaled_height) const
    {
        if (webp_width > cx || webp_height > cx)
        {
            if (webp_width > webp_height)
            {
                const double ratio = 1.0 * cx / webp_width;
                *scaled_width = cx;
                *scaled_height = static_cast<uint32_t>(round(ratio * webp_height));
            }
            else if (webp_height > webp_width)
            {
                const double ratio = 1.0 * cx / webp_height;
                *scaled_width = static_cast<uint32_t>(round(ratio * webp_width));
                *scaled_height = cx;
            }
            else
            {
                *scaled_width = cx;
                *scaled_height = cx;
            }
        }
        else
        {
            *scaled_width = webp_width;
            *scaled_height = webp_height;
        }
    }

#pragma endregion

#pragma region IInitializeWithStream

    HRESULT ThumbnailProvider::Initialize(IStream* pstream, DWORD grfMode)
    {
        HRESULT hr = E_UNEXPECTED;
        if (!stream_)
            hr = pstream->QueryInterface(&stream_);
        return hr;
    }

#pragma endregion
}
