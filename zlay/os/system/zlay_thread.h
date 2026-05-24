#ifndef ZLAY_OS_SYSTEM_ZLAY_THREAD_H
#define ZLAY_OS_SYSTEM_ZLAY_THREAD_H

#ifdef __cplusplus
extern "C" {
#endif

#include <zlay.h>

typedef struct ZLay_Thread ZLay_Thread;
typedef int (*ZLay_ThreadFn)(void* user);

ZLAY_API ZLay_Thread* ZLay_ThreadCreate(ZLay_ThreadFn fn, void* user);
ZLAY_API void ZLay_ThreadJoin(ZLay_Thread* thread);
ZLAY_API void ZLay_ThreadDestroy(ZLay_Thread* thread);

#ifdef __cplusplus
}
#endif

#endif
