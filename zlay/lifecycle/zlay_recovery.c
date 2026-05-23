#include <lifecycle/zlay_recovery.h>

#include <stdio.h>
#include <string.h>

static void ZLay__RecoveryCopyPath(char* dst, size_t capacity, const char* src) {
  size_t i = 0u;

  if (dst == NULL || capacity == 0u) {
    return;
  }

  if (src != NULL) {
    for (; i + 1u < capacity && src[i] != '\0'; ++i) {
      dst[i] = src[i];
    }
  }

  dst[i] = '\0';
}

static bool ZLay__RecoveryFileExists(const char* path) {
  FILE* file;

  if (path == NULL || path[0] == '\0') {
    return false;
  }

  file = fopen(path, "rb");
  if (file == NULL) {
    return false;
  }

  fclose(file);
  return true;
}

void ZLay_RecoveryInit(ZLay_Recovery* recovery, const char* snapshot_path, const char* journal_path) {
  if (recovery == NULL) {
    return;
  }

  memset(recovery, 0, sizeof(*recovery));
  ZLay__RecoveryCopyPath(recovery->snapshot_path, sizeof(recovery->snapshot_path), snapshot_path);
  ZLay__RecoveryCopyPath(recovery->journal_path, sizeof(recovery->journal_path), journal_path);
  recovery->status = ZLAY_RECOVERY_NONE;
}

bool ZLay_RecoveryProbe(ZLay_Recovery* recovery) {
  ZLay_SnapshotHeader header;
  FILE* file;

  if (recovery == NULL || recovery->snapshot_path[0] == '\0') {
    return false;
  }

  file = fopen(recovery->snapshot_path, "rb");
  if (file == NULL) {
    recovery->status = ZLAY_RECOVERY_NONE;
    return false;
  }

  if (fread(&header, sizeof(header), 1u, file) == 1u &&
      header.magic == ZLAY_SNAPSHOT_MAGIC &&
      header.version == ZLAY_SNAPSHOT_VERSION) {
    recovery->status = ZLAY_RECOVERY_SNAPSHOT_AVAILABLE;
    recovery->restored_sequence = header.sequence;
    fclose(file);
    return true;
  }

  fclose(file);
  recovery->status = ZLAY_RECOVERY_FAILED;
  return false;
}

bool ZLay_RecoverySaveSnapshot(
  ZLay_Recovery* recovery,
  const void* bytes,
  size_t size,
  uint64_t timestamp_ms,
  uint64_t sequence
) {
  if (recovery == NULL || recovery->snapshot_path[0] == '\0') {
    return false;
  }

  if (!ZLay_SnapshotSaveBytesFile(recovery->snapshot_path, bytes, size, timestamp_ms, sequence)) {
    recovery->status = ZLAY_RECOVERY_FAILED;
    return false;
  }

  recovery->status = ZLAY_RECOVERY_SNAPSHOT_AVAILABLE;
  recovery->restored_sequence = sequence;
  return true;
}

bool ZLay_RecoveryRestoreSnapshot(ZLay_Recovery* recovery, ZLay_Snapshot* snapshot) {
  if (recovery == NULL || snapshot == NULL || recovery->snapshot_path[0] == '\0') {
    return false;
  }

  if (!ZLay_SnapshotLoadFile(snapshot, recovery->snapshot_path)) {
    recovery->status = ZLAY_RECOVERY_FAILED;
    return false;
  }

  recovery->status = ZLAY_RECOVERY_RESTORED;
  recovery->restored_sequence = snapshot->header.sequence;
  return true;
}

bool ZLay_RecoveryReplayJournal(ZLay_Recovery* recovery, ZLay_JournalReplayFn replay, void* user) {
  if (recovery == NULL || recovery->journal_path[0] == '\0' || replay == NULL) {
    return false;
  }

  if (!ZLay__RecoveryFileExists(recovery->journal_path)) {
    return true;
  }

  return ZLay_JournalReplay(recovery->journal_path, replay, user);
}

bool ZLay_RecoveryClear(ZLay_Recovery* recovery) {
  bool ok = true;

  if (recovery == NULL) {
    return false;
  }

  if (recovery->snapshot_path[0] != '\0' && ZLay__RecoveryFileExists(recovery->snapshot_path)) {
    ok = remove(recovery->snapshot_path) == 0 && ok;
  }

  if (recovery->journal_path[0] != '\0' && ZLay__RecoveryFileExists(recovery->journal_path)) {
    ok = remove(recovery->journal_path) == 0 && ok;
  }

  if (ok) {
    recovery->status = ZLAY_RECOVERY_NONE;
    recovery->restored_sequence = 0u;
  }

  return ok;
}
