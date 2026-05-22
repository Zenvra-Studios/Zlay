# Arsitektur ZLay (Core + OpenGL/Vulkan Backend)

Dokumen ini adalah rujukan **arsitektur teknis** untuk implementasi ZLay: **style system + layout engine + render command list**, dengan backend render terpisah (OpenGL dulu, Vulkan menyusul).

- Ringkasan visi & positioning: `docs/architecture/ZLayout_Zenvra_Architecture.md`

---

## Daftar Isi

- [1. Scope, Tujuan, Non-Tujuan](#1-scope-tujuan-non-tujuan)
- [2. Data Flow (Kontrak Antar Modul)](#2-data-flow-kontrak-antar-modul)
- [3. Struktur Modul (Disarankan)](#3-struktur-modul-disarankan)
- [4. Public API (Immediate Mode)](#4-public-api-immediate-mode)
- [5. Identitas Elemen (ID) & State](#5-identitas-elemen-id--state)
- [6. Style System](#6-style-system)
- [7. Layout Engine](#7-layout-engine)
- [8. Render Command Builder](#8-render-command-builder)
- [9. Backend Renderer](#9-backend-renderer)
- [10. Text (Measure vs Render)](#10-text-measure-vs-render)
- [11. Struktur Folder Distribusi (Single Header Optional)](#11-struktur-folder-distribusi-single-header-optional)
- [12. Roadmap Implementasi (Taktis)](#12-roadmap-implementasi-taktis)

---

## 1. Scope, Tujuan, Non-Tujuan

### 1.1 Scope (yang dibangun)

- Core: node tree, style, layout, render command output, input state dasar.
- Backend: OpenGL renderer MVP, Vulkan renderer (fase lanjut).
- Pola: immediate mode (beginFrame/endFrame).

### 1.2 Tujuan MVP

- API chaining yang nyaman.
- Flex row/column yang stabil + absolute/floating.
- Output render commands (tanpa dependency backend).
- Demo UI: container + sidebar + content + text + button.

### 1.3 Non-Tujuan MVP

- Full CSS parser + selector engine kompleks.
- Retained DOM framework besar.
- Animation system penuh.
- Accessibility lengkap.

---

## 2. Data Flow (Kontrak Antar Modul)

```text
UI Build (API) -> Node Tree + Style
                -> Style Resolve (inherit/theme/state)
                -> Layout Compute (rect per node)
                -> Render Command Build (flatten + clip + z)
                -> Backend Render (OpenGL/Vulkan)
```

Kontrak paling penting: **Core hanya mengeluarkan `RenderCommandList`**, bukan menggambar langsung.

---

## 3. Struktur Modul (Disarankan)

> Ini struktur logis; boleh single-header di level distribusi, tetapi tetap pisahkan modul internal.

```text
zlay/
  core/        (types, ids, memory, context)
  style/       (style props, theme tokens, state)
  layout/      (flex + absolute + measure hooks)
  render/      (render commands + clip + sorting)
  input/       (mouse/keyboard state, hit-test)
  backend/
    opengl/    (OpenGL renderer)
    vulkan/    (Vulkan renderer)
  widgets/     (optional; sugar di atas core)
```

---

## 4. Public API (Immediate Mode)

Frame lifecycle:

```cpp
ui.beginFrame(screenW, screenH, input);
// build UI
RenderCommandList commands = ui.endFrame();
renderer.render(commands);
```

Prinsip API:

- builder returns reference (chaining).
- element identity stabil lewat `id` (string/hash).
- API punya versi verbose + alias pendek.

---

## 5. Identitas Elemen (ID) & State

Immediate mode butuh cara menyimpan state antar frame:

- `ElementId` = hash dari string id + parent path (opsional).
- State per id (hovered, active, focus) disimpan di context.

Sketsa:

```text
frame:
  build nodes -> assign ElementId
  resolve interaction -> update state maps
```

---

## 6. Style System

### 6.1 Model Style

Gunakan representasi style yang eksplisit (no string parsing di MVP).

Kategori minimal:

- layout: width/height/min/max, grow, percent
- spacing: margin/padding/gap
- positioning: relative/absolute/floating + offset
- visuals: bg, border, radius
- text: color, fontSize (measurement via font module)

### 6.2 Theme Tokens (disarankan sejak awal)

Agar UI konsisten dan mudah diganti theme:

```text
--color-bg
--color-panel
--color-text
--radius-md
--space-2
--space-4
```

MVP: token bisa berupa lookup sederhana (map).

---

## 7. Layout Engine

### 7.1 Input/Output

- Input: node tree + resolved style + measure callbacks (text)
- Output: `Rect` per node + clip rect (opsional)

### 7.2 Flex Column (MVP)

Aturan baseline:

- child.x mengikuti parent + paddingLeft
- child.y mengikuti cursorY
- child.width: resolve fixed/percent/grow/auto
- child.height: resolve fixed/percent/grow/auto
- cursorY bertambah `child.height + gap`

### 7.3 Flex Row (MVP)

Mirip column, tetapi cursor bergerak di X.

### 7.4 Absolute/Floating

- `absolute`: posisi relatif parent content box.
- `floating`: posisi relatif screen + `zIndex` (di-render terpisah dari normal flow).

---

## 8. Render Command Builder

### 8.1 Command Types (minimal)

```text
Rect, Border, Text, Image, ClipBegin, ClipEnd
```

### 8.2 Sorting & Clipping

- Normal flow: stable order by build sequence.
- Floating: sort by `zIndex` (ascending).
- Clip stack: push/pop sesuai node yang menjadi clip container.

---

## 9. Backend Renderer

### 9.1 Interface

Backend mengkonsumsi `RenderCommandList` dan punya resource cache:

- shader pipeline
- vertex/index buffers
- font atlas textures
- image textures

### 9.2 OpenGL MVP Milestone

Urutan aman:

1. Rect (solid)
2. Border
3. Rounded rect (approx / shader)
4. Clip (scissor)
5. Text (bitmap atlas)
6. Image

### 9.3 Vulkan Phase

Masuk setelah:

- command list stabil
- batching strategy sudah jelas
- font atlas path sudah matang

---

## 10. Text (Measure vs Render)

Pemisahan wajib:

- `measure(text, font, size) -> (w, h)` dipakai layout engine.
- `render(text, rect, font)` dipakai backend.

MVP dapat mulai dari text rendering sederhana; shaping/IME menjadi phase lanjutan.

---

## 11. Struktur Folder Distribusi (Single Header Optional)

Jika tetap ingin single-header untuk distribusi:

```text
zlay.h
zlay/
  zlay_core.c
  zlay_layout.c
  zlay_style.c
  zlay_render.c
  renderer/
  driver/
  os/
include/
  helper tutorial/example
examples/
tests/
docs/
```

Internal modul dipisah di `zlay/` demi maintainability, sementara root project tetap mengekspor `#include <zlay.h>` dan public backend include lewat CMake.

---

## 12. Roadmap Implementasi (Taktis)

```text
1) Core types + context + ids
2) Style struct + builder API
3) Layout flex column + row
4) RenderCommand build (Rect only)
5) OpenGL rect renderer
6) Padding/margin/gap + bg/border/radius
7) Hit-test + hover/click
8) Clipping + scroll container
9) Text measure + render
10) Floating window + zIndex
11) Vulkan backend
```
