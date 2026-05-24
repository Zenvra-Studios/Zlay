#ifndef ZLAY_OS_SYSTEM_ZLAY_TIMER_H
#define ZLAY_OS_SYSTEM_ZLAY_TIMER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include <zlay.h>

typedef struct ZLay_FrameTimer {
  double last_time;
  double now;
  double delta;
  double elapsed;
  uint64_t frame_index;
} ZLay_FrameTimer;

ZLAY_API double ZLay_TimeNow(void);
ZLAY_API void ZLay_SleepMilliseconds(uint32_t ms);
ZLAY_API void ZLay_FrameTimerInit(ZLay_FrameTimer* timer);
ZLAY_API void ZLay_FrameTimerTick(ZLay_FrameTimer* timer);

#ifdef __cplusplus
}
#endif

#endif
