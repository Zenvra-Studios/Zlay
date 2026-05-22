#include "zlay_internal.h"

static size_t ZLay__AlignUp(size_t value, size_t alignment) {
  if (alignment == 0) return value;
  const size_t mask = alignment - 1;
  return (value + mask) & ~mask;
}

ZLay_Arena ZLay_CreateArenaWithCapacityAndMemory(size_t capacity, void* memory) {
  ZLay_Arena a;
  a.memory = (uint8_t*)memory;
  a.capacity = capacity;
  a.offset = 0;
  return a;
}

void ZLay_ArenaReset(ZLay_Arena* arena) {
  if (!arena) return;
  arena->offset = 0;
}

void* ZLay_ArenaAlloc(ZLay_Arena* arena, size_t size, size_t alignment) {
  if (!arena || !arena->memory) return NULL;
  const size_t aligned = ZLay__AlignUp(arena->offset, alignment == 0 ? (size_t)8 : alignment);
  if (aligned + size > arena->capacity) return NULL;
  void* result = arena->memory + aligned;
  arena->offset = aligned + size;
  return result;
}

