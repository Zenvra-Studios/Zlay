#ifndef ZLAY_OS_WINRT_ZLAY_WINRT_DPI_H
#define ZLAY_OS_WINRT_ZLAY_WINRT_DPI_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include <zlay.h>

ZLAY_API float ZLay_WinRT_DpiScaleForWindow(void* hwnd);
ZLAY_API uint32_t ZLay_WinRT_DpiScalePixels(uint32_t pixels, float scale);
ZLAY_API ZLay_Dimensions ZLay_WinRT_DpiScaleDimensions(ZLay_Dimensions dimensions, float scale);

#ifdef __cplusplus
}
#endif

#endif
