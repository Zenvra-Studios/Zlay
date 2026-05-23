#pragma once

#include <zlay.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ZLay_HistoryEntry {
  uint64_t sequence;
  uint64_t timestamp_ms;
  ZLay_Id target_id;
  ZLay_String label;
  ZLay_String before_value;
  ZLay_String after_value;
  uint32_t flags;
} ZLay_HistoryEntry;

typedef struct ZLay_History {
  ZLay_HistoryEntry* entries;
  uint32_t capacity;
  uint32_t count;
  uint32_t cursor;
  uint64_t next_sequence;
} ZLay_History;

ZLAY_API void ZLay_HistoryInit(ZLay_History* history, ZLay_HistoryEntry* entries, uint32_t capacity);
ZLAY_API void ZLay_HistoryClear(ZLay_History* history);
ZLAY_API bool ZLay_HistoryPush(ZLay_History* history, ZLay_HistoryEntry entry);
ZLAY_API bool ZLay_HistoryCanUndo(const ZLay_History* history);
ZLAY_API bool ZLay_HistoryCanRedo(const ZLay_History* history);
ZLAY_API bool ZLay_HistoryUndo(ZLay_History* history, ZLay_HistoryEntry* out_entry);
ZLAY_API bool ZLay_HistoryRedo(ZLay_History* history, ZLay_HistoryEntry* out_entry);

#ifdef __cplusplus
}
#endif
