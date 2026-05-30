#ifndef ZLAY_DRIVERS_GPU_VULKAN_ZLAY_VK_SYNC_H
#define ZLAY_DRIVERS_GPU_VULKAN_ZLAY_VK_SYNC_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

#include <zlay.h>

typedef struct ZLay_VKSync {
  uint64_t frame_index;
  bool fence_signaled;
  bool image_available;
  bool render_finished;
  bool valid;
} ZLay_VKSync;

ZLAY_API void ZLay_VKSync_Init(ZLay_VKSync* sync);
ZLAY_API bool ZLay_VKSync_BeginFrame(ZLay_VKSync* sync, uint64_t frame_index);
ZLAY_API bool ZLay_VKSync_EndFrame(ZLay_VKSync* sync);
ZLAY_API void ZLay_VKSync_Shutdown(ZLay_VKSync* sync);

#ifdef __cplusplus
}
#endif

#endif
