#pragma once

#include <zlay.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ZLay_ArenaMarker {
  size_t offset;
} ZLay_ArenaMarker;

ZLAY_API ZLay_ArenaMarker ZLay_ArenaMark(const ZLay_Arena* arena);
ZLAY_API void ZLay_ArenaRestore(ZLay_Arena* arena, ZLay_ArenaMarker marker);
ZLAY_API size_t ZLay_ArenaRemaining(const ZLay_Arena* arena);
ZLAY_API bool ZLay_ArenaCanAlloc(const ZLay_Arena* arena, size_t size, size_t alignment);
ZLAY_API bool ZLay_ArenaContains(const ZLay_Arena* arena, const void* ptr);
ZLAY_API void* ZLay_ArenaPushZero(ZLay_Arena* arena, size_t size, size_t alignment);

#ifdef __cplusplus
}
#endif
