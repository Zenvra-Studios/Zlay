#ifndef ZLAY_OS_SYSTEM_ZLAY_CLIPBOARD_H
#define ZLAY_OS_SYSTEM_ZLAY_CLIPBOARD_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

#include <zlay.h>

ZLAY_API bool ZLay_ClipboardSetText(const char* text);
ZLAY_API const char* ZLay_ClipboardGetText(void);

#ifdef __cplusplus
}
#endif

#endif
