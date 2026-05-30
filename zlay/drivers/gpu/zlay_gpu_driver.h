#ifndef ZLAY_DRIVERS_GPU_ZLAY_GPU_DRIVER_H
#define ZLAY_DRIVERS_GPU_ZLAY_GPU_DRIVER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct ZLay_GpuDriver ZLay_GpuDriver;

typedef enum ZLay_GpuBackendKind {
  ZLAY_GPU_BACKEND_NONE = 0,
  ZLAY_GPU_BACKEND_OPENGL,
  ZLAY_GPU_BACKEND_VULKAN,
  ZLAY_GPU_BACKEND_WEBGPU,
  ZLAY_GPU_BACKEND_METAL,
  ZLAY_GPU_BACKEND_D3D11,
  ZLAY_GPU_BACKEND_D3D12
} ZLay_GpuBackendKind;

typedef struct ZLay_GpuBufferHandle {
  uint64_t id;
} ZLay_GpuBufferHandle;

typedef struct ZLay_GpuTextureHandle {
  uint64_t id;
} ZLay_GpuTextureHandle;

typedef struct ZLay_GpuPipelineHandle {
  uint64_t id;
} ZLay_GpuPipelineHandle;

typedef struct ZLay_GpuDriverVTable {
  bool (*init)(ZLay_GpuDriver* driver);
  void (*shutdown)(ZLay_GpuDriver* driver);
  ZLay_GpuBufferHandle (*create_buffer)(ZLay_GpuDriver* driver, const void* data, size_t size);
  void (*destroy_buffer)(ZLay_GpuDriver* driver, ZLay_GpuBufferHandle buffer);
  ZLay_GpuTextureHandle (*create_texture)(ZLay_GpuDriver* driver, int width, int height, const void* pixels);
  void (*destroy_texture)(ZLay_GpuDriver* driver, ZLay_GpuTextureHandle texture);
  void (*set_scissor)(ZLay_GpuDriver* driver, int x, int y, int width, int height);
  void (*draw)(
    ZLay_GpuDriver* driver,
    ZLay_GpuBufferHandle vertex_buffer,
    ZLay_GpuBufferHandle index_buffer,
    uint32_t index_count
  );
} ZLay_GpuDriverVTable;

struct ZLay_GpuDriver {
  const char* name;
  ZLay_GpuBackendKind kind;
  void* userdata;
  const ZLay_GpuDriverVTable* vtable;
};

static inline ZLay_GpuDriver ZLay_GpuDriverMake(
  const char* name,
  ZLay_GpuBackendKind kind,
  void* userdata,
  const ZLay_GpuDriverVTable* vtable
) {
  ZLay_GpuDriver driver;
  driver.name = name;
  driver.kind = kind;
  driver.userdata = userdata;
  driver.vtable = vtable;
  return driver;
}

static inline bool ZLay_GpuDriverIsValid(const ZLay_GpuDriver* driver) {
  return driver != NULL && driver->vtable != NULL;
}

static inline bool ZLay_GpuDriverInit(ZLay_GpuDriver* driver) {
  if (!ZLay_GpuDriverIsValid(driver) || driver->vtable->init == NULL) {
    return false;
  }
  return driver->vtable->init(driver);
}

static inline void ZLay_GpuDriverShutdown(ZLay_GpuDriver* driver) {
  if (ZLay_GpuDriverIsValid(driver) && driver->vtable->shutdown != NULL) {
    driver->vtable->shutdown(driver);
  }
}

static inline ZLay_GpuBufferHandle ZLay_GpuDriverCreateBuffer(
  ZLay_GpuDriver* driver,
  const void* data,
  size_t size
) {
  if (!ZLay_GpuDriverIsValid(driver) || driver->vtable->create_buffer == NULL) {
    return (ZLay_GpuBufferHandle){0u};
  }
  return driver->vtable->create_buffer(driver, data, size);
}

static inline void ZLay_GpuDriverDestroyBuffer(ZLay_GpuDriver* driver, ZLay_GpuBufferHandle buffer) {
  if (ZLay_GpuDriverIsValid(driver) && driver->vtable->destroy_buffer != NULL) {
    driver->vtable->destroy_buffer(driver, buffer);
  }
}

static inline ZLay_GpuTextureHandle ZLay_GpuDriverCreateTexture(
  ZLay_GpuDriver* driver,
  int width,
  int height,
  const void* pixels
) {
  if (!ZLay_GpuDriverIsValid(driver) || driver->vtable->create_texture == NULL) {
    return (ZLay_GpuTextureHandle){0u};
  }
  return driver->vtable->create_texture(driver, width, height, pixels);
}

static inline void ZLay_GpuDriverDestroyTexture(ZLay_GpuDriver* driver, ZLay_GpuTextureHandle texture) {
  if (ZLay_GpuDriverIsValid(driver) && driver->vtable->destroy_texture != NULL) {
    driver->vtable->destroy_texture(driver, texture);
  }
}

static inline void ZLay_GpuDriverSetScissor(
  ZLay_GpuDriver* driver,
  int x,
  int y,
  int width,
  int height
) {
  if (ZLay_GpuDriverIsValid(driver) && driver->vtable->set_scissor != NULL) {
    driver->vtable->set_scissor(driver, x, y, width, height);
  }
}

static inline void ZLay_GpuDriverDraw(
  ZLay_GpuDriver* driver,
  ZLay_GpuBufferHandle vertex_buffer,
  ZLay_GpuBufferHandle index_buffer,
  uint32_t index_count
) {
  if (ZLay_GpuDriverIsValid(driver) && driver->vtable->draw != NULL) {
    driver->vtable->draw(driver, vertex_buffer, index_buffer, index_count);
  }
}

#ifdef __cplusplus
}
#endif

#endif
