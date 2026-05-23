#pragma once

#include <zlay.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef ZLAY_SESSION_ID_CAPACITY
#  define ZLAY_SESSION_ID_CAPACITY 64u
#endif

#ifndef ZLAY_SESSION_PATH_CAPACITY
#  define ZLAY_SESSION_PATH_CAPACITY 260u
#endif

typedef enum ZLay_SessionState {
  ZLAY_SESSION_CLOSED = 0,
  ZLAY_SESSION_ACTIVE = 1,
  ZLAY_SESSION_SUSPENDED = 2,
  ZLAY_SESSION_RESTORED = 3
} ZLay_SessionState;

typedef struct ZLay_Session {
  char id[ZLAY_SESSION_ID_CAPACITY];
  char recovery_path[ZLAY_SESSION_PATH_CAPACITY];
  uint64_t revision;
  ZLay_SessionState state;
  bool dirty;
} ZLay_Session;

ZLAY_API void ZLay_SessionInit(ZLay_Session* session, const char* id, const char* recovery_path);
ZLAY_API void ZLay_SessionBegin(ZLay_Session* session);
ZLAY_API void ZLay_SessionMarkDirty(ZLay_Session* session);
ZLAY_API void ZLay_SessionMarkClean(ZLay_Session* session);
ZLAY_API void ZLay_SessionSuspend(ZLay_Session* session);
ZLAY_API void ZLay_SessionResume(ZLay_Session* session);
ZLAY_API void ZLay_SessionMarkRestored(ZLay_Session* session, uint64_t revision);
ZLAY_API void ZLay_SessionEnd(ZLay_Session* session);

#ifdef __cplusplus
}
#endif
