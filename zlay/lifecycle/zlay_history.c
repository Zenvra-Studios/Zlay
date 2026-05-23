#include <lifecycle/zlay_history.h>

#include <string.h>

void ZLay_HistoryInit(ZLay_History* history, ZLay_HistoryEntry* entries, uint32_t capacity) {
  if (history == NULL) {
    return;
  }

  history->entries = entries;
  history->capacity = capacity;
  history->count = 0u;
  history->cursor = 0u;
  history->next_sequence = 1u;

  if (entries != NULL && capacity > 0u) {
    memset(entries, 0, sizeof(entries[0]) * (size_t)capacity);
  }
}

void ZLay_HistoryClear(ZLay_History* history) {
  if (history == NULL) {
    return;
  }

  if (history->entries != NULL && history->capacity > 0u) {
    memset(history->entries, 0, sizeof(history->entries[0]) * (size_t)history->capacity);
  }

  history->count = 0u;
  history->cursor = 0u;
  history->next_sequence = 1u;
}

bool ZLay_HistoryPush(ZLay_History* history, ZLay_HistoryEntry entry) {
  if (history == NULL || history->entries == NULL || history->capacity == 0u) {
    return false;
  }

  if (history->cursor < history->count) {
    history->count = history->cursor;
  }

  if (entry.sequence == 0u) {
    entry.sequence = history->next_sequence++;
  } else if (history->next_sequence <= entry.sequence) {
    history->next_sequence = entry.sequence + 1u;
  }

  if (history->count == history->capacity) {
    memmove(&history->entries[0], &history->entries[1], sizeof(history->entries[0]) * (size_t)(history->capacity - 1u));
    history->count = history->capacity - 1u;
    if (history->cursor > 0u) {
      --history->cursor;
    }
  }

  history->entries[history->count++] = entry;
  history->cursor = history->count;
  return true;
}

bool ZLay_HistoryCanUndo(const ZLay_History* history) {
  return history != NULL && history->cursor > 0u;
}

bool ZLay_HistoryCanRedo(const ZLay_History* history) {
  return history != NULL && history->cursor < history->count;
}

bool ZLay_HistoryUndo(ZLay_History* history, ZLay_HistoryEntry* out_entry) {
  if (!ZLay_HistoryCanUndo(history)) {
    return false;
  }

  --history->cursor;
  if (out_entry != NULL) {
    *out_entry = history->entries[history->cursor];
  }
  return true;
}

bool ZLay_HistoryRedo(ZLay_History* history, ZLay_HistoryEntry* out_entry) {
  if (!ZLay_HistoryCanRedo(history)) {
    return false;
  }

  if (out_entry != NULL) {
    *out_entry = history->entries[history->cursor];
  }
  ++history->cursor;
  return true;
}
