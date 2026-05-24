#include <os/system/zlay_path.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(_WIN32)
#  define WIN32_LEAN_AND_MEAN
#  include <windows.h>
#else
#  include <limits.h>
#  include <unistd.h>
#endif

#ifndef ZLAY_PATH_BUFFER_SIZE
#  define ZLAY_PATH_BUFFER_SIZE 4096u
#endif

static char zlay_executable_path[ZLAY_PATH_BUFFER_SIZE];
static char zlay_config_path[ZLAY_PATH_BUFFER_SIZE];
static char zlay_cache_path[ZLAY_PATH_BUFFER_SIZE];
static char zlay_temp_path[ZLAY_PATH_BUFFER_SIZE];

static const char* ZLay_PathFallback(const char* value, const char* fallback) {
  return value != NULL && value[0] != '\0' ? value : fallback;
}

static const char* ZLay_CopyPath(char* buffer, size_t capacity, const char* value) {
  if (capacity == 0u) {
    return "";
  }

  if (value == NULL) {
    value = "";
  }

  strncpy(buffer, value, capacity - 1u);
  buffer[capacity - 1u] = '\0';
  return buffer;
}

const char* ZLay_GetExecutablePath(void) {
  if (zlay_executable_path[0] != '\0') {
    return zlay_executable_path;
  }

#if defined(_WIN32)
  {
    DWORD length = GetModuleFileNameA(NULL, zlay_executable_path, (DWORD)sizeof(zlay_executable_path));
    if (length == 0u || length >= sizeof(zlay_executable_path)) {
      zlay_executable_path[0] = '\0';
    }
  }
#elif defined(__linux__)
  {
    ssize_t length = readlink("/proc/self/exe", zlay_executable_path, sizeof(zlay_executable_path) - 1u);
    if (length >= 0) {
      zlay_executable_path[length] = '\0';
    }
  }
#endif

  return zlay_executable_path;
}

const char* ZLay_GetConfigPath(void) {
  const char* path;

  if (zlay_config_path[0] != '\0') {
    return zlay_config_path;
  }

#if defined(_WIN32)
  path = ZLay_PathFallback(getenv("APPDATA"), ".");
#else
  path = ZLay_PathFallback(getenv("XDG_CONFIG_HOME"), NULL);
  if (path == NULL) {
    const char* home = ZLay_PathFallback(getenv("HOME"), ".");
    snprintf(zlay_config_path, sizeof(zlay_config_path), "%s/.config", home);
    return zlay_config_path;
  }
#endif

  return ZLay_CopyPath(zlay_config_path, sizeof(zlay_config_path), path);
}

const char* ZLay_GetCachePath(void) {
  const char* path;

  if (zlay_cache_path[0] != '\0') {
    return zlay_cache_path;
  }

#if defined(_WIN32)
  path = ZLay_PathFallback(getenv("LOCALAPPDATA"), ZLay_GetConfigPath());
#else
  path = ZLay_PathFallback(getenv("XDG_CACHE_HOME"), NULL);
  if (path == NULL) {
    const char* home = ZLay_PathFallback(getenv("HOME"), ".");
    snprintf(zlay_cache_path, sizeof(zlay_cache_path), "%s/.cache", home);
    return zlay_cache_path;
  }
#endif

  return ZLay_CopyPath(zlay_cache_path, sizeof(zlay_cache_path), path);
}

const char* ZLay_GetTempPath(void) {
  const char* path;

  if (zlay_temp_path[0] != '\0') {
    return zlay_temp_path;
  }

#if defined(_WIN32)
  {
    DWORD length = GetTempPathA((DWORD)sizeof(zlay_temp_path), zlay_temp_path);
    if (length > 0u && length < sizeof(zlay_temp_path)) {
      return zlay_temp_path;
    }
  }
  path = ".";
#else
  path = ZLay_PathFallback(getenv("TMPDIR"), "/tmp");
#endif

  return ZLay_CopyPath(zlay_temp_path, sizeof(zlay_temp_path), path);
}
