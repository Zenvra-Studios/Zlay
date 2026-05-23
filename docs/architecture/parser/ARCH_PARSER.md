# ZLay Parser PRD & Architecture

## Overview

ZLay adalah library layout & styling berbasis C yang dirancang untuk:

- Cross-platform
- Vulkan/OpenGL renderer backend
- Declarative layout
- Immediate/declarative hybrid API
- Static/shared library
- vcpkg friendly
- Tanpa WebKit
- Tanpa format binary baru

Parser hanya sebagai optional frontend layer untuk membantu membangun tree UI dari syntax text.

---

# Goals

## Primary Goals

- Pure C core API
- Parser optional
- Stable C ABI
- Static/shared library support
- Renderer agnostic
- Layout + style engine
- Easy embedding into engine (Zenvra)

---

# Non Goals

```txt
❌ Full HTML engine
❌ Full CSS engine
❌ WebKit
❌ Binary UI format
❌ Flutter runtime clone
❌ JavaScript engine
❌ Heavy dependencies
```

---

# Architecture Overview

```txt
Source Text
↓
Lexer
↓
Parser
↓
AST
↓
Semantic Validation
↓
Resolver
↓
ZLay Core API
↓
Render Commands
↓
Renderer Backend (OpenGL/Vulkan)
```

---

# Project Structure

```txt
zlay/
├── include/
│   └── zlay/
│       ├── zlay.h
│       ├── zlay_math.h
│       └── zlay_parser.h
│
├── src/
│   ├── core/
│   ├── layout/
│   ├── render/
│   └── parser/
│
├── examples/
├── tests/
├── CMakeLists.txt
└── vcpkg.json
```

---

# Parser Structure

```txt
src/parser/
├── lexer/
├── ast/
├── syntax/
├── semantic/
├── resolver/
└── serializer/
```

Runtime tidak tinggal di parser. Runtime adalah pondasi core/bridge dan berada
di `zlay/zlay_runtime.h`, supaya bisa menghubungkan parser, event, renderer,
driver, OS, dan UI API tanpa membuat parser menjadi pemilik runtime.

Minimum implementation:

```txt
src/parser/
├── lexer/
├── ast/
├── syntax/
└── resolver/
```

---

# 1. Lexer

## Purpose

Convert source text into tokens.

Example:

```txt
Box {
  width: 100%
  padding: 24
}
```

Tokenized into:

```txt
IDENT(Box)
LBRACE
IDENT(width)
COLON
NUMBER(100)
PERCENT
IDENT(padding)
COLON
NUMBER(24)
RBRACE
```

---

## Folder

```txt
lexer/
├── zlay_token.h
├── zlay_token.c
├── zlay_lexer.h
└── zlay_lexer.c
```

---

## Token Types

```c
typedef enum ZLay_TokenType {
    ZLAY_TOKEN_EOF,

    ZLAY_TOKEN_IDENT,
    ZLAY_TOKEN_STRING,
    ZLAY_TOKEN_NUMBER,
    ZLAY_TOKEN_COLOR,

    ZLAY_TOKEN_LBRACE,
    ZLAY_TOKEN_RBRACE,

    ZLAY_TOKEN_LPAREN,
    ZLAY_TOKEN_RPAREN,

    ZLAY_TOKEN_COLON,
    ZLAY_TOKEN_COMMA,
    ZLAY_TOKEN_DOT,
    ZLAY_TOKEN_HASH,
    ZLAY_TOKEN_PERCENT,

    ZLAY_TOKEN_TRUE,
    ZLAY_TOKEN_FALSE,
    ZLAY_TOKEN_NULL
} ZLay_TokenType;
```

---

# 2. AST

## Purpose

Temporary syntax tree before entering runtime.

Example:

```txt
Box
├── width = 100%
├── padding = 24
└── Text
    └── value = "Hello"
```

---

## Folder

```txt
ast/
├── zlay_ast.h
├── zlay_ast.c
├── zlay_node.h
├── zlay_node.c
├── zlay_value.h
└── zlay_value.c
```

---

## Value Types

```c
typedef enum ZLay_ValueType {
    ZLAY_VALUE_NULL,
    ZLAY_VALUE_STRING,
    ZLAY_VALUE_NUMBER,
    ZLAY_VALUE_BOOL,
    ZLAY_VALUE_COLOR,
    ZLAY_VALUE_IDENT,
    ZLAY_VALUE_PERCENT
} ZLay_ValueType;
```

---

## AST Property

```c
typedef struct ZLay_AstProp {
    ZLay_String name;
    ZLay_Value value;
} ZLay_AstProp;
```

---

## AST Node

```c
typedef struct ZLay_AstNode {
    ZLay_String type;

    ZLay_AstProp* props;
    uint32_t prop_count;

    struct ZLay_AstNode* children;
    uint32_t child_count;

    uint32_t line;
    uint32_t column;
} ZLay_AstNode;
```

---

# 3. Syntax Parser

## Purpose

Convert tokens into AST.

---

## Folder

```txt
syntax/
├── zlay_parser.h
├── zlay_parser.c
├── zlay_parse_error.h
└── zlay_parse_error.c
```

---

## Grammar

```txt
document    = node*
node        = IDENT "{" item* "}"
item        = property | node
property    = IDENT ":" value
value       = STRING | NUMBER | BOOL | COLOR | IDENT | PERCENT
```

---

## Example Syntax

```txt
Box {
  id: "root"
  width: 100%
  height: 100%
  padding: 24

  Text {
    id: "title"
    value: "Hello ZLay"
  }
}
```

---

# 4. Semantic Validation

## Purpose

Validate AST correctness.

Example invalid syntax:

```txt
radius: "large"
```

Should produce:

```txt
radius expects number
```

---

## Folder

```txt
semantic/
├── zlay_semantic.h
├── zlay_semantic.c
├── zlay_schema.h
└── zlay_schema.c
```

---

## Validations

```txt
width        -> number / percent / auto / grow
height       -> number / percent / auto / grow
padding      -> number
radius       -> number
background   -> color
color        -> color
direction    -> row / column
value        -> string
```

---

# 5. Resolver

## Purpose

Convert AST into ZLay runtime API calls.

```txt
AST
↓
ZLay_NodeDeclaration
↓
ZLay__OpenElement()
↓
ZLay__ConfigureOpenElement()
↓
ZLay__CloseElement()
```

Resolver juga menyediakan registry widget agar engine/app dapat menambahkan
komponen deklaratif baru tanpa mengubah parser core. Builtin mapping tetap
minimal (`Box`, `Row`, `Column`, `Text`, `Button`), sementara tipe lain bisa
di-handle lewat `ZLay_ResolverRegister()`.

---

## Folder

```txt
resolver/
├── zlay_resolver.h
├── zlay_resolver.c
├── zlay_style_resolver.h
└── zlay_style_resolver.c
```

String yang dipakai node render disalin ke frame arena ZLay saat resolver
berjalan. Dengan begitu API one-shot `ZLay_BuildFromString()` tetap aman untuk
pola:

```c
ZLay_BeginLayout(ctx);
ZLay_BuildFromString(ctx, source, &result);
ZLay_RenderCommandList cmds = ZLay_EndLayout(ctx);
```

---

## Node Mapping

```txt
Box  -> ZLAY_NODE_BOX
Text -> ZLAY_NODE_TEXT
```

---

## Style Mapping

```txt
width       -> style.width
height      -> style.height
padding     -> padding_left/top/right/bottom
gap         -> style.gap
direction   -> style.flex_direction
background  -> style.background
color       -> style.text_color
radius      -> style.radius
```

---

# 6. Runtime Bridge

## Purpose

Core bridge untuk fitur dinamis dan integrasi lintas modul ZLay.

Runtime bukan bagian dari parser. Parser hanya menghasilkan AST dan resolver
mengubah AST menjadi pemanggilan ZLay core. Runtime berada satu level di folder
`zlay/` agar bisa dipakai oleh frontend lain selain parser.

---

## Folder

```txt
zlay/
├── zlay_runtime.h
└── event/
    └── zlay_runtime.h   // compatibility include ke <zlay_runtime.h>
```

---

## Future Features

```txt
onClick
state binding
hot reload
component registry
animation
```

---

# 7. Serializer (Optional)

## Purpose

Autosave/recovery/state snapshot.

No binary formats.

Allowed formats:

```txt
JSON
plain text
memory snapshot
```

---

## Folder

```txt
serializer/
├── zlay_json.h
├── zlay_json.c
├── zlay_snapshot.h
└── zlay_snapshot.c
```

---

# Recommended DSL Syntax

```txt
Window {
  title: "Zenvra"

  Box {
    id: "root"
    width: 100%
    height: 100%
    padding: 24
    direction: column
    background: #09090B

    Text {
      id: "title"
      value: "Zenvra Engine"
      color: #FFFFFF
    }

    Box {
      id: "panel"
      width: 380
      height: auto
      padding: 20
      radius: 18
      background: #18181B
    }
  }
}
```

---

# Public Parser API

```c
typedef struct ZLay_ParseResult {
    bool success;

    ZLay_AstNode* root;

    ZLay_String error_message;

    uint32_t error_line;
    uint32_t error_column;
} ZLay_ParseResult;
```

---

## Parse API

```c
ZLAY_API ZLay_ParseResult ZLay_Parse(
    ZLay_Arena* arena,
    ZLay_String source
);
```

---

## Build API

```c
ZLAY_API bool ZLay_BuildFromAst(
    ZLay_Context* ctx,
    const ZLay_AstNode* root
);
```

## Resolver Registry

```c
typedef bool (*ZLay_WidgetBuildFn)(
    ZLay_Resolver* resolver,
    ZLay_Context* ctx,
    const ZLay_AstNode* node,
    void* user
);

ZLAY_API void ZLay_ResolverInit(ZLay_Resolver* resolver);
ZLAY_API bool ZLay_ResolverRegister(
    ZLay_Resolver* resolver,
    const char* type,
    ZLay_WidgetBuildFn build,
    void* user
);
```

---

## High-Level Build API

```c
ZLAY_API bool ZLay_BuildFromString(
    ZLay_Context* ctx,
    ZLay_String source,
    ZLay_ParseResult* out_result
);
```

---

# Example Usage

```c
ZLay_BeginLayout(ctx);

ZLay_BuildFromString(
    ctx,
    ZLAY_STRING_LITERAL(
        "Box {"
        "  width: 100%"
        "  height: 100%"
        "  Text { value: \"Hello\" }"
        "}"
    ),
    &result
);

ZLay_RenderCommandList cmds = ZLay_EndLayout(ctx);
```

---

# Renderer Pipeline

```txt
Source Text
↓
Lexer
↓
Parser
↓
AST
↓
Semantic Validation
↓
Resolver
↓
ZLay Core
↓
Render Commands
↓
OpenGL/Vulkan Backend
```

---

# Build System

## CMake Options

```cmake
option(ZLAY_ENABLE_PARSER "Enable parser module" ON)
option(ZLAY_BUILD_SHARED "Build shared library" OFF)
option(ZLAY_BUILD_EXAMPLES "Build examples" ON)
option(ZLAY_BUILD_TESTS "Build tests" OFF)
```

---

# vcpkg Requirements

```txt
✅ install()
✅ static/shared support
✅ public headers
✅ no binary assets
✅ clean C API
✅ release tags
✅ cross-platform build
```

---

# Install Layout

```txt
include/zlay/zlay.h
include/zlay/zlay_math.h
include/zlay/zlay_parser.h

lib/libzlay.a

share/zlay/zlayTargets.cmake
```

---

# Final Design Philosophy

```txt
Core Engine:
Pure C
Stable ABI
Renderer agnostic

Parser:
Optional frontend layer
No binary formats
No CSS engine
No WebKit

Renderer:
Command list based
OpenGL/Vulkan backend

Usage:
Manual API or DSL parser
```
