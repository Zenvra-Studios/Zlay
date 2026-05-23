#pragma once

#include <zlay.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void* (*ZLay_MemoryAllocFn)(size_t size, size_t alignment, void* user);
typedef void* (*ZLay_MemoryReallocFn)(void* ptr, size_t old_size, size_t new_size, size_t alignment, void* user);
typedef void (*ZLay_MemoryFreeFn)(void* ptr, size_t size, size_t alignment, void* user);

typedef struct ZLay_MemoryCallbacks {
  ZLay_MemoryAllocFn alloc;
  ZLay_MemoryReallocFn realloc;
  ZLay_MemoryFreeFn free;
  void* user;
} ZLay_MemoryCallbacks;

ZLAY_API ZLay_MemoryCallbacks ZLay_MemoryCallbacksDefault(void);
ZLAY_API void* ZLay_MemoryAlloc(const ZLay_MemoryCallbacks* callbacks, size_t size, size_t alignment);
ZLAY_API void* ZLay_MemoryRealloc(
  const ZLay_MemoryCallbacks* callbacks,
  void* ptr,
  size_t old_size,
  size_t new_size,
  size_t alignment
);
ZLAY_API void ZLay_MemoryFree(const ZLay_MemoryCallbacks* callbacks, void* ptr, size_t size, size_t alignment);

#ifdef __cplusplus
}
#endif
