#include <lifecycle/zlay_session.h>

#include <string.h>

static void ZLay__CopyCString(char* dst, size_t capacity, const char* src) {
  size_t i = 0u;

  if (dst == NULL || capacity == 0u) {
    return;
  }

  if (src != NULL) {
    for (; i + 1u < capacity && src[i] != '\0'; ++i) {
      dst[i] = src[i];
    }
  }

  dst[i] = '\0';
}

void ZLay_SessionInit(ZLay_Session* session, const char* id, const char* recovery_path) {
  if (session == NULL) {
    return;
  }

  memset(session, 0, sizeof(*session));
  ZLay__CopyCString(session->id, sizeof(session->id), id != NULL ? id : "default");
  ZLay__CopyCString(session->recovery_path, sizeof(session->recovery_path), recovery_path);
  session->state = ZLAY_SESSION_CLOSED;
}

void ZLay_SessionBegin(ZLay_Session* session) {
  if (session == NULL) {
    return;
  }

  session->state = ZLAY_SESSION_ACTIVE;
}

void ZLay_SessionMarkDirty(ZLay_Session* session) {
  if (session == NULL) {
    return;
  }

  session->dirty = true;
  ++session->revision;
}

void ZLay_SessionMarkClean(ZLay_Session* session) {
  if (session == NULL) {
    return;
  }

  session->dirty = false;
}

void ZLay_SessionSuspend(ZLay_Session* session) {
  if (session == NULL || session->state == ZLAY_SESSION_CLOSED) {
    return;
  }

  session->state = ZLAY_SESSION_SUSPENDED;
}

void ZLay_SessionResume(ZLay_Session* session) {
  if (session == NULL || session->state == ZLAY_SESSION_CLOSED) {
    return;
  }

  session->state = ZLAY_SESSION_ACTIVE;
}

void ZLay_SessionMarkRestored(ZLay_Session* session, uint64_t revision) {
  if (session == NULL) {
    return;
  }

  session->revision = revision;
  session->dirty = false;
  session->state = ZLAY_SESSION_RESTORED;
}

void ZLay_SessionEnd(ZLay_Session* session) {
  if (session == NULL) {
    return;
  }

  session->state = ZLAY_SESSION_CLOSED;
  session->dirty = false;
}
