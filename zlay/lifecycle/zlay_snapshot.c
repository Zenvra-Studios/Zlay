#include <lifecycle/zlay_snapshot.h>

#include <stdio.h>
#include <string.h>

static uint64_t ZLay__SnapshotHashBytes(const void* bytes, size_t size) {
  const uint8_t* cursor = (const uint8_t*)bytes;
  uint64_t hash = 1469598103934665603ull;

  for (size_t i = 0u; i < size; ++i) {
    hash ^= (uint64_t)cursor[i];
    hash *= 1099511628211ull;
  }

  return hash;
}

static ZLay_SnapshotHeader ZLay__SnapshotHeaderFor(
  const void* bytes,
  size_t size,
  uint64_t timestamp_ms,
  uint64_t sequence
) {
  ZLay_SnapshotHeader header;
  header.magic = ZLAY_SNAPSHOT_MAGIC;
  header.version = ZLAY_SNAPSHOT_VERSION;
  header.timestamp_ms = timestamp_ms;
  header.sequence = sequence;
  header.payload_size = (uint64_t)size;
  header.checksum = size > 0u ? ZLay__SnapshotHashBytes(bytes, size) : 0u;
  return header;
}

void ZLay_SnapshotInit(ZLay_Snapshot* snapshot, void* storage, size_t storage_size) {
  if (snapshot == NULL) {
    return;
  }

  memset(snapshot, 0, sizeof(*snapshot));
  snapshot->payload = (uint8_t*)storage;
  snapshot->payload_capacity = storage_size;
}

bool ZLay_SnapshotCaptureBytes(
  ZLay_Snapshot* snapshot,
  const void* bytes,
  size_t size,
  uint64_t timestamp_ms,
  uint64_t sequence
) {
  if (snapshot == NULL ||
      size > snapshot->payload_capacity ||
      (size > 0u && (bytes == NULL || snapshot->payload == NULL))) {
    return false;
  }

  if (size > 0u) {
    memcpy(snapshot->payload, bytes, size);
  }

  snapshot->payload_size = size;
  snapshot->header = ZLay__SnapshotHeaderFor(snapshot->payload, size, timestamp_ms, sequence);
  snapshot->valid = true;
  return true;
}

bool ZLay_SnapshotValidate(const ZLay_Snapshot* snapshot) {
  uint64_t checksum;

  if (snapshot == NULL || !snapshot->valid) {
    return false;
  }

  if (snapshot->header.magic != ZLAY_SNAPSHOT_MAGIC || snapshot->header.version != ZLAY_SNAPSHOT_VERSION) {
    return false;
  }

  if (snapshot->header.payload_size != (uint64_t)snapshot->payload_size) {
    return false;
  }

  if (snapshot->payload_size > 0u && snapshot->payload == NULL) {
    return false;
  }

  checksum = snapshot->payload_size > 0u ? ZLay__SnapshotHashBytes(snapshot->payload, snapshot->payload_size) : 0u;
  return checksum == snapshot->header.checksum;
}

bool ZLay_SnapshotSaveFile(const ZLay_Snapshot* snapshot, const char* path) {
  FILE* file;
  bool ok = false;

  if (path == NULL || !ZLay_SnapshotValidate(snapshot)) {
    return false;
  }

  file = fopen(path, "wb");
  if (file == NULL) {
    return false;
  }

  ok = fwrite(&snapshot->header, sizeof(snapshot->header), 1u, file) == 1u;
  if (ok && snapshot->payload_size > 0u) {
    ok = fwrite(snapshot->payload, 1u, snapshot->payload_size, file) == snapshot->payload_size;
  }

  ok = fclose(file) == 0 && ok;
  return ok;
}

bool ZLay_SnapshotSaveBytesFile(
  const char* path,
  const void* bytes,
  size_t size,
  uint64_t timestamp_ms,
  uint64_t sequence
) {
  FILE* file;
  ZLay_SnapshotHeader header;
  bool ok;

  if (path == NULL || (size > 0u && bytes == NULL)) {
    return false;
  }

  file = fopen(path, "wb");
  if (file == NULL) {
    return false;
  }

  header = ZLay__SnapshotHeaderFor(bytes, size, timestamp_ms, sequence);
  ok = fwrite(&header, sizeof(header), 1u, file) == 1u;
  if (ok && size > 0u) {
    ok = fwrite(bytes, 1u, size, file) == size;
  }

  ok = fclose(file) == 0 && ok;
  return ok;
}

bool ZLay_SnapshotLoadFile(ZLay_Snapshot* snapshot, const char* path) {
  FILE* file;
  ZLay_SnapshotHeader header;
  bool ok;

  if (snapshot == NULL || path == NULL) {
    return false;
  }

  file = fopen(path, "rb");
  if (file == NULL) {
    return false;
  }

  ok = fread(&header, sizeof(header), 1u, file) == 1u;
  if (ok) {
    ok = header.magic == ZLAY_SNAPSHOT_MAGIC &&
         header.version == ZLAY_SNAPSHOT_VERSION &&
         header.payload_size <= (uint64_t)snapshot->payload_capacity;
  }

  if (ok && header.payload_size > 0u && snapshot->payload == NULL) {
    ok = false;
  }

  if (ok && header.payload_size > 0u) {
    ok = fread(snapshot->payload, 1u, (size_t)header.payload_size, file) == (size_t)header.payload_size;
  }

  fclose(file);

  if (!ok) {
    return false;
  }

  snapshot->header = header;
  snapshot->payload_size = (size_t)header.payload_size;
  snapshot->valid = true;
  return ZLay_SnapshotValidate(snapshot);
}

void ZLay_SnapshotClear(ZLay_Snapshot* snapshot) {
  if (snapshot == NULL) {
    return;
  }

  snapshot->header = (ZLay_SnapshotHeader){0};
  snapshot->payload_size = 0u;
  snapshot->valid = false;
}
