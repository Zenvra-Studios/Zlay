#ifndef ZLAY_OS_SYSTEM_ZLAY_FILE_H
#define ZLAY_OS_SYSTEM_ZLAY_FILE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stddef.h>

#include <zlay.h>

typedef struct ZLay_FileData {
  void* data;
  size_t size;
} ZLay_FileData;

ZLAY_API bool ZLay_ReadFile(const char* path, ZLay_FileData* out_file);
ZLAY_API bool ZLay_WriteFile(const char* path, const void* data, size_t size);
ZLAY_API void ZLay_FreeFileData(ZLay_FileData* file);

#ifdef __cplusplus
}
#endif

#endif
