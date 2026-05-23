#ifndef ZLAY_DRIVER_ZLAY_PIPELINE_H
#define ZLAY_DRIVER_ZLAY_PIPELINE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

#include <zlay.h>
#include <driver/zlay_context.h>

typedef enum ZLay_PipelineKind {
  ZLAY_PIPELINE_NONE = 0,
  ZLAY_PIPELINE_RECT = 1,
  ZLAY_PIPELINE_TEXT = 2,
  ZLAY_PIPELINE_IMAGE = 3
} ZLay_PipelineKind;

typedef struct ZLay_PipelineState {
  ZLay_PipelineKind kind;
  const char* debug_name;
  uint64_t generation;
  bool valid;
} ZLay_PipelineState;

typedef struct ZLay_PipelineCache {
  ZLay_PipelineState rect;
  ZLay_PipelineState text;
  ZLay_PipelineState image;
  uint64_t generation;
  bool valid;
} ZLay_PipelineCache;

ZLAY_API void ZLay_PipelineCacheInit(ZLay_PipelineCache* cache, const ZLay_DriverCaps* caps);
ZLAY_API void ZLay_PipelineCacheInvalidate(ZLay_PipelineCache* cache);
ZLAY_API bool ZLay_PipelineCacheEnsure(ZLay_PipelineCache* cache, ZLay_PipelineKind kind);
ZLAY_API const ZLay_PipelineState* ZLay_PipelineCacheFind(const ZLay_PipelineCache* cache, ZLay_PipelineKind kind);
ZLAY_API const char* ZLay_PipelineKindName(ZLay_PipelineKind kind);

#ifdef __cplusplus
}
#endif

#endif
