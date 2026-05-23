#ifndef ZLAY_SHELL_H
#define ZLAY_SHELL_H

#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(_WIN32)
#  ifndef WIN32_LEAN_AND_MEAN
#    define WIN32_LEAN_AND_MEAN
#  endif
#  include <io.h>
#  include <windows.h>
#  ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#    define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#  endif
#  ifndef DISABLE_NEWLINE_AUTO_RETURN
#    define DISABLE_NEWLINE_AUTO_RETURN 0x0008
#  endif
#else
#  include <unistd.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(_WIN32)
int fileno(FILE* stream);
#endif

#ifndef ZLAY_SHELL_PRINTF_FORMAT
#  if defined(__GNUC__) || defined(__clang__)
#    define ZLAY_SHELL_PRINTF_FORMAT(format_index, first_arg) __attribute__((format(printf, format_index, first_arg)))
#  else
#    define ZLAY_SHELL_PRINTF_FORMAT(format_index, first_arg)
#  endif
#endif

#ifndef ZLAY_SHELL_ENABLE_COLOR
#  define ZLAY_SHELL_ENABLE_COLOR 1
#endif

#ifndef ZLAY_SHELL_ENABLE_UTF8
#  define ZLAY_SHELL_ENABLE_UTF8 1
#endif

#ifndef ZLAY_SHELL_ESCAPE_CAPACITY
#  define ZLAY_SHELL_ESCAPE_CAPACITY 96
#endif

typedef enum ZLay_ShellColorDepth {
  ZLAY_SHELL_COLOR_NONE = 0,
  ZLAY_SHELL_COLOR_ANSI16 = 16,
  ZLAY_SHELL_COLOR_ANSI256 = 256,
  ZLAY_SHELL_COLOR_TRUECOLOR = 16777216
} ZLay_ShellColorDepth;

typedef enum ZLay_ShellColorKind {
  ZLAY_SHELL_COLOR_KIND_DEFAULT = 0,
  ZLAY_SHELL_COLOR_KIND_INDEXED = 1,
  ZLAY_SHELL_COLOR_KIND_RGB = 2
} ZLay_ShellColorKind;

typedef enum ZLay_ShellStyleFlags {
  ZLAY_SHELL_STYLE_NONE = 0,
  ZLAY_SHELL_STYLE_BOLD = 1u << 0,
  ZLAY_SHELL_STYLE_DIM = 1u << 1,
  ZLAY_SHELL_STYLE_ITALIC = 1u << 2,
  ZLAY_SHELL_STYLE_UNDERLINE = 1u << 3,
  ZLAY_SHELL_STYLE_INVERSE = 1u << 4,
  ZLAY_SHELL_STYLE_STRIKE = 1u << 5
} ZLay_ShellStyleFlags;

typedef enum ZLay_ShellStream {
  ZLAY_SHELL_STREAM_STDOUT = 0,
  ZLAY_SHELL_STREAM_STDERR = 1,
  ZLAY_SHELL_STREAM_STDIN = 2
} ZLay_ShellStream;

typedef enum ZLay_ShellTone {
  ZLAY_SHELL_TONE_TEXT = 0,
  ZLAY_SHELL_TONE_MUTED,
  ZLAY_SHELL_TONE_ACCENT,
  ZLAY_SHELL_TONE_INFO,
  ZLAY_SHELL_TONE_DEBUG,
  ZLAY_SHELL_TONE_SUCCESS,
  ZLAY_SHELL_TONE_WARNING,
  ZLAY_SHELL_TONE_ERROR,
  ZLAY_SHELL_TONE_CRITICAL
} ZLay_ShellTone;

typedef struct ZLay_ShellColor {
  ZLay_ShellColorKind kind;
  uint8_t index;
  uint8_t r;
  uint8_t g;
  uint8_t b;
} ZLay_ShellColor;

typedef struct ZLay_ShellStyle {
  uint32_t flags;
  ZLay_ShellColor fg;
  ZLay_ShellColor bg;
} ZLay_ShellStyle;

typedef struct ZLay_ShellCapabilities {
  bool is_tty;
  bool ansi;
  bool unicode;
  bool hyperlinks;
  bool forced_color;
  bool no_color;
  ZLay_ShellColorDepth color_depth;
  const char* term;
  const char* colorterm;
} ZLay_ShellCapabilities;

static inline const char* ZLay_ShellColorDepthName(ZLay_ShellColorDepth depth) {
  switch (depth) {
    case ZLAY_SHELL_COLOR_NONE: return "none";
    case ZLAY_SHELL_COLOR_ANSI16: return "ansi16";
    case ZLAY_SHELL_COLOR_ANSI256: return "ansi256";
    case ZLAY_SHELL_COLOR_TRUECOLOR: return "truecolor";
    default: return "unknown";
  }
}

static inline const char* ZLay__ShellGetEnv(const char* name) {
  const char* value = getenv(name);
  return (value != NULL && value[0] != '\0') ? value : NULL;
}

static inline bool ZLay__ShellEnvIsTruthy(const char* name) {
  const char* value = ZLay__ShellGetEnv(name);
  if (value == NULL) {
    return false;
  }
  return !(strcmp(value, "0") == 0 || strcmp(value, "false") == 0 || strcmp(value, "FALSE") == 0);
}

static inline bool ZLay__ShellContains(const char* text, const char* needle) {
  return text != NULL && needle != NULL && strstr(text, needle) != NULL;
}

static inline int ZLay__ShellFileNo(FILE* stream) {
  if (stream == NULL) {
    return -1;
  }
#if defined(_WIN32)
  return _fileno(stream);
#else
  return fileno(stream);
#endif
}

static inline bool ZLay_ShellIsTty(FILE* stream) {
  const int fd = ZLay__ShellFileNo(stream);
  if (fd < 0) {
    return false;
  }
#if defined(_WIN32)
  return _isatty(fd) != 0;
#else
  return isatty(fd) != 0;
#endif
}

#if defined(_WIN32)
static inline HANDLE ZLay__ShellWindowsHandle(FILE* stream) {
  if (stream == stderr) {
    return GetStdHandle(STD_ERROR_HANDLE);
  }
  if (stream == stdin) {
    return GetStdHandle(STD_INPUT_HANDLE);
  }
  return GetStdHandle(STD_OUTPUT_HANDLE);
}
#endif

static inline bool ZLay_ShellEnableAnsi(FILE* stream) {
#if !ZLAY_SHELL_ENABLE_COLOR
  (void)stream;
  return false;
#elif defined(_WIN32)
  DWORD mode = 0;
  HANDLE handle = ZLay__ShellWindowsHandle(stream);

  if (handle == NULL || handle == INVALID_HANDLE_VALUE) {
    return false;
  }

  if (!GetConsoleMode(handle, &mode)) {
    return ZLay__ShellEnvIsTruthy("ANSICON") ||
           ZLay__ShellContains(ZLay__ShellGetEnv("ConEmuANSI"), "ON") ||
           ZLay__ShellGetEnv("WT_SESSION") != NULL;
  }

  mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
  mode |= DISABLE_NEWLINE_AUTO_RETURN;
  if (SetConsoleMode(handle, mode)) {
    return true;
  }

  mode &= ~DISABLE_NEWLINE_AUTO_RETURN;
  return SetConsoleMode(handle, mode) != 0;
#else
  (void)stream;
  return true;
#endif
}

static inline bool ZLay_ShellEnableUtf8(void) {
#if !ZLAY_SHELL_ENABLE_UTF8
  return false;
#elif defined(_WIN32)
  return SetConsoleOutputCP(CP_UTF8) != 0 && SetConsoleCP(CP_UTF8) != 0;
#else
  return true;
#endif
}

static inline bool ZLay__ShellHasTrueColor(void) {
  const char* colorterm = ZLay__ShellGetEnv("COLORTERM");
  const char* term = ZLay__ShellGetEnv("TERM");

  return ZLay__ShellContains(colorterm, "truecolor") ||
         ZLay__ShellContains(colorterm, "24bit") ||
         ZLay__ShellContains(term, "truecolor") ||
         ZLay__ShellContains(term, "24bit") ||
         ZLay__ShellGetEnv("WT_SESSION") != NULL ||
         ZLay__ShellGetEnv("KONSOLE_VERSION") != NULL;
}

static inline bool ZLay__ShellHas256Color(void) {
  const char* term = ZLay__ShellGetEnv("TERM");
  return ZLay__ShellContains(term, "256color") ||
         ZLay__ShellContains(term, "xterm") ||
         ZLay__ShellContains(term, "screen") ||
         ZLay__ShellContains(term, "tmux");
}

static inline ZLay_ShellColorDepth ZLay_ShellDetectColorDepth(FILE* stream) {
  const char* force_color;
  const char* term;
  const bool forced = ZLay__ShellEnvIsTruthy("FORCE_COLOR") || ZLay__ShellEnvIsTruthy("CLICOLOR_FORCE");

  if (!ZLAY_SHELL_ENABLE_COLOR) {
    return ZLAY_SHELL_COLOR_NONE;
  }

  force_color = ZLay__ShellGetEnv("FORCE_COLOR");
  if (force_color != NULL) {
    if (strcmp(force_color, "3") == 0 || strcmp(force_color, "truecolor") == 0 || strcmp(force_color, "24bit") == 0) {
      return ZLAY_SHELL_COLOR_TRUECOLOR;
    }
    if (strcmp(force_color, "2") == 0 || strcmp(force_color, "256") == 0) {
      return ZLAY_SHELL_COLOR_ANSI256;
    }
    if (strcmp(force_color, "0") != 0) {
      return ZLAY_SHELL_COLOR_ANSI16;
    }
  }

  if (!forced && ZLay__ShellGetEnv("NO_COLOR") != NULL) {
    return ZLAY_SHELL_COLOR_NONE;
  }

  if (!forced && ZLay__ShellGetEnv("CLICOLOR") != NULL && strcmp(ZLay__ShellGetEnv("CLICOLOR"), "0") == 0) {
    return ZLAY_SHELL_COLOR_NONE;
  }

  if (!forced && !ZLay_ShellIsTty(stream)) {
    return ZLAY_SHELL_COLOR_NONE;
  }

  term = ZLay__ShellGetEnv("TERM");
  if (!forced && term != NULL && strcmp(term, "dumb") == 0) {
    return ZLAY_SHELL_COLOR_NONE;
  }

#if !defined(_WIN32)
  if (!forced && term == NULL) {
    return ZLAY_SHELL_COLOR_NONE;
  }
#endif

  if (ZLay__ShellHasTrueColor()) {
    return ZLAY_SHELL_COLOR_TRUECOLOR;
  }

  if (ZLay__ShellHas256Color()) {
    return ZLAY_SHELL_COLOR_ANSI256;
  }

  return ZLAY_SHELL_COLOR_ANSI16;
}

static inline ZLay_ShellCapabilities ZLay_ShellDetect(FILE* stream) {
  ZLay_ShellCapabilities caps;
  caps.is_tty = ZLay_ShellIsTty(stream);
  caps.forced_color = ZLay__ShellEnvIsTruthy("FORCE_COLOR") || ZLay__ShellEnvIsTruthy("CLICOLOR_FORCE");
  caps.no_color = ZLay__ShellGetEnv("NO_COLOR") != NULL;
  caps.term = ZLay__ShellGetEnv("TERM");
  caps.colorterm = ZLay__ShellGetEnv("COLORTERM");
  caps.color_depth = ZLay_ShellDetectColorDepth(stream);
  caps.ansi = caps.color_depth != ZLAY_SHELL_COLOR_NONE && (caps.forced_color || ZLay_ShellEnableAnsi(stream));
  caps.unicode = ZLay_ShellEnableUtf8();
  caps.hyperlinks = caps.ansi && (ZLay__ShellGetEnv("WT_SESSION") != NULL ||
                                  ZLay__ShellGetEnv("VTE_VERSION") != NULL ||
                                  ZLay__ShellGetEnv("ITERM_SESSION_ID") != NULL ||
                                  ZLay__ShellContains(caps.term, "xterm"));
  return caps;
}

static inline bool ZLay_ShellSupportsColor(FILE* stream) {
  return ZLay_ShellDetectColorDepth(stream) != ZLAY_SHELL_COLOR_NONE &&
         (ZLay__ShellEnvIsTruthy("FORCE_COLOR") ||
          ZLay__ShellEnvIsTruthy("CLICOLOR_FORCE") ||
          ZLay_ShellEnableAnsi(stream));
}

static inline ZLay_ShellColor ZLay_ShellColorDefault(void) {
  ZLay_ShellColor color;
  color.kind = ZLAY_SHELL_COLOR_KIND_DEFAULT;
  color.index = 0;
  color.r = 0;
  color.g = 0;
  color.b = 0;
  return color;
}

static inline ZLay_ShellColor ZLay_ShellColorIndexed(uint8_t index) {
  ZLay_ShellColor color = ZLay_ShellColorDefault();
  color.kind = ZLAY_SHELL_COLOR_KIND_INDEXED;
  color.index = index;
  return color;
}

static inline ZLay_ShellColor ZLay_ShellColorRgb(uint8_t r, uint8_t g, uint8_t b) {
  ZLay_ShellColor color = ZLay_ShellColorDefault();
  color.kind = ZLAY_SHELL_COLOR_KIND_RGB;
  color.r = r;
  color.g = g;
  color.b = b;
  return color;
}

static inline ZLay_ShellStyle ZLay_ShellStyleDefault(void) {
  ZLay_ShellStyle style;
  style.flags = ZLAY_SHELL_STYLE_NONE;
  style.fg = ZLay_ShellColorDefault();
  style.bg = ZLay_ShellColorDefault();
  return style;
}

static inline ZLay_ShellStyle ZLay_ShellStyleFg(ZLay_ShellColor fg) {
  ZLay_ShellStyle style = ZLay_ShellStyleDefault();
  style.fg = fg;
  return style;
}

static inline ZLay_ShellStyle ZLay_ShellStyleFgBg(ZLay_ShellColor fg, ZLay_ShellColor bg) {
  ZLay_ShellStyle style = ZLay_ShellStyleDefault();
  style.fg = fg;
  style.bg = bg;
  return style;
}

static inline ZLay_ShellStyle ZLay_ShellStyleWithFlags(ZLay_ShellStyle style, uint32_t flags) {
  style.flags |= flags;
  return style;
}

static inline ZLay_ShellColor ZLay_ShellToneColor(ZLay_ShellTone tone) {
  switch (tone) {
    case ZLAY_SHELL_TONE_MUTED: return ZLay_ShellColorRgb(148, 163, 184);
    case ZLAY_SHELL_TONE_ACCENT: return ZLay_ShellColorRgb(88, 196, 255);
    case ZLAY_SHELL_TONE_INFO: return ZLay_ShellColorRgb(96, 165, 250);
    case ZLAY_SHELL_TONE_DEBUG: return ZLay_ShellColorRgb(34, 211, 238);
    case ZLAY_SHELL_TONE_SUCCESS: return ZLay_ShellColorRgb(74, 222, 128);
    case ZLAY_SHELL_TONE_WARNING: return ZLay_ShellColorRgb(251, 191, 36);
    case ZLAY_SHELL_TONE_ERROR: return ZLay_ShellColorRgb(248, 113, 113);
    case ZLAY_SHELL_TONE_CRITICAL: return ZLay_ShellColorRgb(244, 114, 182);
    case ZLAY_SHELL_TONE_TEXT:
    default: return ZLay_ShellColorDefault();
  }
}

static inline ZLay_ShellColor ZLay_ShellToneBackground(ZLay_ShellTone tone) {
  switch (tone) {
    case ZLAY_SHELL_TONE_MUTED: return ZLay_ShellColorRgb(30, 41, 59);
    case ZLAY_SHELL_TONE_ACCENT: return ZLay_ShellColorRgb(12, 74, 110);
    case ZLAY_SHELL_TONE_INFO: return ZLay_ShellColorRgb(30, 64, 175);
    case ZLAY_SHELL_TONE_DEBUG: return ZLay_ShellColorRgb(21, 94, 117);
    case ZLAY_SHELL_TONE_SUCCESS: return ZLay_ShellColorRgb(22, 101, 52);
    case ZLAY_SHELL_TONE_WARNING: return ZLay_ShellColorRgb(113, 63, 18);
    case ZLAY_SHELL_TONE_ERROR: return ZLay_ShellColorRgb(127, 29, 29);
    case ZLAY_SHELL_TONE_CRITICAL: return ZLay_ShellColorRgb(131, 24, 67);
    case ZLAY_SHELL_TONE_TEXT:
    default: return ZLay_ShellColorDefault();
  }
}

static inline ZLay_ShellStyle ZLay_ShellStyleTone(ZLay_ShellTone tone) {
  return ZLay_ShellStyleFg(ZLay_ShellToneColor(tone));
}

static inline ZLay_ShellStyle ZLay_ShellStyleStrong(ZLay_ShellTone tone) {
  return ZLay_ShellStyleWithFlags(ZLay_ShellStyleTone(tone), ZLAY_SHELL_STYLE_BOLD);
}

static inline ZLay_ShellStyle ZLay_ShellStyleBadge(ZLay_ShellTone tone) {
  return ZLay_ShellStyleWithFlags(
    ZLay_ShellStyleFgBg(ZLay_ShellColorRgb(248, 250, 252), ZLay_ShellToneBackground(tone)),
    ZLAY_SHELL_STYLE_BOLD
  );
}

static inline int ZLay__ShellRgbToAnsi256(uint8_t r, uint8_t g, uint8_t b) {
  const int gray_average = ((int)r + (int)g + (int)b) / 3;

  if (r == g && g == b) {
    if (gray_average < 8) {
      return 16;
    }
    if (gray_average > 248) {
      return 231;
    }
    return 232 + ((gray_average - 8) * 24) / 247;
  }

  return 16 + (36 * ((int)r * 5 / 255)) + (6 * ((int)g * 5 / 255)) + ((int)b * 5 / 255);
}

static inline int ZLay__ShellRgbToAnsi16(uint8_t r, uint8_t g, uint8_t b, bool background) {
  const int bright = ((int)r + (int)g + (int)b) >= 384 ? 60 : 0;
  int index = 0;

  if (r > 127) {
    index |= 1;
  }
  if (g > 127) {
    index |= 2;
  }
  if (b > 127) {
    index |= 4;
  }

  return (background ? 40 : 30) + bright + index;
}

static inline int ZLay__ShellAppend(char* out, size_t out_size, size_t* offset, const char* text) {
  int written;

  if (out == NULL || offset == NULL || *offset >= out_size) {
    return 0;
  }

  written = snprintf(out + *offset, out_size - *offset, "%s", text);
  if (written < 0) {
    return written;
  }

  *offset += (size_t)written < out_size - *offset ? (size_t)written : out_size - *offset;
  return written;
}

static inline void ZLay__ShellAppendColor(
  char* out,
  size_t out_size,
  size_t* offset,
  ZLay_ShellColor color,
  bool background,
  ZLay_ShellColorDepth depth
) {
  if (out == NULL || offset == NULL || *offset >= out_size) {
    return;
  }

  if (color.kind == ZLAY_SHELL_COLOR_KIND_DEFAULT) {
    return;
  }

  if (color.kind == ZLAY_SHELL_COLOR_KIND_INDEXED) {
    if (depth == ZLAY_SHELL_COLOR_TRUECOLOR || depth == ZLAY_SHELL_COLOR_ANSI256) {
      (void)snprintf(out + *offset, out_size - *offset, ";%d;5;%u", background ? 48 : 38, (unsigned)color.index);
      *offset = strlen(out);
    } else if (depth == ZLAY_SHELL_COLOR_ANSI16) {
      const uint8_t index = (uint8_t)(color.index % 16u);
      const int code = (background ? 40 : 30) + (index % 8u) + (index >= 8u ? 60 : 0);
      (void)snprintf(out + *offset, out_size - *offset, ";%d", code);
      *offset = strlen(out);
    }
    return;
  }

  if (depth == ZLAY_SHELL_COLOR_TRUECOLOR) {
    (void)snprintf(
      out + *offset,
      out_size - *offset,
      ";%d;2;%u;%u;%u",
      background ? 48 : 38,
      (unsigned)color.r,
      (unsigned)color.g,
      (unsigned)color.b
    );
    *offset = strlen(out);
  } else if (depth == ZLAY_SHELL_COLOR_ANSI256) {
    (void)snprintf(out + *offset, out_size - *offset, ";%d;5;%d", background ? 48 : 38, ZLay__ShellRgbToAnsi256(color.r, color.g, color.b));
    *offset = strlen(out);
  } else if (depth == ZLAY_SHELL_COLOR_ANSI16) {
    (void)snprintf(out + *offset, out_size - *offset, ";%d", ZLay__ShellRgbToAnsi16(color.r, color.g, color.b, background));
    *offset = strlen(out);
  }
}

static inline bool ZLay_ShellStyleEscape(char* out, size_t out_size, ZLay_ShellStyle style, ZLay_ShellColorDepth depth) {
  size_t offset = 0;
  bool any = false;

  if (out == NULL || out_size == 0u) {
    return false;
  }

  out[0] = '\0';
  if (depth == ZLAY_SHELL_COLOR_NONE) {
    return false;
  }

  ZLay__ShellAppend(out, out_size, &offset, "\x1b[0");

  if ((style.flags & ZLAY_SHELL_STYLE_BOLD) != 0u) {
    ZLay__ShellAppend(out, out_size, &offset, ";1");
    any = true;
  }
  if ((style.flags & ZLAY_SHELL_STYLE_DIM) != 0u) {
    ZLay__ShellAppend(out, out_size, &offset, ";2");
    any = true;
  }
  if ((style.flags & ZLAY_SHELL_STYLE_ITALIC) != 0u) {
    ZLay__ShellAppend(out, out_size, &offset, ";3");
    any = true;
  }
  if ((style.flags & ZLAY_SHELL_STYLE_UNDERLINE) != 0u) {
    ZLay__ShellAppend(out, out_size, &offset, ";4");
    any = true;
  }
  if ((style.flags & ZLAY_SHELL_STYLE_INVERSE) != 0u) {
    ZLay__ShellAppend(out, out_size, &offset, ";7");
    any = true;
  }
  if ((style.flags & ZLAY_SHELL_STYLE_STRIKE) != 0u) {
    ZLay__ShellAppend(out, out_size, &offset, ";9");
    any = true;
  }

  if (style.fg.kind != ZLAY_SHELL_COLOR_KIND_DEFAULT) {
    ZLay__ShellAppendColor(out, out_size, &offset, style.fg, false, depth);
    any = true;
  }
  if (style.bg.kind != ZLAY_SHELL_COLOR_KIND_DEFAULT) {
    ZLay__ShellAppendColor(out, out_size, &offset, style.bg, true, depth);
    any = true;
  }

  if (!any) {
    out[0] = '\0';
    return false;
  }

  ZLay__ShellAppend(out, out_size, &offset, "m");
  return true;
}

static inline void ZLay_ShellReset(FILE* stream) {
  if (stream != NULL && ZLay_ShellSupportsColor(stream)) {
    fputs("\x1b[0m", stream);
  }
}

static inline void ZLay_ShellSetStyle(FILE* stream, ZLay_ShellStyle style) {
  char escape[ZLAY_SHELL_ESCAPE_CAPACITY];
  const ZLay_ShellColorDepth depth = ZLay_ShellDetectColorDepth(stream);

  if (stream != NULL &&
      ZLay_ShellSupportsColor(stream) &&
      ZLay_ShellStyleEscape(escape, sizeof(escape), style, depth)) {
    fputs(escape, stream);
  }
}

static inline void ZLay_ShellWriteStyled(FILE* stream, ZLay_ShellStyle style, const char* text) {
  const bool color = stream != NULL && ZLay_ShellSupportsColor(stream);

  if (stream == NULL) {
    return;
  }

  if (color) {
    ZLay_ShellSetStyle(stream, style);
  }
  fputs(text != NULL ? text : "", stream);
  if (color) {
    ZLay_ShellReset(stream);
  }
}

static inline void ZLay_ShellPrintfStyled(FILE* stream, ZLay_ShellStyle style, const char* format, ...) ZLAY_SHELL_PRINTF_FORMAT(3, 4);

static inline void ZLay_ShellPrintfStyled(FILE* stream, ZLay_ShellStyle style, const char* format, ...) {
  const bool color = stream != NULL && ZLay_ShellSupportsColor(stream);
  va_list args;

  if (stream == NULL || format == NULL) {
    return;
  }

  if (color) {
    ZLay_ShellSetStyle(stream, style);
  }

  va_start(args, format);
  vfprintf(stream, format, args);
  va_end(args);

  if (color) {
    ZLay_ShellReset(stream);
  }
}

static inline void ZLay_ShellPrintRule(FILE* stream, const char* label, ZLay_ShellTone tone) {
  ZLay_ShellStyle line = ZLay_ShellStyleTone(ZLAY_SHELL_TONE_MUTED);
  ZLay_ShellStyle title = ZLay_ShellStyleStrong(tone);

  if (stream == NULL) {
    return;
  }

  ZLay_ShellWriteStyled(stream, line, "== ");
  ZLay_ShellWriteStyled(stream, title, label != NULL ? label : "");
  ZLay_ShellWriteStyled(stream, line, " ==================================================");
  fputc('\n', stream);
}

static inline void ZLay_ShellPrintBadge(FILE* stream, const char* label, ZLay_ShellTone tone) {
  const bool color = stream != NULL && ZLay_ShellSupportsColor(stream);

  if (stream == NULL) {
    return;
  }

  if (color) {
    ZLay_ShellSetStyle(stream, ZLay_ShellStyleBadge(tone));
    fprintf(stream, " %s ", label != NULL ? label : "");
    ZLay_ShellReset(stream);
  } else {
    fprintf(stream, "[%s]", label != NULL ? label : "");
  }
}

static inline void ZLay_ShellPrintKeyValue(FILE* stream, const char* key, const char* value, ZLay_ShellTone tone) {
  if (stream == NULL) {
    return;
  }

  ZLay_ShellWriteStyled(stream, ZLay_ShellStyleTone(ZLAY_SHELL_TONE_MUTED), key != NULL ? key : "");
  ZLay_ShellWriteStyled(stream, ZLay_ShellStyleTone(ZLAY_SHELL_TONE_MUTED), "=");
  ZLay_ShellWriteStyled(stream, ZLay_ShellStyleStrong(tone), value != NULL ? value : "");
}

static inline void ZLay_ShellPrintCapabilitySummary(FILE* stream, const ZLay_ShellCapabilities* caps) {
  if (stream == NULL || caps == NULL) {
    return;
  }

  ZLay_ShellPrintBadge(stream, "terminal", ZLAY_SHELL_TONE_ACCENT);
  fputc(' ', stream);
  ZLay_ShellPrintKeyValue(
    stream,
    "tty",
    caps->is_tty ? "yes" : "no",
    caps->is_tty ? ZLAY_SHELL_TONE_SUCCESS : ZLAY_SHELL_TONE_MUTED
  );
  fputc(' ', stream);
  ZLay_ShellPrintKeyValue(
    stream,
    "ansi",
    caps->ansi ? "yes" : "no",
    caps->ansi ? ZLAY_SHELL_TONE_SUCCESS : ZLAY_SHELL_TONE_MUTED
  );
  fputc(' ', stream);
  ZLay_ShellPrintKeyValue(stream, "color", ZLay_ShellColorDepthName(caps->color_depth), ZLAY_SHELL_TONE_INFO);
  fputc(' ', stream);
  ZLay_ShellPrintKeyValue(
    stream,
    "unicode",
    caps->unicode ? "yes" : "no",
    caps->unicode ? ZLAY_SHELL_TONE_SUCCESS : ZLAY_SHELL_TONE_MUTED
  );
  fputc(' ', stream);
  ZLay_ShellPrintKeyValue(stream, "term", caps->term != NULL ? caps->term : "unknown", ZLAY_SHELL_TONE_MUTED);
  fputc('\n', stream);
}

#ifdef __cplusplus
}
#endif

#endif // ZLAY_SHELL_H
