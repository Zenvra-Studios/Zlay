#include <drivers/gpu/zlay_frame.h>

#include <string.h>

void ZLay_FrameResourcesReset(ZLay_FrameResources* resources) {
  if (resources == NULL) {
    return;
  }

  memset(resources, 0, sizeof(*resources));
}

void ZLay_FrameSchedulerInit(ZLay_FrameScheduler* scheduler, uint32_t frames_in_flight, bool low_latency) {
  if (scheduler == NULL) {
    return;
  }

  if (frames_in_flight == 0u) {
    frames_in_flight = low_latency ? 1u : 2u;
  }
  if (frames_in_flight > ZLAY_DRIVER_MAX_FRAMES_IN_FLIGHT) {
    frames_in_flight = ZLAY_DRIVER_MAX_FRAMES_IN_FLIGHT;
  }

  scheduler->frames_in_flight = frames_in_flight;
  scheduler->current_frame = 0u;
  scheduler->frame_index = 0u;
  scheduler->low_latency = low_latency;
}

ZLay_FrameResources* ZLay_FrameSchedulerAcquire(
  ZLay_FrameScheduler* scheduler,
  ZLay_FrameResources* resources,
  uint32_t resource_count
) {
  ZLay_FrameResources* frame;

  if (scheduler == NULL || resources == NULL || resource_count == 0u) {
    return NULL;
  }

  if (scheduler->current_frame >= resource_count) {
    scheduler->current_frame = 0u;
  }

  frame = &resources[scheduler->current_frame];
  frame->frame_index = (uint32_t)scheduler->frame_index;
  frame->in_use = true;
  return frame;
}

void ZLay_FrameSchedulerPresent(ZLay_FrameScheduler* scheduler) {
  if (scheduler == NULL || scheduler->frames_in_flight == 0u) {
    return;
  }

  ++scheduler->frame_index;
  scheduler->current_frame = (scheduler->current_frame + 1u) % scheduler->frames_in_flight;
}
