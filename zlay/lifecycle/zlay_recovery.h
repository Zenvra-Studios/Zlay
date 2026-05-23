#pragma once

#include <lifecycle/zlay_journal.h>
#include <lifecycle/zlay_snapshot.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef ZLAY_RECOVERY_PATH_CAPACITY
#  define ZLAY_RECOVERY_PATH_CAPACITY 260u
#endif

typedef enum ZLay_RecoveryStatus {
  ZLAY_RECOVERY_NONE = 0,
  ZLAY_RECOVERY_SNAPSHOT_AVAILABLE = 1,
  ZLAY_RECOVERY_RESTORED = 2,
  ZLAY_RECOVERY_FAILED = 3
} ZLay_RecoveryStatus;

typedef struct ZLay_Recovery {
  char snapshot_path[ZLAY_RECOVERY_PATH_CAPACITY];
  char journal_path[ZLAY_RECOVERY_PATH_CAPACITY];
  ZLay_RecoveryStatus status;
  uint64_t restored_sequence;
} ZLay_Recovery;

ZLAY_API void ZLay_RecoveryInit(ZLay_Recovery* recovery, const char* snapshot_path, const char* journal_path);
ZLAY_API bool ZLay_RecoveryProbe(ZLay_Recovery* recovery);
ZLAY_API bool ZLay_RecoverySaveSnapshot(
  ZLay_Recovery* recovery,
  const void* bytes,
  size_t size,
  uint64_t timestamp_ms,
  uint64_t sequence
);
ZLAY_API bool ZLay_RecoveryRestoreSnapshot(ZLay_Recovery* recovery, ZLay_Snapshot* snapshot);
ZLAY_API bool ZLay_RecoveryReplayJournal(ZLay_Recovery* recovery, ZLay_JournalReplayFn replay, void* user);
ZLAY_API bool ZLay_RecoveryClear(ZLay_Recovery* recovery);

#ifdef __cplusplus
}
#endif
