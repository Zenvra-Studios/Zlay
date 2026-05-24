#include <os/system/zlay_thread.h>

#include <stdlib.h>

#if defined(_WIN32)
#  define WIN32_LEAN_AND_MEAN
#  include <windows.h>
#else
#  include <pthread.h>
#endif

struct ZLay_Thread {
  ZLay_ThreadFn fn;
  void* user;
  int result;
  bool joined;
#if defined(_WIN32)
  HANDLE handle;
#else
  pthread_t handle;
#endif
};

#if defined(_WIN32)
static DWORD WINAPI ZLay_ThreadEntry(LPVOID user) {
  ZLay_Thread* thread = (ZLay_Thread*)user;
  thread->result = thread->fn != NULL ? thread->fn(thread->user) : 0;
  return (DWORD)thread->result;
}
#else
static void* ZLay_ThreadEntry(void* user) {
  ZLay_Thread* thread = (ZLay_Thread*)user;
  thread->result = thread->fn != NULL ? thread->fn(thread->user) : 0;
  return NULL;
}
#endif

ZLay_Thread* ZLay_ThreadCreate(ZLay_ThreadFn fn, void* user) {
  ZLay_Thread* thread;

  if (fn == NULL) {
    return NULL;
  }

  thread = (ZLay_Thread*)calloc(1u, sizeof(ZLay_Thread));
  if (thread == NULL) {
    return NULL;
  }

  thread->fn = fn;
  thread->user = user;

#if defined(_WIN32)
  thread->handle = CreateThread(NULL, 0u, ZLay_ThreadEntry, thread, 0u, NULL);
  if (thread->handle == NULL) {
    free(thread);
    return NULL;
  }
#else
  if (pthread_create(&thread->handle, NULL, ZLay_ThreadEntry, thread) != 0) {
    free(thread);
    return NULL;
  }
#endif

  return thread;
}

void ZLay_ThreadJoin(ZLay_Thread* thread) {
  if (thread == NULL || thread->joined) {
    return;
  }

#if defined(_WIN32)
  WaitForSingleObject(thread->handle, INFINITE);
#else
  pthread_join(thread->handle, NULL);
#endif
  thread->joined = true;
}

void ZLay_ThreadDestroy(ZLay_Thread* thread) {
  if (thread == NULL) {
    return;
  }

  ZLay_ThreadJoin(thread);
#if defined(_WIN32)
  CloseHandle(thread->handle);
#endif
  free(thread);
}
