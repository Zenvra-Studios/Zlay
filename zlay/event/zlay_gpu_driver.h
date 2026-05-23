#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include <event/zlay_event_handle.h>
#include <driver/opengl/zlay_opengl.h>
#include <driver/vulkan/zlay_vulkan.h>
#include <renderer/zlay_renderer.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ZLAY_GPU_CAP_RENDER_COMMANDS (1u << 0)
#define ZLAY_GPU_CAP_TEXTURES (1u << 1)
#define ZLAY_GPU_CAP_CLIPPING (1u << 2)
#define ZLAY_GPU_CAP_TEXT (1u << 3)
#define ZLAY_GPU_CAP_COMPUTE (1u << 4)
#define ZLAY_GPU_CAP_SWAPCHAIN (1u << 5)

typedef struct ZLay_GPUDriver ZLay_GPUDriver;

typedef bool (*ZLay_GPUDriverBeginFrameFn)(ZLay_GPUDriver* driver, const ZLay_RendererFrameInfo* frame);
typedef bool (*ZLay_GPUDriverSubmitFn)(ZLay_GPUDriver* driver, const ZLay_RenderCommandList* commands);
typedef void (*ZLay_GPUDriverEndFrameFn)(ZLay_GPUDriver* driver);
typedef void (*ZLay_GPUDriverShutdownFn)(ZLay_GPUDriver* driver);

struct ZLay_GPUDriver {
  ZLay_RenderDriverKind type;
  const char* name;
  uint32_t capabilities;
  ZLay_Renderer* renderer;
  void* native_context;
  void* native_instance;
  void* native_physical_device;
  void* native_device;
  void* native_queue;
  void* native_surface;
  void* native_swapchain;
  ZLay_GPUDriverBeginFrameFn begin_frame;
  ZLay_GPUDriverSubmitFn submit;
  ZLay_GPUDriverEndFrameFn end_frame;
  ZLay_GPUDriverShutdownFn shutdown;
  void* user;
};

static inline const char* ZLay_RenderDriverKindName(ZLay_RenderDriverKind driver) {
  switch (driver) {
    case ZLAY_RENDER_DRIVER_OPENGL:
      return "OpenGL";
    case ZLAY_RENDER_DRIVER_VULKAN:
      return "Vulkan";
    case ZLAY_RENDER_DRIVER_NONE:
    default:
      return "None";
  }
}

static inline void ZLay_GPUDriverInit(
  ZLay_GPUDriver* driver,
  ZLay_RenderDriverKind type,
  const char* name
) {
  if (driver == NULL) {
    return;
  }

  memset(driver, 0, sizeof(*driver));
  driver->type = type;
  driver->name = name != NULL ? name : ZLay_RenderDriverKindName(type);
  driver->capabilities = ZLAY_GPU_CAP_RENDER_COMMANDS | ZLAY_GPU_CAP_CLIPPING;
}

static inline void ZLay_GPUDriverSetRenderer(ZLay_GPUDriver* driver, ZLay_Renderer* renderer) {
  if (driver != NULL) {
    driver->renderer = renderer;
  }
}

static inline void ZLay_GPUDriverSetCallbacks(
  ZLay_GPUDriver* driver,
  ZLay_GPUDriverBeginFrameFn begin_frame,
  ZLay_GPUDriverSubmitFn submit,
  ZLay_GPUDriverEndFrameFn end_frame,
  ZLay_GPUDriverShutdownFn shutdown
) {
  if (driver == NULL) {
    return;
  }

  driver->begin_frame = begin_frame;
  driver->submit = submit;
  driver->end_frame = end_frame;
  driver->shutdown = shutdown;
}

static inline void ZLay_GPUDriverSetOpenGLNative(
  ZLay_GPUDriver* driver,
  void* context,
  void* surface
) {
  if (driver == NULL) {
    return;
  }

  driver->type = ZLAY_RENDER_DRIVER_OPENGL;
  driver->native_context = context;
  driver->native_surface = surface;
}

static inline void ZLay_GPUDriverSetVulkanNative(
  ZLay_GPUDriver* driver,
  void* instance,
  void* physical_device,
  void* device,
  void* queue,
  void* surface,
  void* swapchain
) {
  if (driver == NULL) {
    return;
  }

  driver->type = ZLAY_RENDER_DRIVER_VULKAN;
  driver->native_instance = instance;
  driver->native_physical_device = physical_device;
  driver->native_device = device;
  driver->native_queue = queue;
  driver->native_surface = surface;
  driver->native_swapchain = swapchain;
  driver->capabilities |= ZLAY_GPU_CAP_SWAPCHAIN;
}

static inline ZLay_GPUDriver ZLay_GPUDriverOpenGL(
  void* context,
  void* surface,
  ZLay_Renderer* renderer
) {
  ZLay_GPUDriver driver;
  ZLay_GPUDriverInit(&driver, ZLAY_RENDER_DRIVER_OPENGL, "OpenGL Render Driver");
  ZLay_GPUDriverSetOpenGLNative(&driver, context, surface);
  ZLay_GPUDriverSetRenderer(&driver, renderer);
  return driver;
}

static inline ZLay_GPUDriver ZLay_GPUDriverVulkan(
  void* instance,
  void* physical_device,
  void* device,
  void* queue,
  void* surface,
  void* swapchain,
  ZLay_Renderer* renderer
) {
  ZLay_GPUDriver driver;
  ZLay_GPUDriverInit(&driver, ZLAY_RENDER_DRIVER_VULKAN, "Vulkan Render Driver");
  ZLay_GPUDriverSetVulkanNative(&driver, instance, physical_device, device, queue, surface, swapchain);
  ZLay_GPUDriverSetRenderer(&driver, renderer);
  return driver;
}

static inline ZLay_GPUDriver ZLay_GPUDriverFromOpenGLRenderer(
  ZLay_OpenGLRenderer* opengl,
  ZLay_Renderer* renderer
) {
  ZLay_GPUDriver driver = ZLay_GPUDriverOpenGL(NULL, NULL, renderer);
  driver.user = opengl;
  driver.capabilities |= ZLAY_GPU_CAP_TEXT | ZLAY_GPU_CAP_TEXTURES;
  return driver;
}

static inline ZLay_GPUDriver ZLay_GPUDriverFromVulkanRenderer(
  ZLay_VulkanRenderer* vulkan,
  ZLay_Renderer* renderer
) {
  ZLay_GPUDriver driver = ZLay_GPUDriverVulkan(NULL, NULL, NULL, NULL, NULL, NULL, renderer);
  driver.user = vulkan;
  driver.capabilities |= ZLAY_GPU_CAP_TEXT | ZLAY_GPU_CAP_TEXTURES | ZLAY_GPU_CAP_SWAPCHAIN;
  return driver;
}

static inline void ZLay_RenderSceneBindGPUDriver(ZLay_RenderScene* scene, ZLay_GPUDriver* driver) {
  if (scene == NULL) {
    return;
  }

  scene->driver_kind = driver != NULL ? driver->type : ZLAY_RENDER_DRIVER_NONE;
  scene->driver_handle = driver;
}

static inline ZLay_GPUDriver* ZLay_RenderSceneGPUDriver(ZLay_RenderScene* scene) {
  return scene != NULL ? (ZLay_GPUDriver*)scene->driver_handle : NULL;
}

static inline bool ZLay_GPUDriverSupports(const ZLay_GPUDriver* driver, uint32_t capability) {
  return driver != NULL && (driver->capabilities & capability) == capability;
}

static inline bool ZLay_GPUDriverBeginFrame(
  ZLay_GPUDriver* driver,
  const ZLay_RendererFrameInfo* frame
) {
  if (driver == NULL || frame == NULL) {
    return false;
  }

  if (driver->begin_frame != NULL) {
    return driver->begin_frame(driver, frame);
  }

  if (driver->renderer != NULL && driver->renderer->begin_frame != NULL) {
    return driver->renderer->begin_frame(driver->renderer->user, frame);
  }

  return true;
}

static inline bool ZLay_GPUDriverSubmit(
  ZLay_GPUDriver* driver,
  const ZLay_RenderCommandList* commands
) {
  if (driver == NULL || commands == NULL) {
    return false;
  }

  if (driver->submit != NULL) {
    return driver->submit(driver, commands);
  }

  if (driver->renderer != NULL && driver->renderer->render != NULL) {
    return driver->renderer->render(driver->renderer->user, commands, &driver->renderer->last_stats);
  }

  return true;
}

static inline void ZLay_GPUDriverEndFrame(ZLay_GPUDriver* driver) {
  if (driver == NULL) {
    return;
  }

  if (driver->end_frame != NULL) {
    driver->end_frame(driver);
    return;
  }

  if (driver->renderer != NULL && driver->renderer->end_frame != NULL) {
    driver->renderer->end_frame(driver->renderer->user);
  }
}

static inline void ZLay_GPUDriverShutdown(ZLay_GPUDriver* driver) {
  if (driver == NULL) {
    return;
  }

  if (driver->shutdown != NULL) {
    driver->shutdown(driver);
    return;
  }

  if (driver->renderer != NULL && driver->renderer->shutdown != NULL) {
    driver->renderer->shutdown(driver->renderer->user);
  }
}

static inline bool ZLay_RenderSceneSubmitCommands(
  ZLay_RenderScene* scene,
  const ZLay_RendererFrameInfo* frame,
  const ZLay_RenderCommandList* commands
) {
  ZLay_GPUDriver* driver = ZLay_RenderSceneGPUDriver(scene);

  if (driver == NULL) {
    return false;
  }

  if (!ZLay_GPUDriverBeginFrame(driver, frame)) {
    return false;
  }

  if (!ZLay_GPUDriverSubmit(driver, commands)) {
    ZLay_GPUDriverEndFrame(driver);
    return false;
  }

  ZLay_GPUDriverEndFrame(driver);
  return true;
}

static inline void ZLay_RenderMarkSubtreeDirty(ZLay_RenderNode* node, uint32_t state) {
  if (node == NULL) {
    return;
  }

  node->state |= state;
  for (uint32_t i = 0u; i < node->child_count; ++i) {
    ZLay_RenderMarkSubtreeDirty(node->children[i], state);
  }
}

#ifdef __cplusplus
}
#endif
