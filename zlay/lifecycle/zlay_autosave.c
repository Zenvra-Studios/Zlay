#include <lifecycle/zlay_autosave.h>

#include <string.h>

void ZLay_AutosaveInit(
  ZLay_Autosave* autosave,
  double interval_ms,
  ZLay_AutosaveWriteFn write,
  void* user
) {
  if (autosave == NULL) {
    return;
  }

  memset(autosave, 0, sizeof(*autosave));
  autosave->interval_ms = interval_ms > 0.0 ? interval_ms : 1000.0;
  autosave->write = write;
  autosave->user = user;
  autosave->enabled = true;
}

void ZLay_AutosaveSetEnabled(ZLay_Autosave* autosave, bool enabled) {
  if (autosave != NULL) {
    autosave->enabled = enabled;
  }
}

void ZLay_AutosaveMarkDirty(ZLay_Autosave* autosave) {
  if (autosave != NULL) {
    autosave->dirty = true;
  }
}

void ZLay_AutosaveMarkClean(ZLay_Autosave* autosave) {
  if (autosave != NULL) {
    autosave->dirty = false;
    autosave->elapsed_ms = 0.0;
  }
}

bool ZLay_AutosaveForce(ZLay_Autosave* autosave) {
  if (autosave == NULL || !autosave->enabled || autosave->write == NULL || !autosave->dirty) {
    return false;
  }

  if (!autosave->write(autosave->user)) {
    return false;
  }

  autosave->dirty = false;
  autosave->elapsed_ms = 0.0;
  ++autosave->save_count;
  return true;
}

bool ZLay_AutosaveTick(ZLay_Autosave* autosave, double delta_ms) {
  if (autosave == NULL || !autosave->enabled || !autosave->dirty) {
    return false;
  }

  if (delta_ms > 0.0) {
    autosave->elapsed_ms += delta_ms;
  }

  if (autosave->elapsed_ms < autosave->interval_ms) {
    return false;
  }

  return ZLay_AutosaveForce(autosave);
}
