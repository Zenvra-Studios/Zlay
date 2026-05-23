#ifndef ZLAY_ZLOGGER_H
#define ZLAY_ZLOGGER_H

#include <stdbool.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
  Header-only logger for ZLay.

  The public surface intentionally mirrors the practical pieces people reach
  for in fmt/spdlog: level names, compile-time level stripping, runtime level
  filtering, source location, colored console output, file/callback sinks, and
  printf-style format checking where the compiler supports it.
*/

#ifndef ZLAY_LOG_LEVEL_TRACE
#  define ZLAY_LOG_LEVEL_TRACE 0
#endif

#ifndef ZLAY_LOG_LEVEL_DEBUG
#  define ZLAY_LOG_LEVEL_DEBUG 1
#endif

#ifndef ZLAY_LOG_LEVEL_INFO
#  define ZLAY_LOG_LEVEL_INFO 2
#endif

#ifndef ZLAY_LOG_LEVEL_WARN
#  define ZLAY_LOG_LEVEL_WARN 3
#endif

#ifndef ZLAY_LOG_LEVEL_ERROR
#  define ZLAY_LOG_LEVEL_ERROR 4
#endif

#ifndef ZLAY_LOG_LEVEL_CRITICAL
#  define ZLAY_LOG_LEVEL_CRITICAL 5
#endif

#ifndef ZLAY_LOG_LEVEL_OFF
#  define ZLAY_LOG_LEVEL_OFF 6
#endif

#ifndef ZLAY_LOG_DEFAULT_LEVEL
#  if defined(NDEBUG)
#    define ZLAY_LOG_DEFAULT_LEVEL ZLAY_LOG_LEVEL_INFO
#  else
#    define ZLAY_LOG_DEFAULT_LEVEL ZLAY_LOG_LEVEL_DEBUG
#  endif
#endif

#ifndef ZLAY_LOG_ACTIVE_LEVEL
#  define ZLAY_LOG_ACTIVE_LEVEL ZLAY_LOG_DEFAULT_LEVEL
#endif

#ifndef ZLAY_ENABLE_LOGGING
#  define ZLAY_ENABLE_LOGGING 1
#endif

#ifndef ZLAY_LOG_DEFAULT_NAME
#  define ZLAY_LOG_DEFAULT_NAME "zlay"
#endif

#ifndef ZLAY_LOG_MESSAGE_CAPACITY
#  define ZLAY_LOG_MESSAGE_CAPACITY 2048
#endif

#if ZLAY_LOG_MESSAGE_CAPACITY < 64
#  error "ZLAY_LOG_MESSAGE_CAPACITY must be at least 64 bytes"
#endif

#ifndef ZLAY_LOG_USE_MALLOC
#  define ZLAY_LOG_USE_MALLOC 1
#endif

#ifndef ZLAY_LOG_TO_STDERR_LEVEL
#  define ZLAY_LOG_TO_STDERR_LEVEL ZLAY_LOG_LEVEL_WARN
#endif

#ifndef ZLAY_LOG_FLUSH_LEVEL
#  define ZLAY_LOG_FLUSH_LEVEL ZLAY_LOG_LEVEL_ERROR
#endif

#ifndef ZLAY_LOG_PRINTF_FORMAT
#  if defined(__GNUC__) || defined(__clang__)
#    define ZLAY_LOG_PRINTF_FORMAT(format_index, first_arg) __attribute__((format(printf, format_index, first_arg)))
#  else
#    define ZLAY_LOG_PRINTF_FORMAT(format_index, first_arg)
#  endif
#endif

#if !defined(va_copy)
#  if defined(__va_copy)
#    define va_copy(destination, source) __va_copy(destination, source)
#  else
#    define va_copy(destination, source) ((destination) = (source))
#  endif
#endif

typedef uint8_t ZLay_LogLevel;

typedef enum ZLay_LogFlags {
  ZLAY_LOG_FLAG_STDIO = 1u << 0,
  ZLAY_LOG_FLAG_TIME = 1u << 1,
  ZLAY_LOG_FLAG_NAME = 1u << 2,
  ZLAY_LOG_FLAG_LEVEL = 1u << 3,
  ZLAY_LOG_FLAG_LOCATION = 1u << 4,
  ZLAY_LOG_FLAG_FUNCTION = 1u << 5,
  ZLAY_LOG_FLAG_COLOR = 1u << 6,
  ZLAY_LOG_FLAG_FLUSH = 1u << 7,
  ZLAY_LOG_FLAG_SHORT_FILE = 1u << 8
} ZLay_LogFlags;

#ifndef ZLAY_LOG_DEFAULT_FLAGS
#  define ZLAY_LOG_DEFAULT_FLAGS \
    (ZLAY_LOG_FLAG_STDIO | ZLAY_LOG_FLAG_TIME | ZLAY_LOG_FLAG_NAME | \
     ZLAY_LOG_FLAG_LEVEL | ZLAY_LOG_FLAG_LOCATION | ZLAY_LOG_FLAG_COLOR | \
     ZLAY_LOG_FLAG_SHORT_FILE)
#endif

typedef struct ZLay_LogRecord {
  ZLay_LogLevel level;
  const char* level_name;
  const char* logger_name;
  const char* file;
  int line;
  const char* function;
  const char* message;
  size_t message_length;
  const char* time_text;
} ZLay_LogRecord;

typedef void (*ZLay_LogSinkFn)(const ZLay_LogRecord* record, void* user);

typedef struct ZLay_LogConfig {
  ZLay_LogLevel level;
  uint32_t flags;
  FILE* stream;
  FILE* file;
  bool owns_file;
  const char* logger_name;
  ZLay_LogSinkFn sink;
  void* sink_user;
} ZLay_LogConfig;

static inline ZLay_LogConfig ZLay__LogDefaultConfig(void) {
  ZLay_LogConfig config;
  config.level = (ZLay_LogLevel)ZLAY_LOG_DEFAULT_LEVEL;
  config.flags = (uint32_t)ZLAY_LOG_DEFAULT_FLAGS;
  config.stream = NULL;
  config.file = NULL;
  config.owns_file = false;
  config.logger_name = ZLAY_LOG_DEFAULT_NAME;
  config.sink = NULL;
  config.sink_user = NULL;
  return config;
}

static inline ZLay_LogConfig* ZLay_LogGetConfig(void) {
  static ZLay_LogConfig config = {
    (ZLay_LogLevel)ZLAY_LOG_DEFAULT_LEVEL,
    (uint32_t)ZLAY_LOG_DEFAULT_FLAGS,
    NULL,
    NULL,
    false,
    ZLAY_LOG_DEFAULT_NAME,
    NULL,
    NULL
  };
  return &config;
}

static inline const char* ZLay_LogLevelName(ZLay_LogLevel level) {
  switch ((int)level) {
    case ZLAY_LOG_LEVEL_TRACE: return "trace";
    case ZLAY_LOG_LEVEL_DEBUG: return "debug";
    case ZLAY_LOG_LEVEL_INFO: return "info";
    case ZLAY_LOG_LEVEL_WARN: return "warn";
    case ZLAY_LOG_LEVEL_ERROR: return "error";
    case ZLAY_LOG_LEVEL_CRITICAL: return "critical";
    case ZLAY_LOG_LEVEL_OFF: return "off";
    default: return "unknown";
  }
}

static inline const char* ZLay_LogLevelShortName(ZLay_LogLevel level) {
  switch ((int)level) {
    case ZLAY_LOG_LEVEL_TRACE: return "T";
    case ZLAY_LOG_LEVEL_DEBUG: return "D";
    case ZLAY_LOG_LEVEL_INFO: return "I";
    case ZLAY_LOG_LEVEL_WARN: return "W";
    case ZLAY_LOG_LEVEL_ERROR: return "E";
    case ZLAY_LOG_LEVEL_CRITICAL: return "C";
    case ZLAY_LOG_LEVEL_OFF: return "O";
    default: return "?";
  }
}

static inline const char* ZLay__LogLevelColor(ZLay_LogLevel level) {
  switch ((int)level) {
    case ZLAY_LOG_LEVEL_TRACE: return "\x1b[90m";
    case ZLAY_LOG_LEVEL_DEBUG: return "\x1b[36m";
    case ZLAY_LOG_LEVEL_INFO: return "\x1b[32m";
    case ZLAY_LOG_LEVEL_WARN: return "\x1b[33m";
    case ZLAY_LOG_LEVEL_ERROR: return "\x1b[31m";
    case ZLAY_LOG_LEVEL_CRITICAL: return "\x1b[1;35m";
    default: return "";
  }
}

static inline int ZLay__LogCharLower(int c) {
  return (c >= 'A' && c <= 'Z') ? (c - 'A' + 'a') : c;
}

static inline bool ZLay__LogStringEqualsIgnoreCase(const char* a, const char* b) {
  if (a == NULL || b == NULL) {
    return false;
  }

  while (*a != '\0' && *b != '\0') {
    if (ZLay__LogCharLower((unsigned char)*a) != ZLay__LogCharLower((unsigned char)*b)) {
      return false;
    }
    ++a;
    ++b;
  }

  return *a == '\0' && *b == '\0';
}

static inline bool ZLay_LogLevelFromString(const char* text, ZLay_LogLevel* out_level) {
  ZLay_LogLevel level = (ZLay_LogLevel)ZLAY_LOG_LEVEL_INFO;

  if (ZLay__LogStringEqualsIgnoreCase(text, "trace")) {
    level = (ZLay_LogLevel)ZLAY_LOG_LEVEL_TRACE;
  } else if (ZLay__LogStringEqualsIgnoreCase(text, "debug")) {
    level = (ZLay_LogLevel)ZLAY_LOG_LEVEL_DEBUG;
  } else if (ZLay__LogStringEqualsIgnoreCase(text, "info")) {
    level = (ZLay_LogLevel)ZLAY_LOG_LEVEL_INFO;
  } else if (ZLay__LogStringEqualsIgnoreCase(text, "warn") || ZLay__LogStringEqualsIgnoreCase(text, "warning")) {
    level = (ZLay_LogLevel)ZLAY_LOG_LEVEL_WARN;
  } else if (ZLay__LogStringEqualsIgnoreCase(text, "error") || ZLay__LogStringEqualsIgnoreCase(text, "err")) {
    level = (ZLay_LogLevel)ZLAY_LOG_LEVEL_ERROR;
  } else if (ZLay__LogStringEqualsIgnoreCase(text, "critical") || ZLay__LogStringEqualsIgnoreCase(text, "fatal")) {
    level = (ZLay_LogLevel)ZLAY_LOG_LEVEL_CRITICAL;
  } else if (ZLay__LogStringEqualsIgnoreCase(text, "off") || ZLay__LogStringEqualsIgnoreCase(text, "none")) {
    level = (ZLay_LogLevel)ZLAY_LOG_LEVEL_OFF;
  } else {
    return false;
  }

  if (out_level != NULL) {
    *out_level = level;
  }
  return true;
}

static inline void ZLay_LogSetLevel(ZLay_LogLevel level) {
  ZLay_LogGetConfig()->level = level;
}

static inline ZLay_LogLevel ZLay_LogGetLevel(void) {
  return ZLay_LogGetConfig()->level;
}

static inline bool ZLay_LogShouldLog(ZLay_LogLevel level) {
  const ZLay_LogConfig* config = ZLay_LogGetConfig();
  return (int)level >= (int)config->level && (int)level < ZLAY_LOG_LEVEL_OFF;
}

static inline void ZLay_LogSetFlags(uint32_t flags) {
  ZLay_LogGetConfig()->flags = flags;
}

static inline uint32_t ZLay_LogGetFlags(void) {
  return ZLay_LogGetConfig()->flags;
}

static inline void ZLay_LogEnableFlags(uint32_t flags) {
  ZLay_LogGetConfig()->flags |= flags;
}

static inline void ZLay_LogDisableFlags(uint32_t flags) {
  ZLay_LogGetConfig()->flags &= ~flags;
}

static inline void ZLay_LogSetName(const char* logger_name) {
  ZLay_LogGetConfig()->logger_name = (logger_name != NULL) ? logger_name : ZLAY_LOG_DEFAULT_NAME;
}

static inline void ZLay_LogSetStream(FILE* stream) {
  ZLay_LogGetConfig()->stream = stream;
}

static inline void ZLay_LogCloseFile(void) {
  ZLay_LogConfig* config = ZLay_LogGetConfig();
  if (config->owns_file && config->file != NULL) {
    fclose(config->file);
  }
  config->file = NULL;
  config->owns_file = false;
}

static inline void ZLay_LogSetFile(FILE* file) {
  ZLay_LogConfig* config = ZLay_LogGetConfig();
  if (config->owns_file && config->file != NULL && config->file != file) {
    fclose(config->file);
  }
  config->file = file;
  config->owns_file = false;
}

static inline bool ZLay_LogOpenFile(const char* path, bool append) {
  FILE* file;

  if (path == NULL || path[0] == '\0') {
    return false;
  }

  file = fopen(path, append ? "a" : "w");
  if (file == NULL) {
    return false;
  }

  ZLay_LogCloseFile();
  ZLay_LogGetConfig()->file = file;
  ZLay_LogGetConfig()->owns_file = true;
  return true;
}

static inline void ZLay_LogSetSink(ZLay_LogSinkFn sink, void* user) {
  ZLay_LogConfig* config = ZLay_LogGetConfig();
  config->sink = sink;
  config->sink_user = user;
}

static inline void ZLay_LogReset(void) {
  ZLay_LogConfig* config = ZLay_LogGetConfig();
  if (config->owns_file && config->file != NULL) {
    fclose(config->file);
  }
  *config = ZLay__LogDefaultConfig();
}

static inline FILE* ZLay__LogDefaultStream(ZLay_LogLevel level) {
  return ((int)level >= ZLAY_LOG_TO_STDERR_LEVEL) ? stderr : stdout;
}

static inline const char* ZLay__LogBaseName(const char* path) {
  const char* base = path;
  const char* cursor = path;

  if (path == NULL) {
    return NULL;
  }

  while (*cursor != '\0') {
    if (*cursor == '/' || *cursor == '\\') {
      base = cursor + 1;
    }
    ++cursor;
  }

  return base;
}

static inline void ZLay__LogTimeText(char out[16]) {
  time_t now;
  struct tm tm_value;
  struct tm* tm_pointer;

  if (out == NULL) {
    return;
  }

  out[0] = '-';
  out[1] = '-';
  out[2] = ':';
  out[3] = '-';
  out[4] = '-';
  out[5] = ':';
  out[6] = '-';
  out[7] = '-';
  out[8] = '\0';

  now = time(NULL);
  if (now == (time_t)-1) {
    return;
  }

#if defined(_MSC_VER)
  if (localtime_s(&tm_value, &now) != 0) {
    return;
  }
  tm_pointer = &tm_value;
#else
  tm_pointer = localtime(&now);
  if (tm_pointer == NULL) {
    return;
  }
  tm_value = *tm_pointer;
  tm_pointer = &tm_value;
#endif

  (void)strftime(out, 16u, "%H:%M:%S", tm_pointer);
}

static inline void ZLay__LogWriteRecord(FILE* stream, const ZLay_LogConfig* config, const ZLay_LogRecord* record, bool color) {
  bool wrote_context = false;
  const uint32_t flags = config->flags;

  if (stream == NULL || record == NULL) {
    return;
  }

  if ((flags & ZLAY_LOG_FLAG_TIME) != 0u) {
    fprintf(stream, "[%s] ", record->time_text != NULL ? record->time_text : "--:--:--");
  }

  if ((flags & ZLAY_LOG_FLAG_NAME) != 0u) {
    fprintf(stream, "[%s] ", record->logger_name != NULL ? record->logger_name : ZLAY_LOG_DEFAULT_NAME);
  }

  if ((flags & ZLAY_LOG_FLAG_LEVEL) != 0u) {
    if (color && (flags & ZLAY_LOG_FLAG_COLOR) != 0u) {
      fprintf(stream, "[%s%s\x1b[0m] ", ZLay__LogLevelColor(record->level), record->level_name);
    } else {
      fprintf(stream, "[%s] ", record->level_name);
    }
  }

  if ((flags & ZLAY_LOG_FLAG_LOCATION) != 0u && record->file != NULL) {
    const char* file = ((flags & ZLAY_LOG_FLAG_SHORT_FILE) != 0u) ? ZLay__LogBaseName(record->file) : record->file;
    fprintf(stream, "%s:%d", file != NULL ? file : "?", record->line);
    wrote_context = true;
  }

  if ((flags & ZLAY_LOG_FLAG_FUNCTION) != 0u && record->function != NULL && record->function[0] != '\0') {
    if (wrote_context) {
      fputc(' ', stream);
    }
    fprintf(stream, "%s()", record->function);
    wrote_context = true;
  }

  if (wrote_context) {
    fputs(": ", stream);
  }

  fputs(record->message != NULL ? record->message : "", stream);
  fputc('\n', stream);

  if ((flags & ZLAY_LOG_FLAG_FLUSH) != 0u || (int)record->level >= ZLAY_LOG_FLUSH_LEVEL) {
    fflush(stream);
  }
}

static inline void ZLay__LogWrite(
  ZLay_LogLevel level,
  const char* file,
  int line,
  const char* function,
  const char* format,
  ...) ZLAY_LOG_PRINTF_FORMAT(5, 6);

static inline void ZLay__LogWrite(ZLay_LogLevel level, const char* file, int line, const char* function, const char* format, ...) {
  ZLay_LogConfig* config;
  ZLay_LogRecord record;
  char time_text[16];
  char stack_message[(size_t)ZLAY_LOG_MESSAGE_CAPACITY];
  char* message = stack_message;
  size_t message_length;
  int needed;
  va_list args;
#if ZLAY_LOG_USE_MALLOC
  va_list args_copy;
#endif

  if (format == NULL || !ZLay_LogShouldLog(level)) {
    return;
  }

  va_start(args, format);
#if ZLAY_LOG_USE_MALLOC
  va_copy(args_copy, args);
#endif
  needed = vsnprintf(stack_message, sizeof(stack_message), format, args);
  va_end(args);

  if (needed < 0) {
    static const char fallback[] = "<zlay log format error>";
    memcpy(stack_message, fallback, sizeof(fallback));
    message_length = sizeof(fallback) - 1u;
  } else {
    message_length = (size_t)needed;
#if ZLAY_LOG_USE_MALLOC
    if ((size_t)needed >= sizeof(stack_message)) {
      char* heap_message = (char*)malloc((size_t)needed + 1u);
      if (heap_message != NULL) {
        int heap_needed = vsnprintf(heap_message, (size_t)needed + 1u, format, args_copy);
        if (heap_needed >= 0) {
          message = heap_message;
          message_length = (size_t)heap_needed;
        } else {
          free(heap_message);
          message_length = strlen(stack_message);
        }
      } else {
        message_length = strlen(stack_message);
      }
    }
#else
    if ((size_t)needed >= sizeof(stack_message)) {
      message_length = strlen(stack_message);
    }
#endif
  }

#if ZLAY_LOG_USE_MALLOC
  va_end(args_copy);
#endif

  config = ZLay_LogGetConfig();
  ZLay__LogTimeText(time_text);

  record.level = level;
  record.level_name = ZLay_LogLevelName(level);
  record.logger_name = config->logger_name;
  record.file = file;
  record.line = line;
  record.function = function;
  record.message = message;
  record.message_length = message_length;
  record.time_text = time_text;

  if ((config->flags & ZLAY_LOG_FLAG_STDIO) != 0u) {
    FILE* stream = (config->stream != NULL) ? config->stream : ZLay__LogDefaultStream(level);
    ZLay__LogWriteRecord(stream, config, &record, true);
  }

  if (config->file != NULL) {
    ZLay__LogWriteRecord(config->file, config, &record, false);
  }

  if (config->sink != NULL) {
    config->sink(&record, config->sink_user);
  }

  if (message != stack_message) {
    free(message);
  }
}

#if defined(__cplusplus) || (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L)
#  define ZLAY_LOG_FUNCTION __func__
#elif defined(_MSC_VER)
#  define ZLAY_LOG_FUNCTION __FUNCTION__
#else
#  define ZLAY_LOG_FUNCTION NULL
#endif

#define ZLAY__LOG_CONCAT_INNER(a, b) a##b
#define ZLAY__LOG_CONCAT(a, b) ZLAY__LOG_CONCAT_INNER(a, b)

#if defined(__COUNTER__)
#  define ZLAY__LOG_UNIQUE(name) ZLAY__LOG_CONCAT(name, __COUNTER__)
#else
#  define ZLAY__LOG_UNIQUE(name) ZLAY__LOG_CONCAT(name, __LINE__)
#endif

#if ZLAY_ENABLE_LOGGING
#  define ZLAY_LOG_ENABLED(level) (((int)(level) >= ZLAY_LOG_ACTIVE_LEVEL) && ZLay_LogShouldLog((ZLay_LogLevel)(level)))
#  define ZLAY_LOG_AT(level, file, line, function, ...) \
    do { \
      const ZLay_LogLevel zlay_log_level = (ZLay_LogLevel)(level); \
      if (ZLAY_LOG_ENABLED(zlay_log_level)) { \
        ZLay__LogWrite(zlay_log_level, (file), (line), (function), __VA_ARGS__); \
      } \
    } while (0)
#  define ZLAY_LOG(level, ...) ZLAY_LOG_AT((level), __FILE__, __LINE__, ZLAY_LOG_FUNCTION, __VA_ARGS__)
#  define ZLAY_LOG_IF(level, condition, ...) \
    do { \
      if ((condition)) { \
        ZLAY_LOG((level), __VA_ARGS__); \
      } \
    } while (0)
#  define ZLAY__LOG_ONCE(level, flag_name, ...) \
    do { \
      const ZLay_LogLevel zlay_log_level = (ZLay_LogLevel)(level); \
      static bool flag_name = false; \
      if (!flag_name && ZLAY_LOG_ENABLED(zlay_log_level)) { \
        flag_name = true; \
        ZLay__LogWrite(zlay_log_level, __FILE__, __LINE__, ZLAY_LOG_FUNCTION, __VA_ARGS__); \
      } \
    } while (0)
#  define ZLAY_LOG_ONCE(level, ...) ZLAY__LOG_ONCE((level), ZLAY__LOG_UNIQUE(zlay_log_once_), __VA_ARGS__)
#  define ZLAY__LOG_EVERY_N(level, count, counter_name, ...) \
    do { \
      static uint32_t counter_name = 0u; \
      const uint32_t zlay_log_every_n = (uint32_t)(count); \
      if (zlay_log_every_n != 0u && (counter_name++ % zlay_log_every_n) == 0u) { \
        ZLAY_LOG((level), __VA_ARGS__); \
      } \
    } while (0)
#  define ZLAY_LOG_EVERY_N(level, count, ...) ZLAY__LOG_EVERY_N((level), (count), ZLAY__LOG_UNIQUE(zlay_log_every_), __VA_ARGS__)
#else
#  define ZLAY_LOG_ENABLED(level) false
#  define ZLAY_LOG_AT(level, file, line, function, ...) ((void)0)
#  define ZLAY_LOG(level, ...) ((void)0)
#  define ZLAY_LOG_IF(level, condition, ...) ((void)0)
#  define ZLAY_LOG_ONCE(level, ...) ((void)0)
#  define ZLAY_LOG_EVERY_N(level, count, ...) ((void)0)
#endif

#if ZLAY_ENABLE_LOGGING && ZLAY_LOG_ACTIVE_LEVEL <= ZLAY_LOG_LEVEL_TRACE
#  define ZLAY_LOG_TRACE(...) ZLAY_LOG((ZLay_LogLevel)ZLAY_LOG_LEVEL_TRACE, __VA_ARGS__)
#else
#  define ZLAY_LOG_TRACE(...) ((void)0)
#endif

#if ZLAY_ENABLE_LOGGING && ZLAY_LOG_ACTIVE_LEVEL <= ZLAY_LOG_LEVEL_DEBUG
#  define ZLAY_LOG_DEBUG(...) ZLAY_LOG((ZLay_LogLevel)ZLAY_LOG_LEVEL_DEBUG, __VA_ARGS__)
#else
#  define ZLAY_LOG_DEBUG(...) ((void)0)
#endif

#if ZLAY_ENABLE_LOGGING && ZLAY_LOG_ACTIVE_LEVEL <= ZLAY_LOG_LEVEL_INFO
#  define ZLAY_LOG_INFO(...) ZLAY_LOG((ZLay_LogLevel)ZLAY_LOG_LEVEL_INFO, __VA_ARGS__)
#else
#  define ZLAY_LOG_INFO(...) ((void)0)
#endif

#if ZLAY_ENABLE_LOGGING && ZLAY_LOG_ACTIVE_LEVEL <= ZLAY_LOG_LEVEL_WARN
#  define ZLAY_LOG_WARN(...) ZLAY_LOG((ZLay_LogLevel)ZLAY_LOG_LEVEL_WARN, __VA_ARGS__)
#else
#  define ZLAY_LOG_WARN(...) ((void)0)
#endif

#if ZLAY_ENABLE_LOGGING && ZLAY_LOG_ACTIVE_LEVEL <= ZLAY_LOG_LEVEL_ERROR
#  define ZLAY_LOG_ERROR(...) ZLAY_LOG((ZLay_LogLevel)ZLAY_LOG_LEVEL_ERROR, __VA_ARGS__)
#else
#  define ZLAY_LOG_ERROR(...) ((void)0)
#endif

#if ZLAY_ENABLE_LOGGING && ZLAY_LOG_ACTIVE_LEVEL <= ZLAY_LOG_LEVEL_CRITICAL
#  define ZLAY_LOG_CRITICAL(...) ZLAY_LOG((ZLay_LogLevel)ZLAY_LOG_LEVEL_CRITICAL, __VA_ARGS__)
#else
#  define ZLAY_LOG_CRITICAL(...) ((void)0)
#endif

#define ZLAY_TRACE(...) ZLAY_LOG_TRACE(__VA_ARGS__)
#define ZLAY_DEBUG(...) ZLAY_LOG_DEBUG(__VA_ARGS__)
#define ZLAY_INFO(...) ZLAY_LOG_INFO(__VA_ARGS__)
#define ZLAY_WARN(...) ZLAY_LOG_WARN(__VA_ARGS__)
#define ZLAY_ERROR(...) ZLAY_LOG_ERROR(__VA_ARGS__)
#define ZLAY_CRITICAL(...) ZLAY_LOG_CRITICAL(__VA_ARGS__)

#if ZLAY_ENABLE_LOGGING
#  define ZLAY_LOG_ASSERT(condition) \
    do { \
      if (!(condition)) { \
        ZLAY_LOG_AT((ZLay_LogLevel)ZLAY_LOG_LEVEL_CRITICAL, __FILE__, __LINE__, ZLAY_LOG_FUNCTION, "assertion failed: %s", #condition); \
        abort(); \
      } \
    } while (0)
#  define ZLAY_LOG_ASSERT_MSG(condition, ...) \
    do { \
      if (!(condition)) { \
        ZLAY_LOG_AT((ZLay_LogLevel)ZLAY_LOG_LEVEL_CRITICAL, __FILE__, __LINE__, ZLAY_LOG_FUNCTION, __VA_ARGS__); \
        abort(); \
      } \
    } while (0)
#else
#  define ZLAY_LOG_ASSERT(condition) ((void)0)
#  define ZLAY_LOG_ASSERT_MSG(condition, ...) ((void)0)
#endif

#ifdef __cplusplus
}
#endif

#endif // ZLAY_ZLOGGER_H
