#pragma once

#include <zlay.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef ZLAY_JOURNAL_PATH_CAPACITY
#  define ZLAY_JOURNAL_PATH_CAPACITY 260u
#endif

typedef struct ZLay_JournalChange {
  uint64_t sequence;
  uint64_t timestamp_ms;
  ZLay_Id target_id;
  ZLay_String field;
  ZLay_String before_value;
  ZLay_String after_value;
  uint32_t flags;
} ZLay_JournalChange;

typedef bool (*ZLay_JournalReplayFn)(const ZLay_JournalChange* change, void* user);

typedef struct ZLay_Journal {
  char path[ZLAY_JOURNAL_PATH_CAPACITY];
  void* handle;
  uint64_t next_sequence;
  bool open;
} ZLay_Journal;

ZLAY_API void ZLay_JournalInit(ZLay_Journal* journal, const char* path);
ZLAY_API bool ZLay_JournalOpen(ZLay_Journal* journal);
ZLAY_API bool ZLay_JournalAppendChange(ZLay_Journal* journal, const ZLay_JournalChange* change);
ZLAY_API bool ZLay_JournalFlush(ZLay_Journal* journal);
ZLAY_API void ZLay_JournalClose(ZLay_Journal* journal);
ZLAY_API bool ZLay_JournalReplay(const char* path, ZLay_JournalReplayFn replay, void* user);
ZLAY_API bool ZLay_JournalClearFile(const char* path);

#ifdef __cplusplus
}
#endif
