#ifndef ZLAY_OS_ZLAY_DPI_H
#define ZLAY_OS_ZLAY_DPI_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include <zlay.h>

#define ZLAY_DPI_BASELINE 96.0f

ZLAY_API float ZLay_DpiScaleFromDpi(float dpi);
ZLAY_API float ZLay_DpiFromScale(float scale);
ZLAY_API uint32_t ZLay_DpiScalePixels(uint32_t pixels, float scale);

#ifdef __cplusplus
}
#endif

#endif
