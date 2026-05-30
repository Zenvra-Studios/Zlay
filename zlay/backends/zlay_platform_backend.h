#ifndef ZLAY_BACKENDS_ZLAY_PLATFORM_BACKEND_H
#define ZLAY_BACKENDS_ZLAY_PLATFORM_BACKEND_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

typedef struct ZLay_Context ZLay_Context;
typedef struct ZLay_PlatformBackend ZLay_PlatformBackend;

typedef struct ZLay_PlatformEvent {
  int type;
  float mouse_x;
  float mouse_y;
  float scroll_x;
  float scroll_y;
  int key;
  int down;
} ZLay_PlatformEvent;

typedef struct ZLay_PlatformBackendVTable {
  bool (*init)(ZLay_PlatformBackend* backend, ZLay_Context* ctx);
  void (*shutdown)(ZLay_PlatformBackend* backend, ZLay_Context* ctx);
  bool (*poll_event)(ZLay_PlatformBackend* backend, ZLay_Context* ctx, ZLay_PlatformEvent* out_event);
  void (*get_window_size)(ZLay_PlatformBackend* backend, int* out_width, int* out_height);
  float (*get_dpi_scale)(ZLay_PlatformBackend* backend);
} ZLay_PlatformBackendVTable;

struct ZLay_PlatformBackend {
  const char* name;
  void* userdata;
  const ZLay_PlatformBackendVTable* vtable;
};

static inline ZLay_PlatformBackend ZLay_PlatformBackendMake(
  const char* name,
  void* userdata,
  const ZLay_PlatformBackendVTable* vtable
) {
  ZLay_PlatformBackend backend;
  backend.name = name;
  backend.userdata = userdata;
  backend.vtable = vtable;
  return backend;
}

static inline bool ZLay_PlatformBackendIsValid(const ZLay_PlatformBackend* backend) {
  return backend != NULL && backend->vtable != NULL;
}

static inline bool ZLay_PlatformBackendInit(ZLay_PlatformBackend* backend, ZLay_Context* ctx) {
  if (!ZLay_PlatformBackendIsValid(backend) || backend->vtable->init == NULL) {
    return false;
  }
  return backend->vtable->init(backend, ctx);
}

static inline void ZLay_PlatformBackendShutdown(ZLay_PlatformBackend* backend, ZLay_Context* ctx) {
  if (ZLay_PlatformBackendIsValid(backend) && backend->vtable->shutdown != NULL) {
    backend->vtable->shutdown(backend, ctx);
  }
}

static inline bool ZLay_PlatformBackendPollEvent(
  ZLay_PlatformBackend* backend,
  ZLay_Context* ctx,
  ZLay_PlatformEvent* out_event
) {
  if (!ZLay_PlatformBackendIsValid(backend) || backend->vtable->poll_event == NULL) {
    return false;
  }
  return backend->vtable->poll_event(backend, ctx, out_event);
}

static inline void ZLay_PlatformBackendGetWindowSize(
  ZLay_PlatformBackend* backend,
  int* out_width,
  int* out_height
) {
  if (ZLay_PlatformBackendIsValid(backend) && backend->vtable->get_window_size != NULL) {
    backend->vtable->get_window_size(backend, out_width, out_height);
    return;
  }
  if (out_width != NULL) {
    *out_width = 0;
  }
  if (out_height != NULL) {
    *out_height = 0;
  }
}

static inline float ZLay_PlatformBackendGetDpiScale(ZLay_PlatformBackend* backend) {
  if (ZLay_PlatformBackendIsValid(backend) && backend->vtable->get_dpi_scale != NULL) {
    return backend->vtable->get_dpi_scale(backend);
  }
  return 1.0f;
}

#ifdef __cplusplus
}
#endif

#endif
