#pragma once

#include <zlay.h>

#include <lifecycle/zlay_session.h>
#include <lifecycle/zlay_snapshot.h>
#include <lifecycle/zlay_history.h>
#include <lifecycle/zlay_autosave.h>
#include <lifecycle/zlay_journal.h>
#include <lifecycle/zlay_recovery.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum ZLay_LifecycleState {
  ZLAY_LIFECYCLE_CREATED = 0,
  ZLAY_LIFECYCLE_RUNNING = 1,
  ZLAY_LIFECYCLE_SUSPENDED = 2,
  ZLAY_LIFECYCLE_SHUTTING_DOWN = 3,
  ZLAY_LIFECYCLE_STOPPED = 4
} ZLay_LifecycleState;

typedef enum ZLay_LifecycleEvent {
  ZLAY_LIFECYCLE_EVENT_INIT = 0,
  ZLAY_LIFECYCLE_EVENT_UPDATE = 1,
  ZLAY_LIFECYCLE_EVENT_SUSPEND = 2,
  ZLAY_LIFECYCLE_EVENT_RESUME = 3,
  ZLAY_LIFECYCLE_EVENT_SHUTDOWN = 4
} ZLay_LifecycleEvent;

typedef void (*ZLay_LifecycleEventFn)(ZLay_LifecycleEvent event, void* user);
typedef void (*ZLay_LifecycleUpdateFn)(double delta_ms, void* user);

typedef struct ZLay_LifecycleCallbacks {
  ZLay_LifecycleEventFn on_event;
  ZLay_LifecycleUpdateFn on_update;
} ZLay_LifecycleCallbacks;

typedef struct ZLay_Lifecycle {
  ZLay_LifecycleState state;
  ZLay_LifecycleCallbacks callbacks;
  void* user;
  double time_ms;
  uint64_t frame_index;
} ZLay_Lifecycle;

ZLAY_API void ZLay_LifecycleInit(ZLay_Lifecycle* lifecycle, ZLay_LifecycleCallbacks callbacks, void* user);
ZLAY_API bool ZLay_LifecycleStart(ZLay_Lifecycle* lifecycle);
ZLAY_API bool ZLay_LifecycleUpdate(ZLay_Lifecycle* lifecycle, double delta_ms);
ZLAY_API bool ZLay_LifecycleSuspend(ZLay_Lifecycle* lifecycle);
ZLAY_API bool ZLay_LifecycleResume(ZLay_Lifecycle* lifecycle);
ZLAY_API bool ZLay_LifecycleShutdown(ZLay_Lifecycle* lifecycle);
ZLAY_API bool ZLay_LifecycleRecordChange(
  ZLay_Session* session,
  ZLay_Journal* journal,
  ZLay_History* history,
  ZLay_Autosave* autosave,
  const ZLay_JournalChange* change
);

#ifdef __cplusplus
}
#endif
