#include <core/zlay_cache.h>

#include <string.h>

static int32_t ZLay__CacheFindSlot(const ZLay_Cache* cache, ZLay_Id key) {
  if (cache == NULL || cache->entries == NULL || key.hash == 0u) {
    return -1;
  }

  for (uint32_t i = 0u; i < cache->capacity; ++i) {
    if (cache->entries[i].active && cache->entries[i].key.hash == key.hash) {
      return (int32_t)i;
    }
  }

  return -1;
}

static int32_t ZLay__CacheFindFreeSlot(const ZLay_Cache* cache) {
  if (cache == NULL || cache->entries == NULL) {
    return -1;
  }

  for (uint32_t i = 0u; i < cache->capacity; ++i) {
    if (!cache->entries[i].active) {
      return (int32_t)i;
    }
  }

  return -1;
}

void ZLay_CacheInit(ZLay_Cache* cache, ZLay_CacheEntry* entries, uint32_t capacity) {
  if (cache == NULL) {
    return;
  }

  cache->entries = entries;
  cache->capacity = capacity;
  cache->count = 0u;
  cache->generation = 1u;

  if (entries != NULL && capacity > 0u) {
    memset(entries, 0, sizeof(entries[0]) * (size_t)capacity);
  }
}

void ZLay_CacheClear(ZLay_Cache* cache) {
  if (cache == NULL || cache->entries == NULL) {
    return;
  }

  memset(cache->entries, 0, sizeof(cache->entries[0]) * (size_t)cache->capacity);
  cache->count = 0u;
  ++cache->generation;
}

bool ZLay_CachePut(ZLay_Cache* cache, ZLay_Id key, void* value) {
  int32_t slot;

  if (cache == NULL || cache->entries == NULL || key.hash == 0u) {
    return false;
  }

  slot = ZLay__CacheFindSlot(cache, key);
  if (slot < 0) {
    slot = ZLay__CacheFindFreeSlot(cache);
    if (slot < 0) {
      return false;
    }
    ++cache->count;
  }

  cache->entries[slot].key = key;
  cache->entries[slot].value = value;
  cache->entries[slot].generation = cache->generation;
  cache->entries[slot].active = true;
  return true;
}

void* ZLay_CacheGet(ZLay_Cache* cache, ZLay_Id key) {
  int32_t slot = ZLay__CacheFindSlot(cache, key);
  if (slot < 0) {
    return NULL;
  }

  cache->entries[slot].generation = cache->generation;
  return cache->entries[slot].value;
}

bool ZLay_CacheRemove(ZLay_Cache* cache, ZLay_Id key) {
  int32_t slot = ZLay__CacheFindSlot(cache, key);
  if (slot < 0) {
    return false;
  }

  cache->entries[slot] = (ZLay_CacheEntry){0};
  if (cache->count > 0u) {
    --cache->count;
  }
  return true;
}

void ZLay_CacheBumpGeneration(ZLay_Cache* cache) {
  if (cache != NULL) {
    ++cache->generation;
  }
}

uint32_t ZLay_CacheSweepOlderThan(ZLay_Cache* cache, uint64_t min_generation) {
  uint32_t removed = 0u;

  if (cache == NULL || cache->entries == NULL) {
    return 0u;
  }

  for (uint32_t i = 0u; i < cache->capacity; ++i) {
    if (cache->entries[i].active && cache->entries[i].generation < min_generation) {
      cache->entries[i] = (ZLay_CacheEntry){0};
      ++removed;
    }
  }

  cache->count = cache->count > removed ? cache->count - removed : 0u;
  return removed;
}
