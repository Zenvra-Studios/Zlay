#ifndef ZLAY_DRIVERS_GPU_ZLAY_PRESENT_H
#define ZLAY_DRIVERS_GPU_ZLAY_PRESENT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

#include <zlay.h>

typedef enum ZLay_PresentMode {
  ZLAY_PRESENT_MODE_IMMEDIATE = 0,
  ZLAY_PRESENT_MODE_FIFO = 1,
  ZLAY_PRESENT_MODE_MAILBOX = 2
} ZLay_PresentMode;

typedef struct ZLay_PresentConfig {
  bool vsync;
  bool low_latency;
  uint32_t frames_in_flight;
  ZLay_PresentMode mode;
} ZLay_PresentConfig;

ZLAY_API ZLay_PresentConfig ZLay_PresentConfigDefault(void);
ZLAY_API ZLay_PresentMode ZLay_PresentChooseMode(bool vsync, bool low_latency);
ZLAY_API uint32_t ZLay_PresentChooseFramesInFlight(bool low_latency, uint32_t requested);
ZLAY_API const char* ZLay_PresentModeName(ZLay_PresentMode mode);

#ifdef __cplusplus
}
#endif

#endif
