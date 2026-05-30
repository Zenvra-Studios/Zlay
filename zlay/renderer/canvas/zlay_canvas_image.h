#ifndef ZLAY_RENDERER_CANVAS_ZLAY_CANVAS_IMAGE_H
#define ZLAY_RENDERER_CANVAS_ZLAY_CANVAS_IMAGE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

#include <zlay.h>

typedef struct ZLay_CanvasImageOptions {
  ZLay_Rect source_rect;
  ZLay_Color tint;
} ZLay_CanvasImageOptions;

ZLAY_API ZLay_ImageHandle ZLay_ImageHandleMake(uint64_t id);
ZLAY_API bool ZLay_ImageHandleIsValid(ZLay_ImageHandle image);
ZLAY_API ZLay_CanvasImageOptions ZLay_CanvasImageOptionsDefault(void);

#ifdef __cplusplus
}
#endif

#endif
