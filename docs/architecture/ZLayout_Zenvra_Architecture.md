# ZLay / ZLayout — Vision & Arsitektur Tingkat Tinggi

Dokumen ini menjadi pondasi arah arsitektural untuk **library layout & styling native C++** yang ditargetkan untuk **tools/engine editor** berbasis **OpenGL/Vulkan** (graphics-heavy desktop apps).

> Catatan: Penamaan di dokumen ini menggunakan **ZLay** sebagai nama library, dan **ZLayout** sebagai istilah/konsep sistem layout.

- Lanjutan (arsitektur teknis): `docs/architecture/arsitektur_library_ui_layout_opengl_vulkan.md`
- Narasi visi lintas platform/renderer: `docs/vision.md`

---

## Daftar Isi

- [1. Ringkasan (TL;DR)](#1-ringkasan-tldr)
- [2. Tujuan & Non-Tujuan](#2-tujuan--non-tujuan)
- [3. Positioning](#3-positioning)
- [4. Prinsip Arsitektur](#4-prinsip-arsitektur)
- [5. Layering (High-Level)](#5-layering-high-level)
- [6. Konsep Inti](#6-konsep-inti)
- [7. Renderer Interface (Kontrak)](#7-renderer-interface-kontrak)
- [8. API Feel (CSS-like, Native C++)](#8-api-feel-css-like-native-c)
- [9. Komponen Penting untuk Tools/CAD-like UI](#9-komponen-penting-untuk-toolscad-like-ui)
- [10. Risiko Teknis (Yang Sulit)](#10-risiko-teknis-yang-sulit)
- [11. Roadmap Aman (Fase)](#11-roadmap-aman-fase)
- [12. Definisi Selesai (Definition of Done) untuk “Pondasi”](#12-definisi-selesai-definition-of-done-untuk-pondasi)

---

## 1. Ringkasan (TL;DR)

ZLay adalah **layout + styling core** untuk UI native di aplikasi yang punya viewport graphics besar (engine editor, CAD-like tools, dsb).

- Fokus: layout, styling, render command output, input dasar, widget dasar.
- Bukan fokus: WebView/Chromium, HTML/CSS runtime, “Flutter/Qt killer”.
- Backend: renderer-agnostic (OpenGL/Vulkan dulu, WebGPU/Metal/D3D/software/custom engine renderer bisa menyusul).

---

## 2. Tujuan & Non-Tujuan

### 2.1 Tujuan

- Menyediakan **layout engine** (flex baseline, absolute/floating) yang stabil.
- Menyediakan **style system** yang terasa “CSS-like” namun idiomatik C++.
- Menghasilkan **render command list** yang mudah di-render oleh backend (OpenGL/Vulkan).
- Menjadi pondasi untuk UI tools: docking, panel, inspector, viewport overlay.

### 2.2 Non-Tujuan (fase awal)

- Tidak menargetkan full framework aplikasi desktop multi-platform lengkap.
- Tidak membangun runtime HTML/CSS/JS atau WebView.
- Tidak memaksakan “semua renderer sekaligus” sejak awal.
- Tidak mengejar widget ecosystem besar di v0.x.

---

## 3. Positioning

### 3.1 Masalah yang ingin dipecahkan

UI desktop berbasis WebView/Electron cepat dari sisi UI/UX, tetapi sering:

- berat (startup/memory/dependency),
- sulit menyatu dengan viewport OpenGL/Vulkan,
- terbatas ketika butuh kontrol render level rendah.

### 3.2 Nilai utama ZLay

- UI native yang **ringan** dan **renderer-friendly**.
- Integrasi kuat dengan viewport graphics (overlay, gizmo, tool panels).
- Arsitektur modular: core tidak bergantung langsung pada OpenGL/Vulkan.

---

## 4. Prinsip Arsitektur

- **Renderer agnostic**: core tidak tahu OpenGL/Vulkan; hanya output command.
- **Immediate mode first**: UI dibangun ulang tiap frame untuk MVP.
- **Data-oriented**: node/command disimpan pada arena/arrays untuk performa.
- **Separation of concerns**: style → layout → render command → backend render.
- **Small core, grow by modules**: docking/widgets/theme datang setelah core stabil.

---

## 5. Layering (High-Level)

```text
App / Tools
  |
  v
Public API (chaining builder / macros / convenience)
  |
  v
Style System (properties + states + theme tokens)
  |
  v
Layout Engine (flex baseline + absolute/floating)
  |
  v
Render Command Builder (flatten + clip + z-sort)
  |
  +----------------------+
  |                      |
  v                      v
OpenGL Backend        Vulkan Backend
```

---

## 6. Konsep Inti

### 6.1 Node Tree

- UI dibentuk sebagai tree node (container/leaf).
- Setiap node punya style, dan hasil layout (rect).

### 6.2 Style System

Target properti awal:

- sizing: `width/height`, `min/max`, `percent`, `grow`
- spacing: `margin/padding/gap`
- visual: `background`, `border`, `radius`
- position: `relative`, `absolute`, `floating`, `z-index`
- text basics: `color`, `fontSize` (layout butuh measurement)

### 6.3 Layout Engine

Layout engine:

- hanya menghitung posisi/ukuran,
- tidak menggambar,
- deterministik dan mudah dites.

### 6.4 Render Commands

Output akhir core berupa array command (contoh):

```text
Rect / Border / Text / Image / ClipBegin / ClipEnd
```

Backend renderer cukup mengkonsumsi command ini.

---

## 7. Renderer Interface (Kontrak)

Renderer backend wajib bisa mengerjakan minimal:

- rect + border + rounded rect
- text (dengan atlas/font module)
- clipping (scissor/clip stack)
- z-ordering (atau menerima command yang sudah terurut)

Kontrak ideal:

```cpp
struct ZLayRenderer {
    virtual void beginFrame() = 0;
    virtual void render(const RenderCommandList& commands) = 0;
    virtual void endFrame() = 0;
    virtual ~ZLayRenderer() = default;
};
```

---

## 8. API Feel (CSS-like, Native C++)

Target rasa API:

- chaining
- readable (nama panjang) + opsional alias (nama pendek)
- aman untuk immediate mode (beginFrame/endFrame)

Contoh arah:

```cpp
ui.box("card")
  .w(320).h(180)
  .bg("#1e293b").rounded(16)
  .p(16).gap(8)
  .flexCol();
```

---

## 9. Komponen Penting untuk Tools/CAD-like UI

Ini bukan target v0.1, tapi menjadi “north star”:

- docking/floating windows
- panels (inspector, hierarchy, properties)
- scroll container
- viewport overlay (gizmo, grid, selection outline)
- numeric input + unit display
- command palette

---

## 10. Risiko Teknis (Yang Sulit)

Area yang biasanya paling berat di UI native:

- text rendering + shaping + fallback
- text input (selection, IME, clipboard)
- DPI scaling + multi-window
- layout invalidation & perf (batching)

Untuk MVP, boleh memakai dependensi (mis. FreeType/stb_truetype, HarfBuzz nanti).

---

## 11. Roadmap Aman (Fase)

```text
Phase 1: Core + Style + Flex (no text shaping)
Phase 2: OpenGL backend MVP (rect/border/text basic)
Phase 3: Interaction basics (hover/click/focus)
Phase 4: Scroll + clipping + z-index
Phase 5: Docking/floating windows
Phase 6: Vulkan backend
Phase 7: Widgets lebih kaya + tooling (inspector/debug overlay)
```

---

## 12. Definisi Selesai (Definition of Done) untuk “Pondasi”

Pondasi dianggap matang ketika:

- style + layout menghasilkan output konsisten & teruji
- command list deterministic & mudah di-debug
- backend OpenGL dapat render UI demo (sidebar + viewport + inspector)
- input dasar (hover/click) stabil
