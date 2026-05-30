#include <backends/platform/gtk/zlay_gtk_backend.h>

#include <stdlib.h>
#include <string.h>

static char* ZLay_GTK_StringDuplicate(const char* text) {
  size_t length;
  char* copy;

  if (text == NULL) {
    text = "";
  }

  length = strlen(text);
  copy = (char*)malloc(length + 1u);
  if (copy == NULL) {
    return NULL;
  }

  memcpy(copy, text, length + 1u);
  return copy;
}

bool ZLay_GTK_ClipboardSetText(const char* text) {
  ZLay_GTK_State* state = ZLay_GTK_StateGet();
  char* copy;

  if (state->clipboard_set_text != NULL) {
    return state->clipboard_set_text(text != NULL ? text : "", state->user);
  }

  copy = ZLay_GTK_StringDuplicate(text);
  if (copy == NULL) {
    return false;
  }

  free(state->clipboard_text);
  state->clipboard_text = copy;
  return true;
}

const char* ZLay_GTK_ClipboardGetText(void) {
  ZLay_GTK_State* state = ZLay_GTK_StateGet();

  if (state->clipboard_get_text != NULL) {
    const char* text = state->clipboard_get_text(state->user);
    return text != NULL ? text : "";
  }

  return state->clipboard_text != NULL ? state->clipboard_text : "";
}
