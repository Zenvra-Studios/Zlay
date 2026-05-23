# ZLay vcpkg Overlay

Run from the repository root:

```bash
vcpkg install zlay --overlay-ports=cmake/vcpkg
```

Optional features map to the same CMake options used by the native build:

```bash
vcpkg install "zlay[parser,renderer,shaders,drivers,os]" --overlay-ports=cmake/vcpkg
```

Consumers can then use:

```cmake
find_package(ZLay CONFIG REQUIRED)
target_link_libraries(app PRIVATE zlay::zlay)
```
