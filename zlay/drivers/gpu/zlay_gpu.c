#include <drivers/gpu/zlay_gpu.h>

#include <string.h>

static size_t ZLay__DriverAlignUp(size_t value, size_t alignment) {
  const size_t mask = alignment > 0u ? alignment - 1u : 0u;
  return mask != 0u ? (value + mask) & ~mask : value;
}

void ZLay_DirtyStateInit(ZLay_DirtyState* state) {
  if (state == NULL) {
    return;
  }

  state->flags = ZLAY_DIRTY_ALL;
  state->revision = 1u;
}

void ZLay_DirtyStateMark(ZLay_DirtyState* state, uint32_t flags) {
  if (state == NULL || flags == ZLAY_DIRTY_NONE) {
    return;
  }

  state->flags |= flags;
  ++state->revision;
}

void ZLay_DirtyStateClear(ZLay_DirtyState* state, uint32_t flags) {
  if (state == NULL) {
    return;
  }

  state->flags &= ~flags;
}

void ZLay_DirtyStateClearAll(ZLay_DirtyState* state) {
  if (state != NULL) {
    state->flags = ZLAY_DIRTY_NONE;
  }
}

bool ZLay_DirtyStateHas(const ZLay_DirtyState* state, uint32_t flags) {
  return state != NULL && (state->flags & flags) != 0u;
}

void ZLay_GpuRingBufferInit(ZLay_GpuRingBuffer* buffer, void* memory, size_t capacity) {
  if (buffer == NULL) {
    return;
  }

  buffer->memory = (uint8_t*)memory;
  buffer->capacity = capacity;
  buffer->offset = 0u;
  buffer->last_alloc_size = 0u;
}

void ZLay_GpuRingBufferReset(ZLay_GpuRingBuffer* buffer) {
  if (buffer == NULL) {
    return;
  }

  buffer->offset = 0u;
  buffer->last_alloc_size = 0u;
}

void* ZLay_GpuRingBufferAlloc(ZLay_GpuRingBuffer* buffer, size_t size, size_t alignment) {
  size_t aligned;
  void* result;

  if (buffer == NULL || buffer->memory == NULL || size == 0u) {
    return NULL;
  }

  aligned = ZLay__DriverAlignUp(buffer->offset, alignment == 0u ? (size_t)8u : alignment);
  if (aligned > buffer->capacity || size > buffer->capacity - aligned) {
    return NULL;
  }

  result = buffer->memory + aligned;
  buffer->offset = aligned + size;
  buffer->last_alloc_size = size;
  return result;
}

size_t ZLay_GpuRingBufferRemaining(const ZLay_GpuRingBuffer* buffer) {
  if (buffer == NULL || buffer->offset >= buffer->capacity) {
    return 0u;
  }

  return buffer->capacity - buffer->offset;
}
