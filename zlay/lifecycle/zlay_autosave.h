#pragma once

#include <zlay.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef bool (*ZLay_AutosaveWriteFn)(void* user);

typedef struct ZLay_Autosave {
  double interval_ms;
  double elapsed_ms;
  uint64_t save_count;
  ZLay_AutosaveWriteFn write;
  void* user;
  bool dirty;
  bool enabled;
} ZLay_Autosave;

ZLAY_API void ZLay_AutosaveInit(
  ZLay_Autosave* autosave,
  double interval_ms,
  ZLay_AutosaveWriteFn write,
  void* user
);
ZLAY_API void ZLay_AutosaveSetEnabled(ZLay_Autosave* autosave, bool enabled);
ZLAY_API void ZLay_AutosaveMarkDirty(ZLay_Autosave* autosave);
ZLAY_API void ZLay_AutosaveMarkClean(ZLay_Autosave* autosave);
ZLAY_API bool ZLay_AutosaveTick(ZLay_Autosave* autosave, double delta_ms);
ZLAY_API bool ZLay_AutosaveForce(ZLay_Autosave* autosave);

#ifdef __cplusplus
}
#endif
