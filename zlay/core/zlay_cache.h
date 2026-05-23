#pragma once

#include <zlay.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ZLay_CacheEntry {
  ZLay_Id key;
  void* value;
  uint64_t generation;
  bool active;
} ZLay_CacheEntry;

typedef struct ZLay_Cache {
  ZLay_CacheEntry* entries;
  uint32_t capacity;
  uint32_t count;
  uint64_t generation;
} ZLay_Cache;

ZLAY_API void ZLay_CacheInit(ZLay_Cache* cache, ZLay_CacheEntry* entries, uint32_t capacity);
ZLAY_API void ZLay_CacheClear(ZLay_Cache* cache);
ZLAY_API bool ZLay_CachePut(ZLay_Cache* cache, ZLay_Id key, void* value);
ZLAY_API void* ZLay_CacheGet(ZLay_Cache* cache, ZLay_Id key);
ZLAY_API bool ZLay_CacheRemove(ZLay_Cache* cache, ZLay_Id key);
ZLAY_API void ZLay_CacheBumpGeneration(ZLay_Cache* cache);
ZLAY_API uint32_t ZLay_CacheSweepOlderThan(ZLay_Cache* cache, uint64_t min_generation);

#ifdef __cplusplus
}
#endif
