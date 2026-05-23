#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <event/zlay_add_eventlistener.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef ZLAY_HTTP_MAX_HEADERS
#  define ZLAY_HTTP_MAX_HEADERS 24u
#endif

#define ZLAY_EVT_NETWORK_PROGRESS ZLAY_EVENT_STRING_LITERAL("networkprogress")
#define ZLAY_EVT_NETWORK_ABORT ZLAY_EVENT_STRING_LITERAL("networkabort")
#define ZLAY_EVT_NETWORK_STATE ZLAY_EVENT_STRING_LITERAL("networkstate")

typedef enum ZLay_HTTPMethod {
  ZLAY_HTTP_GET = 0,
  ZLAY_HTTP_POST,
  ZLAY_HTTP_PUT,
  ZLAY_HTTP_PATCH,
  ZLAY_HTTP_DELETE,
  ZLAY_HTTP_HEAD,
  ZLAY_HTTP_OPTIONS,
  ZLAY_HTTP_CUSTOM
} ZLay_HTTPMethod;

typedef enum ZLay_HTTPReadyState {
  ZLAY_HTTP_READY_IDLE = 0,
  ZLAY_HTTP_READY_OPEN,
  ZLAY_HTTP_READY_HEADERS,
  ZLAY_HTTP_READY_STREAMING,
  ZLAY_HTTP_READY_DONE
} ZLay_HTTPReadyState;

typedef enum ZLay_HTTPResponseType {
  ZLAY_HTTP_RESPONSE_TEXT = 0,
  ZLAY_HTTP_RESPONSE_BINARY,
  ZLAY_HTTP_RESPONSE_JSON
} ZLay_HTTPResponseType;

typedef struct ZLay_HTTPHeader {
  ZLay_EventString name;
  ZLay_EventString value;
} ZLay_HTTPHeader;

typedef struct ZLay_HTTPRequest {
  uint32_t id;
  ZLay_HTTPMethod method;
  ZLay_EventString custom_method;
  ZLay_EventString url;
  ZLay_HTTPHeader headers[ZLAY_HTTP_MAX_HEADERS];
  uint32_t header_count;
  const void* body;
  size_t body_size;
  double timeout_ms;
  ZLay_HTTPResponseType response_type;
  ZLay_RenderNode* target;
  void* user;
} ZLay_HTTPRequest;

typedef struct ZLay_HTTPResponse {
  uint32_t request_id;
  uint16_t status;
  ZLay_EventString status_text;
  ZLay_EventString url;
  ZLay_HTTPHeader headers[ZLAY_HTTP_MAX_HEADERS];
  uint32_t header_count;
  const void* body;
  size_t body_size;
  size_t loaded;
  size_t total;
  ZLay_EventString error;
  ZLay_RenderNode* target;
  bool ok;
  bool done;
} ZLay_HTTPResponse;

typedef struct ZLay_HTTPClient ZLay_HTTPClient;

typedef bool (*ZLay_HTTPClientSendFn)(ZLay_HTTPClient* client, const ZLay_HTTPRequest* request);
typedef void (*ZLay_HTTPClientAbortFn)(ZLay_HTTPClient* client, uint32_t request_id);
typedef void (*ZLay_HTTPTransferCallback)(const ZLay_HTTPResponse* response, void* user);

struct ZLay_HTTPClient {
  ZLay_HTTPClientSendFn send;
  ZLay_HTTPClientAbortFn abort;
  uint32_t next_request_id;
  void* user;
};

typedef struct ZLay_HTTPTransfer {
  ZLay_HTTPClient* client;
  ZLay_HTTPRequest request;
  ZLay_HTTPResponse response;
  ZLay_HTTPReadyState ready_state;
  ZLay_HTTPTransferCallback on_state;
  ZLay_HTTPTransferCallback on_load;
  ZLay_HTTPTransferCallback on_error;
  ZLay_HTTPTransferCallback on_progress;
  void* user;
  bool opened;
  bool sent;
  bool aborted;
} ZLay_HTTPTransfer;

static inline const char* ZLay_HTTPMethodName(ZLay_HTTPMethod method) {
  switch (method) {
    case ZLAY_HTTP_GET:
      return "GET";
    case ZLAY_HTTP_POST:
      return "POST";
    case ZLAY_HTTP_PUT:
      return "PUT";
    case ZLAY_HTTP_PATCH:
      return "PATCH";
    case ZLAY_HTTP_DELETE:
      return "DELETE";
    case ZLAY_HTTP_HEAD:
      return "HEAD";
    case ZLAY_HTTP_OPTIONS:
      return "OPTIONS";
    case ZLAY_HTTP_CUSTOM:
    default:
      return "CUSTOM";
  }
}

static inline bool ZLay_HTTPStatusOK(uint16_t status) {
  return status >= 200u && status < 300u;
}

static inline void ZLay_HTTPClientInit(
  ZLay_HTTPClient* client,
  ZLay_HTTPClientSendFn send,
  ZLay_HTTPClientAbortFn abort,
  void* user
) {
  if (client == NULL) {
    return;
  }

  memset(client, 0, sizeof(*client));
  client->send = send;
  client->abort = abort;
  client->next_request_id = 1u;
  client->user = user;
}

static inline void ZLay_RenderSceneSetNetworkClient(
  ZLay_RenderScene* scene,
  ZLay_HTTPClient* client
) {
  if (scene != NULL) {
    scene->network_client = client;
  }
}

static inline void ZLay_HTTPRequestInit(
  ZLay_HTTPRequest* request,
  ZLay_HTTPMethod method,
  ZLay_EventString url
) {
  if (request == NULL) {
    return;
  }

  memset(request, 0, sizeof(*request));
  request->method = method;
  request->url = url;
  request->timeout_ms = 30000.0;
  request->response_type = ZLAY_HTTP_RESPONSE_TEXT;
}

static inline bool ZLay_HTTPRequestAddHeader(
  ZLay_HTTPRequest* request,
  ZLay_EventString name,
  ZLay_EventString value
) {
  if (request == NULL || ZLay_EventStringIsEmpty(name) || request->header_count >= ZLAY_HTTP_MAX_HEADERS) {
    return false;
  }

  request->headers[request->header_count].name = name;
  request->headers[request->header_count].value = value;
  ++request->header_count;
  return true;
}

static inline ZLay_EventString ZLay_HTTPRequestHeader(
  const ZLay_HTTPRequest* request,
  ZLay_EventString name
) {
  if (request == NULL) {
    return ZLay_EventStringEmpty();
  }

  for (uint32_t i = 0u; i < request->header_count; ++i) {
    if (ZLay_EventStringEquals(request->headers[i].name, name)) {
      return request->headers[i].value;
    }
  }

  return ZLay_EventStringEmpty();
}

static inline void ZLay_HTTPResponseInit(
  ZLay_HTTPResponse* response,
  uint32_t request_id,
  uint16_t status,
  ZLay_EventString url
) {
  if (response == NULL) {
    return;
  }

  memset(response, 0, sizeof(*response));
  response->request_id = request_id;
  response->status = status;
  response->url = url;
  response->ok = ZLay_HTTPStatusOK(status);
}

static inline bool ZLay_HTTPResponseAddHeader(
  ZLay_HTTPResponse* response,
  ZLay_EventString name,
  ZLay_EventString value
) {
  if (response == NULL || ZLay_EventStringIsEmpty(name) || response->header_count >= ZLAY_HTTP_MAX_HEADERS) {
    return false;
  }

  response->headers[response->header_count].name = name;
  response->headers[response->header_count].value = value;
  ++response->header_count;
  return true;
}

static inline ZLay_EventString ZLay_HTTPResponseHeader(
  const ZLay_HTTPResponse* response,
  ZLay_EventString name
) {
  if (response == NULL) {
    return ZLay_EventStringEmpty();
  }

  for (uint32_t i = 0u; i < response->header_count; ++i) {
    if (ZLay_EventStringEquals(response->headers[i].name, name)) {
      return response->headers[i].value;
    }
  }

  return ZLay_EventStringEmpty();
}

static inline uint32_t ZLay_HTTPClientSend(ZLay_HTTPClient* client, ZLay_HTTPRequest* request) {
  if (client == NULL || request == NULL || client->send == NULL) {
    return 0u;
  }

  if (request->id == 0u) {
    request->id = client->next_request_id++;
  }

  return client->send(client, request) ? request->id : 0u;
}

static inline void ZLay_HTTPClientAbort(ZLay_HTTPClient* client, uint32_t request_id) {
  if (client != NULL && client->abort != NULL && request_id != 0u) {
    client->abort(client, request_id);
  }
}

static inline bool ZLay_HTTPDispatchResponse(
  ZLay_RenderNode* target,
  ZLay_EventString event_type,
  const ZLay_HTTPResponse* response
) {
  ZLay_RenderEvent event;

  if (target == NULL || response == NULL) {
    return false;
  }

  ZLay_RenderEventInit(&event, event_type, ZLAY_RENDER_EVENT_ASCENDS, (void*)response);
  return ZLay_RenderDispatchEvent(target, &event);
}

static inline uint32_t ZLay_HTTPSceneRequest(
  ZLay_RenderScene* scene,
  ZLay_RenderNode* target,
  ZLay_HTTPMethod method,
  ZLay_EventString url,
  const void* body,
  size_t body_size
) {
  ZLay_HTTPClient* client;
  ZLay_HTTPRequest request;

  if (scene == NULL || scene->network_client == NULL) {
    return 0u;
  }

  client = (ZLay_HTTPClient*)scene->network_client;
  ZLay_HTTPRequestInit(&request, method, url);
  request.target = target;
  request.body = body;
  request.body_size = body_size;
  return ZLay_HTTPClientSend(client, &request);
}

static inline void ZLay_HTTPTransferInit(
  ZLay_HTTPTransfer* transfer,
  ZLay_HTTPClient* client,
  ZLay_RenderNode* target
) {
  if (transfer == NULL) {
    return;
  }

  memset(transfer, 0, sizeof(*transfer));
  transfer->client = client;
  transfer->request.target = target;
  transfer->ready_state = ZLAY_HTTP_READY_IDLE;
}

static inline void ZLay_HTTPTransferOpen(
  ZLay_HTTPTransfer* transfer,
  ZLay_HTTPMethod method,
  ZLay_EventString url
) {
  ZLay_RenderNode* target;

  if (transfer == NULL) {
    return;
  }

  target = transfer->request.target;
  ZLay_HTTPRequestInit(&transfer->request, method, url);
  transfer->request.target = target;
  transfer->ready_state = ZLAY_HTTP_READY_OPEN;
  transfer->opened = true;
}

static inline bool ZLay_HTTPTransferSetHeader(
  ZLay_HTTPTransfer* transfer,
  ZLay_EventString name,
  ZLay_EventString value
) {
  return transfer != NULL && ZLay_HTTPRequestAddHeader(&transfer->request, name, value);
}

static inline void ZLay_HTTPTransferNotify(
  ZLay_HTTPTransfer* transfer,
  ZLay_EventString event_type,
  ZLay_HTTPTransferCallback callback
) {
  if (transfer == NULL) {
    return;
  }

  if (callback != NULL) {
    callback(&transfer->response, transfer->user);
  }

  ZLay_HTTPDispatchResponse(transfer->request.target, event_type, &transfer->response);
}

static inline uint32_t ZLay_HTTPTransferSend(
  ZLay_HTTPTransfer* transfer,
  const void* body,
  size_t body_size
) {
  uint32_t id;

  if (transfer == NULL || transfer->client == NULL || !transfer->opened) {
    return 0u;
  }

  transfer->request.body = body;
  transfer->request.body_size = body_size;
  transfer->request.user = transfer;
  id = ZLay_HTTPClientSend(transfer->client, &transfer->request);
  if (id == 0u) {
    transfer->ready_state = ZLAY_HTTP_READY_DONE;
    ZLay_HTTPTransferNotify(transfer, ZLAY_EVT_NETWORK_ERROR, transfer->on_error);
    return 0u;
  }

  transfer->sent = true;
  transfer->ready_state = ZLAY_HTTP_READY_STREAMING;
  ZLay_HTTPTransferNotify(transfer, ZLAY_EVT_NETWORK_STATE, transfer->on_state);
  return id;
}

static inline void ZLay_HTTPTransferProgress(
  ZLay_HTTPTransfer* transfer,
  size_t loaded,
  size_t total
) {
  if (transfer == NULL || transfer->aborted) {
    return;
  }

  transfer->response.loaded = loaded;
  transfer->response.total = total;
  transfer->ready_state = ZLAY_HTTP_READY_STREAMING;
  ZLay_HTTPTransferNotify(transfer, ZLAY_EVT_NETWORK_PROGRESS, transfer->on_progress);
}

static inline void ZLay_HTTPTransferReceive(
  ZLay_HTTPTransfer* transfer,
  const ZLay_HTTPResponse* response
) {
  if (transfer == NULL || response == NULL || transfer->aborted) {
    return;
  }

  transfer->response = *response;
  transfer->response.target = transfer->request.target;
  transfer->response.done = true;
  transfer->ready_state = ZLAY_HTTP_READY_DONE;
  ZLay_HTTPTransferNotify(transfer, ZLAY_EVT_NETWORK_STATE, transfer->on_state);
  ZLay_HTTPTransferNotify(
    transfer,
    transfer->response.ok ? ZLAY_EVT_NETWORK_LOAD : ZLAY_EVT_NETWORK_ERROR,
    transfer->response.ok ? transfer->on_load : transfer->on_error
  );
}

static inline void ZLay_HTTPTransferAbort(ZLay_HTTPTransfer* transfer) {
  if (transfer == NULL || transfer->client == NULL || transfer->request.id == 0u) {
    return;
  }

  transfer->aborted = true;
  ZLay_HTTPClientAbort(transfer->client, transfer->request.id);
  transfer->ready_state = ZLAY_HTTP_READY_DONE;
  ZLay_HTTPTransferNotify(transfer, ZLAY_EVT_NETWORK_ABORT, transfer->on_error);
}

#ifdef __cplusplus
}
#endif
