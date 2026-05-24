#ifndef ZLAY_OS_SYSTEM_ZLAY_MUTEX_H
#define ZLAY_OS_SYSTEM_ZLAY_MUTEX_H

#ifdef __cplusplus
extern "C" {
#endif

#include <zlay.h>

typedef struct ZLay_Mutex ZLay_Mutex;

ZLAY_API ZLay_Mutex* ZLay_MutexCreate(void);
ZLAY_API void ZLay_MutexDestroy(ZLay_Mutex* mutex);
ZLAY_API void ZLay_MutexLock(ZLay_Mutex* mutex);
ZLAY_API void ZLay_MutexUnlock(ZLay_Mutex* mutex);

#ifdef __cplusplus
}
#endif

#endif
