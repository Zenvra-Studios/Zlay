#include <backends/platform/gtk/zlay_gtk_backend.h>

#include <time.h>

uint64_t ZLay_GTK_NowNs(void) {
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  return ((uint64_t)ts.tv_sec * 1000000000ull) + (uint64_t)ts.tv_nsec;
}

double ZLay_GTK_EventTimestamp(void) {
  return (double)ZLay_GTK_NowNs() / 1000000000.0;
}
