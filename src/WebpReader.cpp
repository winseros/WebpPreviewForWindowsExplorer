#include "WebpReader.h"
#include <winerror.h>
#include <webp/decode.h>

namespace webpext
{
    HRESULT WebpReader::ReadWebpHeader(TBYTE* webp_data, LONG webp_data_size, INT* webp_width, INT* webp_height,
                                       BOOLEAN* webp_alpha) const
    {
        WebPBitstreamFeatures features;
        if (WebPGetFeatures(webp_data, webp_data_size, &features) != VP8_STATUS_OK)
            return E_FAIL;

        *webp_width = features.width;
        *webp_height = features.height;
        *webp_alpha = features.has_alpha == TRUE;

        return S_OK;
    }

    HRESULT WebpReader::ReadAsBitmap(WebpReadInfo* webp_read_info) const
    {
        INT webp_width, webp_height;
        WebPGetInfo(webp_read_info->webp_data, webp_read_info->webp_data_size, &webp_width, &webp_height);

        WebPDecoderConfig config;
        if (!WebPInitDecoderConfig(&config))
            return E_FAIL;

        config.output.colorspace = MODE_BGRA;
        if (webp_width > webp_read_info->scaled_width || webp_height > webp_read_info->scaled_height)
        {
            config.options.use_scaling = 1;
            config.options.scaled_width = webp_read_info->scaled_width;
            config.options.scaled_height = webp_read_info->scaled_height;
        }

        if (WebPDecode(webp_read_info->webp_data, webp_read_info->webp_data_size, &config) != VP8_STATUS_OK)
        {
            WebPFreeDecBuffer(&config.output);
            return E_FAIL;
        }

        memcpy(webp_read_info->bmp_data, config.output.u.RGBA.rgba, config.output.u.RGBA.size);
        WebPFreeDecBuffer(&config.output);

        return S_OK;
    }
}
