#pragma once

#include <guiddef.h>

namespace webpext
{
    extern void IncreaseDllRefCount();

    extern void DecreaseDllRefCount();
}


#define WEBP_SHELL_CLSID "{C39D7E2A-FC3C-4DEE-BE80-380065FC2C2A}"
#define WEBP_PROJECT_NAME "Webp Thumbnail Provider"

static constexpr GUID CLSID_WebpThumbProvider =
    {0xc39d7e2a, 0xfc3c, 0x4dee, {0xbe, 0x80, 0x38, 0x0, 0x65, 0xfc, 0x2c, 0x2a}};
