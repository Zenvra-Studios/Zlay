#include <drivers/gpu/vulkan/zlay_vk_command.h>

#include <string.h>

void ZLay_VKCommand_Init(ZLay_VKCommand* command) {
  if (!command) return;
  memset(command, 0, sizeof(*command));
}

bool ZLay_VKCommand_Begin(ZLay_VKCommand* command, uint64_t frame_index, uint32_t image_index) {
  if (!command || command->recording) return false;
  command->frame_index = frame_index;
  command->image_index = image_index;
  command->draw_calls = 0u;
  command->recording = true;
  command->submitted = false;
  return true;
}

bool ZLay_VKCommand_RecordPacket(ZLay_VKCommand* command, const ZLay_DriverFramePacket* packet, uint32_t draw_calls) {
  if (!command || !packet || !command->recording) return false;
  command->image_index = packet->swapchain_image_index;
  command->draw_calls = draw_calls;
  return true;
}

bool ZLay_VKCommand_End(ZLay_VKCommand* command) {
  if (!command || !command->recording) return false;
  command->recording = false;
  command->submitted = true;
  return true;
}

void ZLay_VKCommand_Reset(ZLay_VKCommand* command) {
  if (!command) return;
  command->recording = false;
  command->submitted = false;
  command->draw_calls = 0u;
}
