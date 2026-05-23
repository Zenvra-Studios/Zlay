#include <core/zlay_memory.h>

#include <stdlib.h>
#include <string.h>

#if defined(_MSC_VER)
#  include <malloc.h>
#endif

static size_t ZLay__MemoryNormalizeAlignment(size_t alignment) {
  size_t normalized = sizeof(void*);

  if (alignment > normalized) {
    normalized = alignment;
  }

  if ((normalized & (normalized - 1u)) != 0u) {
    size_t power = sizeof(void*);
    while (power < normalized) {
      power <<= 1u;
    }
    normalized = power;
  }

  return normalized;
}

static size_t ZLay__MemoryRoundUp(size_t value, size_t alignment) {
  const size_t mask = alignment - 1u;
  return (value + mask) & ~mask;
}

static void ZLay__DefaultFree(void* ptr, size_t size, size_t alignment, void* user);

static void* ZLay__DefaultAlloc(size_t size, size_t alignment, void* user) {
  size_t normalized;
  (void)user;

  if (size == 0u) {
    return NULL;
  }

  normalized = ZLay__MemoryNormalizeAlignment(alignment);
#if defined(_MSC_VER)
  return _aligned_malloc(size, normalized);
#elif defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L && !defined(__APPLE__)
  return aligned_alloc(normalized, ZLay__MemoryRoundUp(size, normalized));
#else
  (void)normalized;
  return malloc(size);
#endif
}

static void* ZLay__DefaultRealloc(
  void* ptr,
  size_t old_size,
  size_t new_size,
  size_t alignment,
  void* user
) {
  void* next;

  if (ptr == NULL) {
    return ZLay__DefaultAlloc(new_size, alignment, user);
  }

  if (new_size == 0u) {
    ZLay__DefaultFree(ptr, old_size, alignment, user);
    return NULL;
  }

  next = ZLay__DefaultAlloc(new_size, alignment, user);
  if (next == NULL) {
    return NULL;
  }

  memcpy(next, ptr, old_size < new_size ? old_size : new_size);
  ZLay__DefaultFree(ptr, old_size, alignment, user);
  return next;
}

static void ZLay__DefaultFree(void* ptr, size_t size, size_t alignment, void* user) {
  (void)size;
  (void)alignment;
  (void)user;

  if (ptr == NULL) {
    return;
  }

#if defined(_MSC_VER)
  _aligned_free(ptr);
#else
  free(ptr);
#endif
}

ZLay_MemoryCallbacks ZLay_MemoryCallbacksDefault(void) {
  ZLay_MemoryCallbacks callbacks;
  callbacks.alloc = ZLay__DefaultAlloc;
  callbacks.realloc = ZLay__DefaultRealloc;
  callbacks.free = ZLay__DefaultFree;
  callbacks.user = NULL;
  return callbacks;
}

void* ZLay_MemoryAlloc(const ZLay_MemoryCallbacks* callbacks, size_t size, size_t alignment) {
  ZLay_MemoryCallbacks fallback;

  if (callbacks == NULL || callbacks->alloc == NULL) {
    fallback = ZLay_MemoryCallbacksDefault();
    callbacks = &fallback;
  }

  return callbacks->alloc(size, alignment, callbacks->user);
}

void* ZLay_MemoryRealloc(
  const ZLay_MemoryCallbacks* callbacks,
  void* ptr,
  size_t old_size,
  size_t new_size,
  size_t alignment
) {
  ZLay_MemoryCallbacks fallback;

  if (callbacks == NULL || callbacks->realloc == NULL) {
    fallback = ZLay_MemoryCallbacksDefault();
    callbacks = &fallback;
  }

  return callbacks->realloc(ptr, old_size, new_size, alignment, callbacks->user);
}

void ZLay_MemoryFree(const ZLay_MemoryCallbacks* callbacks, void* ptr, size_t size, size_t alignment) {
  ZLay_MemoryCallbacks fallback;

  if (callbacks == NULL || callbacks->free == NULL) {
    fallback = ZLay_MemoryCallbacksDefault();
    callbacks = &fallback;
  }

  callbacks->free(ptr, size, alignment, callbacks->user);
}
