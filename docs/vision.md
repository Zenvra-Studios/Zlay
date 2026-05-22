# Visi ZLay

ZLay dibuat sebagai pondasi layout dan styling untuk GUI native yang bisa masuk ke banyak ranah tanpa membawa runtime web di dalamnya.

Yang ingin dibangun bukan sekadar library untuk menggambar kotak, tetapi lapisan dasar yang bisa dipakai untuk membuat antarmuka modern pada engine, editor, tools, viewport graphics, panel CAD-like, game tooling, synthesizer UI, bitmap synthesis, debug HUD, overlay, dan berbagai bentuk graphics user interface lain.

Inti idenya sederhana:

```text
layout + style + render commands
```

ZLay tidak membutuhkan CSS runtime, tidak membutuhkan DOM, tidak membutuhkan JavaScript untuk memanipulasi UI, dan tidak bergantung pada WebKit atau Chromium. Kalau suatu saat ZLay tampil di environment WebGPU, targetnya tetap sama: core ZLay tetap menghasilkan command dan data render, lalu backend WebGPU yang menerjemahkan data itu ke pipeline grafis. WebGPU adalah target renderer, bukan alasan untuk memasukkan CSS atau scripting web ke dalam core.

## Kenapa Bukan CSS/WebKit

CSS dan WebKit bagus untuk web, tetapi ZLay memilih jalur berbeda.

ZLay ingin menjadi sistem UI yang:

- kecil dan mudah ditanam ke engine,
- bisa berjalan dekat dengan renderer,
- dapat dikontrol dari C/C++,
- tidak membawa browser engine,
- tidak membutuhkan scripting DOM,
- bisa dipakai di aplikasi graphics-heavy,
- mudah dipindahkan ke backend render baru.

Dengan cara ini, UI tidak menjadi “hal asing” di atas engine. UI menjadi bagian dari pipeline grafis itu sendiri.

## Ranah yang Ingin Disentuh

ZLay diarahkan agar bisa tumbuh ke banyak konteks:

- game engine editor,
- graphics user interface untuk viewport 2D/3D,
- CAD-like layout panels,
- node editor,
- property inspector,
- timeline, graph editor, dan profiler UI,
- synthesizer dan audio tool UI,
- bitmap synthesis dan procedural image tooling,
- software rasterized debug UI,
- GPU accelerated desktop tool,
- embedded tool UI,
- command palette,
- layouting untuk overlay dan HUD.

Istilah besarnya: ZLay ingin menjadi pondasi GUI native untuk dunia graphics programming, bukan framework aplikasi web yang dibawa masuk ke native.

## Renderer Agnostic

Core ZLay tidak boleh terkunci ke satu graphics API.

Backend yang masuk akal:

```text
OpenGL
Vulkan
WebGPU
Metal
Direct3D 11
Direct3D 12
software rasterizer
custom engine renderer
```

Karena output core berupa `ZLay_RenderCommandList`, backend renderer bisa dibuat bertahap. Hari ini OpenGL dan Vulkan bisa menjadi awal. Nanti WebGPU dapat masuk sebagai driver baru tanpa mengubah cara layout bekerja.

## Platform Agnostic

ZLay juga tidak boleh terkunci ke satu OS.

Platform backend dapat diarahkan ke:

```text
Win32
WinRT
Cocoa / macOS
GTK
X11
Wayland
POSIX Unix
BSD family
Solaris / illumos
Android
iOS
Emscripten
```

Platform backend bertugas menjembatani window, input, DPI, timer, clipboard, cursor, dan style info native. Core layout tetap sama.

## Bentuk Akhir yang Diinginkan

ZLay idealnya terasa seperti pondasi kecil yang tajam:

- API C yang stabil dan bisa dipakai dari C++.
- Struktur seperti zlib: kecil, jelas, portable, dan tidak cerewet.
- Gaya layout seperti Clay: deklaratif, immediate, dan renderer-agnostic.
- Backend seperti ImGui: platform backend terpisah dari renderer backend.
- Styling modern tanpa harus membawa CSS engine.

Arahnya bukan membuat “web baru” di dalam engine, tetapi membuat bahasa layout native yang cukup bersih untuk dipakai di mana saja.

## Prinsip Jangka Panjang

- Core harus tetap portable C.
- Renderer backend boleh banyak, tetapi core tidak boleh tahu detail GPU API.
- Platform backend boleh banyak, tetapi core tidak boleh tahu detail OS.
- Styling harus data-oriented, bukan string parser yang berat.
- Parser CSS-like boleh ada nanti, tetapi opsional dan bukan runtime wajib.
- WebGPU boleh masuk sebagai backend renderer, bukan sebagai dependency web scripting.
- Semua fitur besar harus tumbuh dari command list, style object, layout tree, dan backend interface yang bersih.

