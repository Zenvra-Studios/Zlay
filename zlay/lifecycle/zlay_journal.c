#include <lifecycle/zlay_journal.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ZLAY_JOURNAL_RECORD_MAGIC 0x5A4A524Eu
#define ZLAY_JOURNAL_RECORD_VERSION 1u

typedef struct ZLay__JournalRecordHeader {
  uint32_t magic;
  uint32_t version;
  uint32_t flags;
  uint32_t field_length;
  uint32_t before_length;
  uint32_t after_length;
  uint64_t sequence;
  uint64_t timestamp_ms;
  uint64_t target_hash;
} ZLay__JournalRecordHeader;

static void ZLay__JournalCopyPath(char* dst, size_t capacity, const char* src) {
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

static uint32_t ZLay__JournalStringLength(ZLay_String string) {
  return string.chars != NULL ? string.length : 0u;
}

static bool ZLay__JournalWriteString(FILE* file, ZLay_String string) {
  uint32_t length = ZLay__JournalStringLength(string);
  if (length == 0u) {
    return true;
  }

  return fwrite(string.chars, 1u, (size_t)length, file) == (size_t)length;
}

static bool ZLay__JournalReadString(FILE* file, uint32_t length, ZLay_String* out_string) {
  char* chars;

  if (out_string == NULL) {
    return false;
  }

  *out_string = (ZLay_String){0};
  if (length == 0u) {
    return true;
  }

  chars = (char*)malloc((size_t)length);
  if (chars == NULL) {
    return false;
  }

  if (fread(chars, 1u, (size_t)length, file) != (size_t)length) {
    free(chars);
    return false;
  }

  out_string->chars = chars;
  out_string->length = length;
  out_string->is_static = false;
  return true;
}

static void ZLay__JournalFreeString(ZLay_String string) {
  if (!string.is_static) {
    free((void*)string.chars);
  }
}

void ZLay_JournalInit(ZLay_Journal* journal, const char* path) {
  if (journal == NULL) {
    return;
  }

  memset(journal, 0, sizeof(*journal));
  ZLay__JournalCopyPath(journal->path, sizeof(journal->path), path);
  journal->next_sequence = 1u;
}

bool ZLay_JournalOpen(ZLay_Journal* journal) {
  FILE* file;

  if (journal == NULL || journal->path[0] == '\0') {
    return false;
  }

  if (journal->open) {
    return true;
  }

  file = fopen(journal->path, "ab");
  if (file == NULL) {
    return false;
  }

  journal->handle = file;
  journal->open = true;
  return true;
}

bool ZLay_JournalAppendChange(ZLay_Journal* journal, const ZLay_JournalChange* change) {
  FILE* file;
  ZLay_JournalChange copy;
  ZLay__JournalRecordHeader header;

  if (journal == NULL || change == NULL || !journal->open || journal->handle == NULL) {
    return false;
  }

  file = (FILE*)journal->handle;
  copy = *change;
  if (copy.sequence == 0u) {
    copy.sequence = journal->next_sequence++;
  } else if (journal->next_sequence <= copy.sequence) {
    journal->next_sequence = copy.sequence + 1u;
  }

  header.magic = ZLAY_JOURNAL_RECORD_MAGIC;
  header.version = ZLAY_JOURNAL_RECORD_VERSION;
  header.flags = copy.flags;
  header.field_length = ZLay__JournalStringLength(copy.field);
  header.before_length = ZLay__JournalStringLength(copy.before_value);
  header.after_length = ZLay__JournalStringLength(copy.after_value);
  header.sequence = copy.sequence;
  header.timestamp_ms = copy.timestamp_ms;
  header.target_hash = copy.target_id.hash;

  if (fwrite(&header, sizeof(header), 1u, file) != 1u) {
    return false;
  }

  return ZLay__JournalWriteString(file, copy.field) &&
         ZLay__JournalWriteString(file, copy.before_value) &&
         ZLay__JournalWriteString(file, copy.after_value);
}

bool ZLay_JournalFlush(ZLay_Journal* journal) {
  if (journal == NULL || !journal->open || journal->handle == NULL) {
    return false;
  }

  return fflush((FILE*)journal->handle) == 0;
}

void ZLay_JournalClose(ZLay_Journal* journal) {
  if (journal == NULL || journal->handle == NULL) {
    return;
  }

  fclose((FILE*)journal->handle);
  journal->handle = NULL;
  journal->open = false;
}

bool ZLay_JournalReplay(const char* path, ZLay_JournalReplayFn replay, void* user) {
  FILE* file;
  bool ok = true;

  if (path == NULL || replay == NULL) {
    return false;
  }

  file = fopen(path, "rb");
  if (file == NULL) {
    return false;
  }

  for (;;) {
    ZLay__JournalRecordHeader header;
    ZLay_JournalChange change;

    if (fread(&header, sizeof(header), 1u, file) != 1u) {
      ok = feof(file) != 0;
      break;
    }

    if (header.magic != ZLAY_JOURNAL_RECORD_MAGIC || header.version != ZLAY_JOURNAL_RECORD_VERSION) {
      ok = false;
      break;
    }

    memset(&change, 0, sizeof(change));
    change.sequence = header.sequence;
    change.timestamp_ms = header.timestamp_ms;
    change.target_id.hash = header.target_hash;
    change.flags = header.flags;

    ok = ZLay__JournalReadString(file, header.field_length, &change.field) &&
         ZLay__JournalReadString(file, header.before_length, &change.before_value) &&
         ZLay__JournalReadString(file, header.after_length, &change.after_value);

    if (ok) {
      ok = replay(&change, user);
    }

    ZLay__JournalFreeString(change.field);
    ZLay__JournalFreeString(change.before_value);
    ZLay__JournalFreeString(change.after_value);

    if (!ok) {
      break;
    }
  }

  fclose(file);
  return ok;
}

bool ZLay_JournalClearFile(const char* path) {
  if (path == NULL) {
    return false;
  }

  return remove(path) == 0;
}
