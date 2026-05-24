#include <os/win32/zlay_win32_backend.h>

#include <stdlib.h>
#include <string.h>

static char* zlay_win32_clipboard_text;

static char* ZLay_Win32_DuplicateText(const char* text) {
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

bool ZLay_Win32_ClipboardSetText(const char* text) {
  int wide_length;
  HGLOBAL memory;
  WCHAR* wide;
  char* copy;

  copy = ZLay_Win32_DuplicateText(text);
  if (copy == NULL) {
    return false;
  }

  wide_length = MultiByteToWideChar(CP_UTF8, 0, copy, -1, NULL, 0);
  if (wide_length <= 0) {
    free(copy);
    return false;
  }

  memory = GlobalAlloc(GMEM_MOVEABLE, (SIZE_T)wide_length * sizeof(WCHAR));
  if (memory == NULL) {
    free(copy);
    return false;
  }

  wide = (WCHAR*)GlobalLock(memory);
  MultiByteToWideChar(CP_UTF8, 0, copy, -1, wide, wide_length);
  GlobalUnlock(memory);

  if (!OpenClipboard(NULL)) {
    GlobalFree(memory);
    free(copy);
    return false;
  }

  EmptyClipboard();
  SetClipboardData(CF_UNICODETEXT, memory);
  CloseClipboard();

  free(zlay_win32_clipboard_text);
  zlay_win32_clipboard_text = copy;
  return true;
}

const char* ZLay_Win32_ClipboardGetText(void) {
  HANDLE data;
  WCHAR* wide;
  int utf8_length;
  char* utf8;

  if (!OpenClipboard(NULL)) {
    return zlay_win32_clipboard_text != NULL ? zlay_win32_clipboard_text : "";
  }

  data = GetClipboardData(CF_UNICODETEXT);
  if (data == NULL) {
    CloseClipboard();
    return "";
  }

  wide = (WCHAR*)GlobalLock(data);
  if (wide == NULL) {
    CloseClipboard();
    return "";
  }

  utf8_length = WideCharToMultiByte(CP_UTF8, 0, wide, -1, NULL, 0, NULL, NULL);
  if (utf8_length <= 0) {
    GlobalUnlock(data);
    CloseClipboard();
    return "";
  }

  utf8 = (char*)malloc((size_t)utf8_length);
  if (utf8 == NULL) {
    GlobalUnlock(data);
    CloseClipboard();
    return "";
  }

  WideCharToMultiByte(CP_UTF8, 0, wide, -1, utf8, utf8_length, NULL, NULL);
  GlobalUnlock(data);
  CloseClipboard();

  free(zlay_win32_clipboard_text);
  zlay_win32_clipboard_text = utf8;
  return zlay_win32_clipboard_text;
}
