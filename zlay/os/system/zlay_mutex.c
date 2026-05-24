#include <os/system/zlay_mutex.h>

#include <stdlib.h>

#if defined(_WIN32)
#  define WIN32_LEAN_AND_MEAN
#  include <windows.h>
#else
#  include <pthread.h>
#endif

struct ZLay_Mutex {
#if defined(_WIN32)
  CRITICAL_SECTION handle;
#else
  pthread_mutex_t handle;
#endif
};

ZLay_Mutex* ZLay_MutexCreate(void) {
  ZLay_Mutex* mutex = (ZLay_Mutex*)calloc(1u, sizeof(ZLay_Mutex));
  if (mutex == NULL) {
    return NULL;
  }

#if defined(_WIN32)
  InitializeCriticalSection(&mutex->handle);
#else
  if (pthread_mutex_init(&mutex->handle, NULL) != 0) {
    free(mutex);
    return NULL;
  }
#endif

  return mutex;
}

void ZLay_MutexDestroy(ZLay_Mutex* mutex) {
  if (mutex == NULL) {
    return;
  }

#if defined(_WIN32)
  DeleteCriticalSection(&mutex->handle);
#else
  pthread_mutex_destroy(&mutex->handle);
#endif
  free(mutex);
}

void ZLay_MutexLock(ZLay_Mutex* mutex) {
  if (mutex == NULL) {
    return;
  }

#if defined(_WIN32)
  EnterCriticalSection(&mutex->handle);
#else
  pthread_mutex_lock(&mutex->handle);
#endif
}

void ZLay_MutexUnlock(ZLay_Mutex* mutex) {
  if (mutex == NULL) {
    return;
  }

#if defined(_WIN32)
  LeaveCriticalSection(&mutex->handle);
#else
  pthread_mutex_unlock(&mutex->handle);
#endif
}
