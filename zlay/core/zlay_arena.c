#include <core/zlay_arena.h>

#include <string.h>

static size_t ZLay__CoreAlignUp(size_t value, size_t alignment) {
  if (alignment == 0u) {
    return value;
  }

  const size_t mask = alignment - 1u;
  return (value + mask) & ~mask;
}

ZLay_Arena ZLay_CreateArenaWithCapacityAndMemory(size_t capacity, void* memory) {
  ZLay_Arena arena;
  arena.memory = (uint8_t*)memory;
  arena.capacity = capacity;
  arena.offset = 0u;
  return arena;
}

void ZLay_ArenaReset(ZLay_Arena* arena) {
  if (arena == NULL) {
    return;
  }

  arena->offset = 0u;
}

void* ZLay_ArenaAlloc(ZLay_Arena* arena, size_t size, size_t alignment) {
  size_t aligned;
  void* result;

  if (arena == NULL || arena->memory == NULL) {
    return NULL;
  }

  aligned = ZLay__CoreAlignUp(arena->offset, alignment == 0u ? (size_t)8u : alignment);
  if (aligned > arena->capacity || size > arena->capacity - aligned) {
    return NULL;
  }

  result = arena->memory + aligned;
  arena->offset = aligned + size;
  return result;
}

ZLay_ArenaMarker ZLay_ArenaMark(const ZLay_Arena* arena) {
  ZLay_ArenaMarker marker;
  marker.offset = arena != NULL ? arena->offset : 0u;
  return marker;
}

void ZLay_ArenaRestore(ZLay_Arena* arena, ZLay_ArenaMarker marker) {
  if (arena == NULL || marker.offset > arena->capacity) {
    return;
  }

  arena->offset = marker.offset;
}

size_t ZLay_ArenaRemaining(const ZLay_Arena* arena) {
  if (arena == NULL || arena->offset >= arena->capacity) {
    return 0u;
  }

  return arena->capacity - arena->offset;
}

bool ZLay_ArenaCanAlloc(const ZLay_Arena* arena, size_t size, size_t alignment) {
  size_t aligned;

  if (arena == NULL || arena->memory == NULL) {
    return false;
  }

  aligned = ZLay__CoreAlignUp(arena->offset, alignment == 0u ? (size_t)8u : alignment);
  return aligned <= arena->capacity && size <= (arena->capacity - aligned);
}

bool ZLay_ArenaContains(const ZLay_Arena* arena, const void* ptr) {
  const uint8_t* bytes;

  if (arena == NULL || arena->memory == NULL || ptr == NULL) {
    return false;
  }

  bytes = (const uint8_t*)ptr;
  return bytes >= arena->memory && bytes < (arena->memory + arena->capacity);
}

void* ZLay_ArenaPushZero(ZLay_Arena* arena, size_t size, size_t alignment) {
  void* result = ZLay_ArenaAlloc(arena, size, alignment);
  if (result != NULL) {
    memset(result, 0, size);
  }

  return result;
}
