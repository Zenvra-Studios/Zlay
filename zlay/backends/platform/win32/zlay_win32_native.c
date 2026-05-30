#include <backends/platform/win32/zlay_win32_backend.h>

typedef struct ZLay_Win32_MonitorCountState {
  uint32_t count;
} ZLay_Win32_MonitorCountState;

typedef struct ZLay_Win32_MonitorInfoState {
  uint32_t target;
  uint32_t index;
  ZLay_MonitorInfo* info;
  bool found;
} ZLay_Win32_MonitorInfoState;

static char zlay_win32_monitor_names[16][CCHDEVICENAME];

static HCURSOR ZLay_Win32_LoadCursor(ZLay_Cursor cursor) {
  switch (cursor) {
    case ZLAY_CURSOR_HAND: return LoadCursorW(NULL, IDC_HAND);
    case ZLAY_CURSOR_TEXT: return LoadCursorW(NULL, IDC_IBEAM);
    case ZLAY_CURSOR_RESIZE_HORIZONTAL: return LoadCursorW(NULL, IDC_SIZEWE);
    case ZLAY_CURSOR_RESIZE_VERTICAL: return LoadCursorW(NULL, IDC_SIZENS);
    case ZLAY_CURSOR_CROSSHAIR: return LoadCursorW(NULL, IDC_CROSS);
    case ZLAY_CURSOR_HIDDEN: return NULL;
    case ZLAY_CURSOR_ARROW:
    default: return LoadCursorW(NULL, IDC_ARROW);
  }
}

void ZLay_Win32_WindowSetCursor(ZLay_Window* window, ZLay_Cursor cursor) {
  HCURSOR native_cursor;

  if (window == NULL) {
    return;
  }

  native_cursor = ZLay_Win32_LoadCursor(cursor);
  SetCursor(native_cursor);
}

static BOOL CALLBACK ZLay_Win32_CountMonitor(HMONITOR monitor, HDC dc, LPRECT rect, LPARAM user) {
  ZLay_Win32_MonitorCountState* state = (ZLay_Win32_MonitorCountState*)user;
  (void)monitor;
  (void)dc;
  (void)rect;
  state->count += 1u;
  return TRUE;
}

uint32_t ZLay_Win32_GetMonitorCount(void) {
  ZLay_Win32_MonitorCountState state;

  state.count = 0u;
  EnumDisplayMonitors(NULL, NULL, ZLay_Win32_CountMonitor, (LPARAM)&state);
  return state.count;
}

static BOOL CALLBACK ZLay_Win32_InfoMonitor(HMONITOR monitor, HDC dc, LPRECT rect, LPARAM user) {
  MONITORINFOEXA info;
  ZLay_Win32_MonitorInfoState* state = (ZLay_Win32_MonitorInfoState*)user;
  uint32_t name_index;
  (void)dc;
  (void)rect;

  if (state->index++ != state->target) {
    return TRUE;
  }

  info.cbSize = sizeof(info);
  if (GetMonitorInfoA(monitor, (MONITORINFO*)&info)) {
    name_index = state->target;
    if (name_index >= 16u) {
      name_index = 15u;
    }
    lstrcpynA(zlay_win32_monitor_names[name_index], info.szDevice, CCHDEVICENAME);

    state->info->x = info.rcMonitor.left;
    state->info->y = info.rcMonitor.top;
    state->info->width = (uint32_t)(info.rcMonitor.right - info.rcMonitor.left);
    state->info->height = (uint32_t)(info.rcMonitor.bottom - info.rcMonitor.top);
    state->info->scale_x = 1.0f;
    state->info->scale_y = 1.0f;
    state->info->refresh_rate = 0u;
    state->info->name = zlay_win32_monitor_names[name_index];
    state->found = true;
  }

  return FALSE;
}

bool ZLay_Win32_GetMonitorInfo(uint32_t index, ZLay_MonitorInfo* out_info) {
  ZLay_Win32_MonitorInfoState state;

  if (out_info == NULL) {
    return false;
  }

  state.target = index;
  state.index = 0u;
  state.info = out_info;
  state.found = false;
  *out_info = (ZLay_MonitorInfo){0};
  EnumDisplayMonitors(NULL, NULL, ZLay_Win32_InfoMonitor, (LPARAM)&state);
  return state.found;
}
