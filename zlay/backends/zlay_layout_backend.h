#ifndef ZLAY_BACKENDS_ZLAY_LAYOUT_BACKEND_H
#define ZLAY_BACKENDS_ZLAY_LAYOUT_BACKEND_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

#include <zlay.h>

typedef struct ZLay_Context ZLay_Context;
typedef struct ZLay_LayoutBackend ZLay_LayoutBackend;

typedef struct ZLay_LayoutBackendVTable {
  bool (*init)(ZLay_LayoutBackend* backend, ZLay_Context* ctx);
  void (*shutdown)(ZLay_LayoutBackend* backend, ZLay_Context* ctx);
  void (*begin_frame)(ZLay_LayoutBackend* backend, ZLay_Context* ctx, float width, float height, float delta_time);
  ZLay_RenderCommandList (*end_frame)(ZLay_LayoutBackend* backend, ZLay_Context* ctx);
  void (*set_pointer)(ZLay_LayoutBackend* backend, ZLay_Context* ctx, float x, float y, bool down);
  void (*set_scroll)(ZLay_LayoutBackend* backend, ZLay_Context* ctx, float x, float y);
} ZLay_LayoutBackendVTable;

struct ZLay_LayoutBackend {
  const char* name;
  void* userdata;
  const ZLay_LayoutBackendVTable* vtable;
};

static inline ZLay_LayoutBackend ZLay_LayoutBackendMake(
  const char* name,
  void* userdata,
  const ZLay_LayoutBackendVTable* vtable
) {
  ZLay_LayoutBackend backend;
  backend.name = name;
  backend.userdata = userdata;
  backend.vtable = vtable;
  return backend;
}

static inline bool ZLay_LayoutBackendIsValid(const ZLay_LayoutBackend* backend) {
  return backend != NULL && backend->vtable != NULL;
}

static inline bool ZLay_LayoutBackendInit(ZLay_LayoutBackend* backend, ZLay_Context* ctx) {
  if (!ZLay_LayoutBackendIsValid(backend) || backend->vtable->init == NULL) {
    return false;
  }
  return backend->vtable->init(backend, ctx);
}

static inline void ZLay_LayoutBackendShutdown(ZLay_LayoutBackend* backend, ZLay_Context* ctx) {
  if (ZLay_LayoutBackendIsValid(backend) && backend->vtable->shutdown != NULL) {
    backend->vtable->shutdown(backend, ctx);
  }
}

static inline void ZLay_LayoutBackendBeginFrame(
  ZLay_LayoutBackend* backend,
  ZLay_Context* ctx,
  float width,
  float height,
  float delta_time
) {
  if (ZLay_LayoutBackendIsValid(backend) && backend->vtable->begin_frame != NULL) {
    backend->vtable->begin_frame(backend, ctx, width, height, delta_time);
  }
}

static inline ZLay_RenderCommandList ZLay_LayoutBackendEndFrame(ZLay_LayoutBackend* backend, ZLay_Context* ctx) {
  if (ZLay_LayoutBackendIsValid(backend) && backend->vtable->end_frame != NULL) {
    return backend->vtable->end_frame(backend, ctx);
  }
  return (ZLay_RenderCommandList){0};
}

static inline void ZLay_LayoutBackendSetPointer(
  ZLay_LayoutBackend* backend,
  ZLay_Context* ctx,
  float x,
  float y,
  bool down
) {
  if (ZLay_LayoutBackendIsValid(backend) && backend->vtable->set_pointer != NULL) {
    backend->vtable->set_pointer(backend, ctx, x, y, down);
  }
}

static inline void ZLay_LayoutBackendSetScroll(
  ZLay_LayoutBackend* backend,
  ZLay_Context* ctx,
  float x,
  float y
) {
  if (ZLay_LayoutBackendIsValid(backend) && backend->vtable->set_scroll != NULL) {
    backend->vtable->set_scroll(backend, ctx, x, y);
  }
}

#ifdef __cplusplus
}
#endif

#endif
