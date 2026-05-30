#ifndef ZLAY_DRIVERS_GPU_ZLAY_FRAME_H
#define ZLAY_DRIVERS_GPU_ZLAY_FRAME_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

#include <zlay.h>

#ifndef ZLAY_DRIVER_MAX_FRAMES_IN_FLIGHT
#  define ZLAY_DRIVER_MAX_FRAMES_IN_FLIGHT 3u
#endif

typedef struct ZLay_FrameResources {
  uint32_t frame_index;
  void* vertex_buffer;
  void* index_buffer;
  void* uniform_buffer;
  void* command_buffer;
  void* fence;
  void* image_available;
  void* render_finished;
  bool in_use;
} ZLay_FrameResources;

typedef struct ZLay_FrameScheduler {
  uint32_t frames_in_flight;
  uint32_t current_frame;
  uint64_t frame_index;
  bool low_latency;
} ZLay_FrameScheduler;

ZLAY_API void ZLay_FrameResourcesReset(ZLay_FrameResources* resources);
ZLAY_API void ZLay_FrameSchedulerInit(ZLay_FrameScheduler* scheduler, uint32_t frames_in_flight, bool low_latency);
ZLAY_API ZLay_FrameResources* ZLay_FrameSchedulerAcquire(
  ZLay_FrameScheduler* scheduler,
  ZLay_FrameResources* resources,
  uint32_t resource_count
);
ZLAY_API void ZLay_FrameSchedulerPresent(ZLay_FrameScheduler* scheduler);

#ifdef __cplusplus
}
#endif

#endif
