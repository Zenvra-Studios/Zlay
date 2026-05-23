#include <driver/vulkan/zlay_vk_sync.h>

#include <string.h>

void ZLay_VKSync_Init(ZLay_VKSync* sync) {
  if (!sync) return;
  memset(sync, 0, sizeof(*sync));
  sync->fence_signaled = true;
  sync->valid = true;
}

bool ZLay_VKSync_BeginFrame(ZLay_VKSync* sync, uint64_t frame_index) {
  if (!sync || !sync->valid || !sync->fence_signaled) return false;
  sync->frame_index = frame_index;
  sync->fence_signaled = false;
  sync->image_available = true;
  sync->render_finished = false;
  return true;
}

bool ZLay_VKSync_EndFrame(ZLay_VKSync* sync) {
  if (!sync || !sync->valid || !sync->image_available) return false;
  sync->render_finished = true;
  sync->image_available = false;
  sync->fence_signaled = true;
  return true;
}

void ZLay_VKSync_Shutdown(ZLay_VKSync* sync) {
  if (!sync) return;
  sync->fence_signaled = false;
  sync->image_available = false;
  sync->render_finished = false;
  sync->valid = false;
}
