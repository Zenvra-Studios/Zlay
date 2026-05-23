#include <lifecycle/zlay_lifecycle.h>

#include <string.h>

static void ZLay__LifecycleEmit(ZLay_Lifecycle* lifecycle, ZLay_LifecycleEvent event) {
  if (lifecycle != NULL && lifecycle->callbacks.on_event != NULL) {
    lifecycle->callbacks.on_event(event, lifecycle->user);
  }
}

void ZLay_LifecycleInit(ZLay_Lifecycle* lifecycle, ZLay_LifecycleCallbacks callbacks, void* user) {
  if (lifecycle == NULL) {
    return;
  }

  memset(lifecycle, 0, sizeof(*lifecycle));
  lifecycle->state = ZLAY_LIFECYCLE_CREATED;
  lifecycle->callbacks = callbacks;
  lifecycle->user = user;
}

bool ZLay_LifecycleStart(ZLay_Lifecycle* lifecycle) {
  if (lifecycle == NULL ||
      (lifecycle->state != ZLAY_LIFECYCLE_CREATED && lifecycle->state != ZLAY_LIFECYCLE_STOPPED)) {
    return false;
  }

  lifecycle->state = ZLAY_LIFECYCLE_RUNNING;
  lifecycle->time_ms = 0.0;
  lifecycle->frame_index = 0u;
  ZLay__LifecycleEmit(lifecycle, ZLAY_LIFECYCLE_EVENT_INIT);
  return true;
}

bool ZLay_LifecycleUpdate(ZLay_Lifecycle* lifecycle, double delta_ms) {
  if (lifecycle == NULL || lifecycle->state != ZLAY_LIFECYCLE_RUNNING) {
    return false;
  }

  if (delta_ms > 0.0) {
    lifecycle->time_ms += delta_ms;
  }

  ++lifecycle->frame_index;
  if (lifecycle->callbacks.on_update != NULL) {
    lifecycle->callbacks.on_update(delta_ms, lifecycle->user);
  }
  ZLay__LifecycleEmit(lifecycle, ZLAY_LIFECYCLE_EVENT_UPDATE);
  return true;
}

bool ZLay_LifecycleSuspend(ZLay_Lifecycle* lifecycle) {
  if (lifecycle == NULL || lifecycle->state != ZLAY_LIFECYCLE_RUNNING) {
    return false;
  }

  lifecycle->state = ZLAY_LIFECYCLE_SUSPENDED;
  ZLay__LifecycleEmit(lifecycle, ZLAY_LIFECYCLE_EVENT_SUSPEND);
  return true;
}

bool ZLay_LifecycleResume(ZLay_Lifecycle* lifecycle) {
  if (lifecycle == NULL || lifecycle->state != ZLAY_LIFECYCLE_SUSPENDED) {
    return false;
  }

  lifecycle->state = ZLAY_LIFECYCLE_RUNNING;
  ZLay__LifecycleEmit(lifecycle, ZLAY_LIFECYCLE_EVENT_RESUME);
  return true;
}

bool ZLay_LifecycleShutdown(ZLay_Lifecycle* lifecycle) {
  if (lifecycle == NULL ||
      lifecycle->state == ZLAY_LIFECYCLE_STOPPED ||
      lifecycle->state == ZLAY_LIFECYCLE_SHUTTING_DOWN) {
    return false;
  }

  lifecycle->state = ZLAY_LIFECYCLE_SHUTTING_DOWN;
  ZLay__LifecycleEmit(lifecycle, ZLAY_LIFECYCLE_EVENT_SHUTDOWN);
  lifecycle->state = ZLAY_LIFECYCLE_STOPPED;
  return true;
}

bool ZLay_LifecycleRecordChange(
  ZLay_Session* session,
  ZLay_Journal* journal,
  ZLay_History* history,
  ZLay_Autosave* autosave,
  const ZLay_JournalChange* change
) {
  bool ok = true;
  ZLay_JournalChange copy;

  if (change == NULL) {
    return false;
  }

  copy = *change;
  if (copy.sequence == 0u) {
    if (journal != NULL && journal->next_sequence != 0u) {
      copy.sequence = journal->next_sequence;
    } else if (history != NULL && history->next_sequence != 0u) {
      copy.sequence = history->next_sequence;
    }
  }

  if (journal != NULL) {
    if (!journal->open && !ZLay_JournalOpen(journal)) {
      ok = false;
    } else if (!ZLay_JournalAppendChange(journal, &copy)) {
      ok = false;
    } else if (!ZLay_JournalFlush(journal)) {
      ok = false;
    }
  }

  if (history != NULL) {
    ZLay_HistoryEntry entry;
    entry.sequence = copy.sequence;
    entry.timestamp_ms = copy.timestamp_ms;
    entry.target_id = copy.target_id;
    entry.label = copy.field;
    entry.before_value = copy.before_value;
    entry.after_value = copy.after_value;
    entry.flags = copy.flags;
    ok = ZLay_HistoryPush(history, entry) && ok;
  }

  if (session != NULL) {
    ZLay_SessionMarkDirty(session);
  }

  if (autosave != NULL) {
    ZLay_AutosaveMarkDirty(autosave);
  }

  return ok;
}
