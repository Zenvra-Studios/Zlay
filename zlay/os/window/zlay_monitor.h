#ifndef ZLAY_OS_WINDOW_ZLAY_MONITOR_H
#define ZLAY_OS_WINDOW_ZLAY_MONITOR_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

#include <zlay.h>

typedef struct ZLay_MonitorInfo {
  int32_t x;
  int32_t y;
  uint32_t width;
  uint32_t height;
  float scale_x;
  float scale_y;
  uint32_t refresh_rate;
  const char* name;
} ZLay_MonitorInfo;

ZLAY_API uint32_t ZLay_GetMonitorCount(void);
ZLAY_API bool ZLay_GetMonitorInfo(uint32_t index, ZLay_MonitorInfo* out_info);

#ifdef __cplusplus
}
#endif

#endif
