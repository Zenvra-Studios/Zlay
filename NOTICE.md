# Third-party Notices

ZLay is licensed under the GNU General Public License v3.0 or later.
See `LICENSE`.

This project includes or integrates with third-party code and platform
dependencies listed below. ZLay public APIs should remain under the `ZLay_*`
facade; third-party APIs such as `Clay_*` are used internally unless a backend
header explicitly documents otherwise.

## Bundled Third-party Source

### Clay

- Project: Clay
- Author: Nic Barker
- Repository: https://github.com/nicbarker/clay
- Version in tree: 0.14
- License: zlib/libpng
- Location: `lib/clay.h`
- Preserved license text: `lib/CLAY_LICENSE.md` and the license footer in
  `lib/clay.h`
- Usage: optional internal layout backend through `backend/clay/z_clay_backend.c`
  and `backend/clay/z_clay_backend.h`

Clay is not authored by Zenvra Studio or ZLay contributors. If the vendored
`lib/clay.h` file is modified, the modified version must be plainly marked as
modified and must keep the original Clay notice.

### stb_image

- Project: stb_image
- Author: Sean Barrett and contributors
- Repository: https://github.com/nothings/stb
- Version in tree: 2.30
- License: dual licensed, MIT License or Public Domain/Unlicense
- Location: `lib/stb_image.h`
- Usage: image loading support for renderer/tooling code.

The original license notice is preserved at the end of `lib/stb_image.h`.

### stb_truetype

- Project: stb_truetype
- Author: Sean Barrett / RAD Game Tools and contributors
- Repository: https://github.com/nothings/stb
- Version in tree: 1.26
- License: dual licensed, MIT License or Public Domain/Unlicense
- Location: `lib/stb_truetype.h`
- Usage: TrueType font parsing, glyph metrics, and glyph rasterization support.

The original license notice is preserved in `lib/stb_truetype.h`.

### json.h

- Project: json.h
- Author/maintainer: Sheredom
- Repository: https://github.com/sheredom/json.h
- License: Public Domain/Unlicense
- Location: `lib/json.h`
- Usage: JSON parsing and writing support.

The original public domain notice is preserved at the top of `lib/json.h`.

### xxHash

- Project: xxHash
- Author: Yann Collet
- Repository: https://github.com/Cyan4973/xxHash
- License: BSD 2-Clause
- Location: `lib/xxhash.h`
- Usage: fast non-cryptographic hashing support.

The original BSD 2-Clause notice is preserved at the top of `lib/xxhash.h`.

## External Dependencies and Platform APIs

The following dependencies or APIs are referenced by the build, examples, or
platform backends, but their source code is not bundled in this repository:

- C standard library and C++ standard library
- POSIX/Pthreads where available
- System math library (`m`) on Unix-like targets
- Realtime library (`rt`) on supported Unix-like targets
- X11 for Linux/X11 windowing and examples
- GTK for optional Linux GTK backend code
- Win32, WinRT, COM, and OLE APIs for Windows platform support
- Cocoa, Foundation, QuartzCore, and UIKit for Apple platform support
- OpenGL and Vulkan APIs for renderer driver backends
- Metal, Direct3D, and WebGPU are represented as renderer backend targets or
  abstractions, but are not bundled here
- Python 3 is used as a build-time tool for shader source embedding
- vcpkg packaging metadata is provided under `cmake/vcpkg/zlay`

Licenses for these external dependencies are provided by their respective
system packages, SDKs, or vendors.

## Project Packaging Metadata

- `cmake/vcpkg/zlay/copyright` contains ZLay package copyright metadata.
- `lib/CLAY_LICENSE.md` preserves the Clay license as a standalone notice file.

## Modification Policy

When third-party source files are updated or modified:

- keep the upstream copyright and license notices intact;
- mark modified vendored files clearly when the upstream license requires it;
- update this `NOTICE.md` if a dependency is added, removed, relocated, or its
  license changes;
- do not expose third-party layout APIs directly as ZLay's user-facing API when
  a ZLay facade can be used instead.
