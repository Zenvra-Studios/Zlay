# ZLay Platform Detection System (PRD)

## Overview

Sistem ini dirancang untuk menyediakan abstraksi deteksi platform lintas sistem operasi pada bahasa C/C++ menggunakan pendekatan:

- Hexadecimal Bit Flags
- Modular Macro Detection
- Low-Level Engine Style Architecture
- Extensible Cross-Platform Runtime Layer

Tujuan utama:
- Menghindari `#elif hell`
- Mengurangi macro boolean berlebihan
- Mempermudah scalability backend platform
- Memberikan struktur modern ala engine/runtime besar

---

# Goals

## Primary Goals

- Membuat platform detection yang scalable
- Mendukung multi-platform modern
- Menggunakan bitmask hexadecimal
- Memungkinkan kombinasi flag platform
- Memudahkan backend abstraction
- Memudahkan maintenance jangka panjang

---

# Design Philosophy

Pendekatan ini mengadopsi pola yang umum ditemukan pada:

- Game Engine
- Rendering Engine
- Runtime VM
- Kernel Abstraction Layer
- Cross-platform Middleware

Alih-alih menggunakan:

```cpp
#if WINDOWS
#elif LINUX
#elif MACOS
#elif BSD
#endif
```

Sistem menggunakan:

```cpp
#if (ZLAY_CURRENT_PLATFORM & ZLAY_PLATFORM_WINDOWS)
#endif
```

yang lebih modular dan extensible.

---

# Architecture

## Core Concept

Setiap platform memiliki representasi:

- Hexadecimal Flag
- Unique Bit Position

Contoh:

| Platform | Hex |
|---|---|
| Windows | `0x00000001` |
| Linux | `0x00000800` |
| macOS | `0x00000010` |
| Android | `0x00000400` |

---

# Folder Structure

```txt
include/
└── zlay/
    ├── zlay_platform.h
    ├── zlay_platform_windows.h
    ├── zlay_platform_linux.h
    ├── zlay_platform_apple.h
    ├── zlay_platform_android.h
    ├── zlay_platform_bsd.h
    └── ...
```

---

# Platform Flag System

## Flag Definition

```cpp
#define ZLAY_PLATFORM_NONE         0x00000000

#define ZLAY_PLATFORM_WINDOWS      0x00000001
#define ZLAY_PLATFORM_WIN32        0x00000002
#define ZLAY_PLATFORM_WIN64        0x00000004

#define ZLAY_PLATFORM_APPLE        0x00000008
#define ZLAY_PLATFORM_MACOS        0x00000010
#define ZLAY_PLATFORM_IOS          0x00000020

#define ZLAY_PLATFORM_ANDROID      0x00000400
#define ZLAY_PLATFORM_LINUX        0x00000800
```

---

# Group Flag System

## BSD Group

```cpp
#define ZLAY_PLATFORM_BSD \
( ZLAY_PLATFORM_FREEBSD | \
  ZLAY_PLATFORM_OPENBSD | \
  ZLAY_PLATFORM_NETBSD  | \
  ZLAY_PLATFORM_DRAGONFLYBSD )
```

## UNIX Group

```cpp
#define ZLAY_PLATFORM_UNIX \
( ZLAY_PLATFORM_APPLE      | \
  ZLAY_PLATFORM_ANDROID    | \
  ZLAY_PLATFORM_LINUX      | \
  ZLAY_PLATFORM_BSD        | \
  ZLAY_PLATFORM_SOLARIS    | \
  ZLAY_PLATFORM_HAIKU      | \
  ZLAY_PLATFORM_CYGWIN     | \
  ZLAY_PLATFORM_EMSCRIPTEN )
```

---

# Current Platform State

## Runtime Macro State

```cpp
#define ZLAY_CURRENT_PLATFORM ZLAY_PLATFORM_NONE
```

Nilai ini akan diubah saat compile-time berdasarkan hasil deteksi platform compiler.

---

# Detection Layer

## Windows

```cpp
#if defined(_WIN64)

    #undef  ZLAY_CURRENT_PLATFORM
    #define ZLAY_CURRENT_PLATFORM \
        (ZLAY_PLATFORM_WINDOWS | ZLAY_PLATFORM_WIN64)

#elif defined(_WIN32)

    #undef  ZLAY_CURRENT_PLATFORM
    #define ZLAY_CURRENT_PLATFORM \
        (ZLAY_PLATFORM_WINDOWS | ZLAY_PLATFORM_WIN32)

#endif
```

---

## Linux

```cpp
#elif defined(__linux__)

    #undef ZLAY_CURRENT_PLATFORM
    #define ZLAY_CURRENT_PLATFORM \
        (ZLAY_PLATFORM_LINUX)
```

---

## Apple Platforms

```cpp
#elif defined(__APPLE__) && defined(__MACH__)
```

Menggunakan:

```cpp
#include <TargetConditionals.h>
```

untuk membedakan:

- macOS
- iOS
- tvOS
- watchOS
- visionOS
- Mac Catalyst

---

# Utility Macros

## Platform Query Helper

```cpp
#define ZLAY_HAS_PLATFORM(x) \
    (ZLAY_CURRENT_PLATFORM & (x))
```

---

# Usage Examples

## Windows Detection

```cpp
#if ZLAY_HAS_PLATFORM(ZLAY_PLATFORM_WINDOWS)

#endif
```

---

## UNIX Detection

```cpp
#if ZLAY_HAS_PLATFORM(ZLAY_PLATFORM_UNIX)

#endif
```

---

## Linux Detection

```cpp
#if ZLAY_HAS_PLATFORM(ZLAY_PLATFORM_LINUX)

#endif
```

---

# Advantages

## 1. Scalable

Menambahkan platform baru cukup:

```cpp
#define ZLAY_PLATFORM_PLAYSTATION 0x00100000
```

tanpa mengubah struktur besar existing macro.

---

## 2. Cleaner Than Boolean Macros

Daripada:

```cpp
#define WINDOWS 1
#define LINUX 0
#define MACOS 0
```

cukup menggunakan satu state:

```cpp
ZLAY_CURRENT_PLATFORM
```

---

## 3. Supports Combined Flags

Platform dapat memiliki multiple states:

```cpp
(ZLAY_PLATFORM_WINDOWS | ZLAY_PLATFORM_WIN64)
```

---

## 4. Engine-Oriented Design

Pattern ini umum pada:

- Rendering backend
- Graphics abstraction
- VM runtime
- Engine core
- Hardware abstraction

---

# Recommended Future Extensions

## Architecture Detection

```cpp
#define ZLAY_ARCH_X86
#define ZLAY_ARCH_X64
#define ZLAY_ARCH_ARM
#define ZLAY_ARCH_ARM64
```

---

## Compiler Detection

```cpp
#define ZLAY_COMPILER_MSVC
#define ZLAY_COMPILER_CLANG
#define ZLAY_COMPILER_GCC
```

---

## Graphics Backend Detection

```cpp
#define ZLAY_RENDERER_OPENGL
#define ZLAY_RENDERER_VULKAN
#define ZLAY_RENDERER_METAL
#define ZLAY_RENDERER_DIRECTX
```

---

# Recommended Philosophy

Disarankan untuk:
- meminimalkan macro exposure
- memisahkan backend platform
- menggunakan abstraction layer
- lebih fokus ke capability detection daripada OS detection

Contoh:

```txt
platform/
 ├── win32/
 ├── x11/
 ├── cocoa/
 ├── wayland/
 └── android/
```

---

# Conclusion

Sistem ini memberikan:

- Cross-platform scalability
- Cleaner compile-time abstraction
- Modern engine architecture
- Minimal macro chaos
- Better maintainability

Pendekatan hexadecimal bitmask lebih cocok untuk:
- Engine development
- Middleware
- Runtime abstraction
- Native framework architecture

dibanding pendekatan traditional boolean macro yang cepat berkembang menjadi `macro hell`.