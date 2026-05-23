#ifndef ZLAY_DRIVER_ZLAY_GPU_H
#define ZLAY_DRIVER_ZLAY_GPU_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <zlay.h>

typedef enum ZLay_DirtyFlags {
  ZLAY_DIRTY_NONE = 0,
  ZLAY_DIRTY_STYLE = 1 << 0,
  ZLAY_DIRTY_LAYOUT = 1 << 1,
  ZLAY_DIRTY_TEXT = 1 << 2,
  ZLAY_DIRTY_PAINT = 1 << 3,
  ZLAY_DIRTY_SURFACE = 1 << 4,
  ZLAY_DIRTY_GPU_RESOURCES = 1 << 5,
  ZLAY_DIRTY_ALL = -1
} ZLay_DirtyFlags;

typedef struct ZLay_DirtyState {
  uint32_t flags;
  uint64_t revision;
} ZLay_DirtyState;

typedef struct ZLay_GpuRingBuffer {
  uint8_t* memory;
  size_t capacity;
  size_t offset;
  size_t last_alloc_size;
} ZLay_GpuRingBuffer;

typedef struct ZLay_GpuStats {
  uint32_t draw_calls;
  uint32_t rect_count;
  uint32_t text_count;
  uint32_t clip_count;
  uint32_t vertex_count;
  uint32_t index_count;
  bool overflowed;
} ZLay_GpuStats;

ZLAY_API void ZLay_DirtyStateInit(ZLay_DirtyState* state);
ZLAY_API void ZLay_DirtyStateMark(ZLay_DirtyState* state, uint32_t flags);
ZLAY_API void ZLay_DirtyStateClear(ZLay_DirtyState* state, uint32_t flags);
ZLAY_API void ZLay_DirtyStateClearAll(ZLay_DirtyState* state);
ZLAY_API bool ZLay_DirtyStateHas(const ZLay_DirtyState* state, uint32_t flags);

ZLAY_API void ZLay_GpuRingBufferInit(ZLay_GpuRingBuffer* buffer, void* memory, size_t capacity);
ZLAY_API void ZLay_GpuRingBufferReset(ZLay_GpuRingBuffer* buffer);
ZLAY_API void* ZLay_GpuRingBufferAlloc(ZLay_GpuRingBuffer* buffer, size_t size, size_t alignment);
ZLAY_API size_t ZLay_GpuRingBufferRemaining(const ZLay_GpuRingBuffer* buffer);

#ifdef __cplusplus
}
#endif

#endif
