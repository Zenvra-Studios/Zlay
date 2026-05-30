#ifndef ZLAY_BACKENDS_ZLAY_HOST_BACKEND_H
#define ZLAY_BACKENDS_ZLAY_HOST_BACKEND_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

typedef struct ZLay_Context ZLay_Context;
typedef struct ZLay_HostBackend ZLay_HostBackend;

typedef struct ZLay_HostBackendVTable {
  bool (*init)(ZLay_HostBackend* backend, ZLay_Context* ctx);
  void (*shutdown)(ZLay_HostBackend* backend, ZLay_Context* ctx);
  void (*begin_frame)(ZLay_HostBackend* backend, ZLay_Context* ctx);
  void (*end_frame)(ZLay_HostBackend* backend, ZLay_Context* ctx);
} ZLay_HostBackendVTable;

struct ZLay_HostBackend {
  const char* name;
  void* userdata;
  const ZLay_HostBackendVTable* vtable;
};

static inline ZLay_HostBackend ZLay_HostBackendMake(
  const char* name,
  void* userdata,
  const ZLay_HostBackendVTable* vtable
) {
  ZLay_HostBackend backend;
  backend.name = name;
  backend.userdata = userdata;
  backend.vtable = vtable;
  return backend;
}

static inline bool ZLay_HostBackendIsValid(const ZLay_HostBackend* backend) {
  return backend != NULL && backend->vtable != NULL;
}

static inline bool ZLay_HostBackendInit(ZLay_HostBackend* backend, ZLay_Context* ctx) {
  if (!ZLay_HostBackendIsValid(backend) || backend->vtable->init == NULL) {
    return false;
  }
  return backend->vtable->init(backend, ctx);
}

static inline void ZLay_HostBackendShutdown(ZLay_HostBackend* backend, ZLay_Context* ctx) {
  if (ZLay_HostBackendIsValid(backend) && backend->vtable->shutdown != NULL) {
    backend->vtable->shutdown(backend, ctx);
  }
}

static inline void ZLay_HostBackendBeginFrame(ZLay_HostBackend* backend, ZLay_Context* ctx) {
  if (ZLay_HostBackendIsValid(backend) && backend->vtable->begin_frame != NULL) {
    backend->vtable->begin_frame(backend, ctx);
  }
}

static inline void ZLay_HostBackendEndFrame(ZLay_HostBackend* backend, ZLay_Context* ctx) {
  if (ZLay_HostBackendIsValid(backend) && backend->vtable->end_frame != NULL) {
    backend->vtable->end_frame(backend, ctx);
  }
}

#ifdef __cplusplus
}
#endif

#endif
