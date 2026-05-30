#ifndef ZLAY_DRIVERS_GPU_VULKAN_ZLAY_VK_COMMAND_H
#define ZLAY_DRIVERS_GPU_VULKAN_ZLAY_VK_COMMAND_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

#include <drivers/gpu/zlay_driver.h>

typedef struct ZLay_VKCommand {
  uint64_t frame_index;
  uint32_t image_index;
  uint32_t draw_calls;
  bool recording;
  bool submitted;
} ZLay_VKCommand;

ZLAY_API void ZLay_VKCommand_Init(ZLay_VKCommand* command);
ZLAY_API bool ZLay_VKCommand_Begin(ZLay_VKCommand* command, uint64_t frame_index, uint32_t image_index);
ZLAY_API bool ZLay_VKCommand_RecordPacket(ZLay_VKCommand* command, const ZLay_DriverFramePacket* packet, uint32_t draw_calls);
ZLAY_API bool ZLay_VKCommand_End(ZLay_VKCommand* command);
ZLAY_API void ZLay_VKCommand_Reset(ZLay_VKCommand* command);

#ifdef __cplusplus
}
#endif

#endif
