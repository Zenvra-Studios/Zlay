#pragma once

#include <zlay.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ZLAY_SNAPSHOT_MAGIC 0x5A534E50u
#define ZLAY_SNAPSHOT_VERSION 1u

typedef struct ZLay_SnapshotHeader {
  uint32_t magic;
  uint32_t version;
  uint64_t timestamp_ms;
  uint64_t sequence;
  uint64_t payload_size;
  uint64_t checksum;
} ZLay_SnapshotHeader;

typedef struct ZLay_Snapshot {
  ZLay_SnapshotHeader header;
  uint8_t* payload;
  size_t payload_size;
  size_t payload_capacity;
  bool valid;
} ZLay_Snapshot;

ZLAY_API void ZLay_SnapshotInit(ZLay_Snapshot* snapshot, void* storage, size_t storage_size);
ZLAY_API bool ZLay_SnapshotCaptureBytes(
  ZLay_Snapshot* snapshot,
  const void* bytes,
  size_t size,
  uint64_t timestamp_ms,
  uint64_t sequence
);
ZLAY_API bool ZLay_SnapshotValidate(const ZLay_Snapshot* snapshot);
ZLAY_API bool ZLay_SnapshotSaveFile(const ZLay_Snapshot* snapshot, const char* path);
ZLAY_API bool ZLay_SnapshotSaveBytesFile(
  const char* path,
  const void* bytes,
  size_t size,
  uint64_t timestamp_ms,
  uint64_t sequence
);
ZLAY_API bool ZLay_SnapshotLoadFile(ZLay_Snapshot* snapshot, const char* path);
ZLAY_API void ZLay_SnapshotClear(ZLay_Snapshot* snapshot);

#ifdef __cplusplus
}
#endif
