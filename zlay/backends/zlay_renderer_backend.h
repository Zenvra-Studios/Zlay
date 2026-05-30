#ifndef ZLAY_BACKENDS_ZLAY_RENDERER_BACKEND_H
#define ZLAY_BACKENDS_ZLAY_RENDERER_BACKEND_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

#include <zlay.h>

typedef struct ZLay_Context ZLay_Context;
typedef struct ZLay_RendererBackend ZLay_RendererBackend;

typedef struct ZLay_RendererBackendVTable {
  bool (*init)(ZLay_RendererBackend* backend, ZLay_Context* ctx);
  void (*shutdown)(ZLay_RendererBackend* backend, ZLay_Context* ctx);
  void (*begin_frame)(ZLay_RendererBackend* backend, ZLay_Context* ctx, int width, int height);
  void (*render)(ZLay_RendererBackend* backend, ZLay_Context* ctx, const ZLay_RenderCommandList* commands);
  void (*end_frame)(ZLay_RendererBackend* backend, ZLay_Context* ctx);
  void (*resize)(ZLay_RendererBackend* backend, ZLay_Context* ctx, int width, int height);
} ZLay_RendererBackendVTable;

struct ZLay_RendererBackend {
  const char* name;
  void* userdata;
  const ZLay_RendererBackendVTable* vtable;
};

static inline ZLay_RendererBackend ZLay_RendererBackendMake(
  const char* name,
  void* userdata,
  const ZLay_RendererBackendVTable* vtable
) {
  ZLay_RendererBackend backend;
  backend.name = name;
  backend.userdata = userdata;
  backend.vtable = vtable;
  return backend;
}

static inline bool ZLay_RendererBackendIsValid(const ZLay_RendererBackend* backend) {
  return backend != NULL && backend->vtable != NULL;
}

static inline bool ZLay_RendererBackendInit(ZLay_RendererBackend* backend, ZLay_Context* ctx) {
  if (!ZLay_RendererBackendIsValid(backend) || backend->vtable->init == NULL) {
    return false;
  }
  return backend->vtable->init(backend, ctx);
}

static inline void ZLay_RendererBackendShutdown(ZLay_RendererBackend* backend, ZLay_Context* ctx) {
  if (ZLay_RendererBackendIsValid(backend) && backend->vtable->shutdown != NULL) {
    backend->vtable->shutdown(backend, ctx);
  }
}

static inline void ZLay_RendererBackendBeginFrame(
  ZLay_RendererBackend* backend,
  ZLay_Context* ctx,
  int width,
  int height
) {
  if (ZLay_RendererBackendIsValid(backend) && backend->vtable->begin_frame != NULL) {
    backend->vtable->begin_frame(backend, ctx, width, height);
  }
}

static inline void ZLay_RendererBackendRender(
  ZLay_RendererBackend* backend,
  ZLay_Context* ctx,
  const ZLay_RenderCommandList* commands
) {
  if (ZLay_RendererBackendIsValid(backend) && backend->vtable->render != NULL) {
    backend->vtable->render(backend, ctx, commands);
  }
}

static inline void ZLay_RendererBackendEndFrame(ZLay_RendererBackend* backend, ZLay_Context* ctx) {
  if (ZLay_RendererBackendIsValid(backend) && backend->vtable->end_frame != NULL) {
    backend->vtable->end_frame(backend, ctx);
  }
}

static inline void ZLay_RendererBackendResize(
  ZLay_RendererBackend* backend,
  ZLay_Context* ctx,
  int width,
  int height
) {
  if (ZLay_RendererBackendIsValid(backend) && backend->vtable->resize != NULL) {
    backend->vtable->resize(backend, ctx, width, height);
  }
}

#ifdef __cplusplus
}
#endif

#endif
