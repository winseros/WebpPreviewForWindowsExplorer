#pragma once

#include <ShlObj.h>

namespace webpext
{
    struct WebpReadInfo
    {
        const TBYTE* webp_data;
        const ULONG webp_data_size;
        const INT scaled_width;
        const INT scaled_height;
        TBYTE* bmp_data;
    };

    class WebpReader
    {
    public:
        HRESULT ReadWebpHeader(TBYTE* webp_data, LONG webp_data_size, INT* webp_width, INT* webp_height, BOOLEAN* webp_alpha) const;

        HRESULT ReadAsBitmap(WebpReadInfo* webp_read_info) const;
    };
}
