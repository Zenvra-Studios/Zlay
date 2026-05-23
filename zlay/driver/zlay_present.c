#include <driver/zlay_present.h>

ZLay_PresentConfig ZLay_PresentConfigDefault(void) {
  ZLay_PresentConfig config;
  config.vsync = true;
  config.low_latency = false;
  config.frames_in_flight = 2u;
  config.mode = ZLAY_PRESENT_MODE_FIFO;
  return config;
}

ZLay_PresentMode ZLay_PresentChooseMode(bool vsync, bool low_latency) {
  if (!vsync) {
    return ZLAY_PRESENT_MODE_IMMEDIATE;
  }

  return low_latency ? ZLAY_PRESENT_MODE_MAILBOX : ZLAY_PRESENT_MODE_FIFO;
}

uint32_t ZLay_PresentChooseFramesInFlight(bool low_latency, uint32_t requested) {
  if (requested != 0u) {
    if (requested < 1u) return 1u;
    if (requested > 3u) return 3u;
    return requested;
  }

  return low_latency ? 1u : 2u;
}

const char* ZLay_PresentModeName(ZLay_PresentMode mode) {
  switch (mode) {
    case ZLAY_PRESENT_MODE_IMMEDIATE: return "Immediate";
    case ZLAY_PRESENT_MODE_MAILBOX: return "Mailbox";
    case ZLAY_PRESENT_MODE_FIFO:
    default: return "FIFO";
  }
}
