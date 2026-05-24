#include <os/system/zlay_timer.h>

#include <os/zlay_os.h>

#if defined(_WIN32)
#  define WIN32_LEAN_AND_MEAN
#  include <windows.h>
#else
#  include <errno.h>
#  include <time.h>
#endif

double ZLay_TimeNow(void) {
  return (double)ZLay_OS_NowNs() / 1000000000.0;
}

void ZLay_SleepMilliseconds(uint32_t ms) {
#if defined(_WIN32)
  Sleep((DWORD)ms);
#else
  struct timespec requested;
  struct timespec remaining;

  requested.tv_sec = (time_t)(ms / 1000u);
  requested.tv_nsec = (long)((ms % 1000u) * 1000000u);

  while (nanosleep(&requested, &remaining) != 0 && errno == EINTR) {
    requested = remaining;
  }
#endif
}

void ZLay_FrameTimerInit(ZLay_FrameTimer* timer) {
  if (timer == NULL) {
    return;
  }

  timer->last_time = ZLay_TimeNow();
  timer->now = timer->last_time;
  timer->delta = 0.0;
  timer->elapsed = 0.0;
  timer->frame_index = 0u;
}

void ZLay_FrameTimerTick(ZLay_FrameTimer* timer) {
  if (timer == NULL) {
    return;
  }

  timer->last_time = timer->now;
  timer->now = ZLay_TimeNow();
  timer->delta = timer->now - timer->last_time;
  if (timer->delta < 0.0) {
    timer->delta = 0.0;
  }
  timer->elapsed += timer->delta;
  timer->frame_index += 1u;
}
