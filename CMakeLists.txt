cmake_minimum_required(VERSION 3.24)

project(ZLay LANGUAGES C CXX)

option(ZLAY_BUILD_EXAMPLES "Build ZLay examples" ON)
option(ZLAY_BUILD_DRIVERS "Build OpenGL/Vulkan drivers (stubs by default)" ON)
option(ZLAY_BUILD_OS "Build ZLay OS helpers (timer, etc.)" ON)
option(ZLAY_BUILD_RENDERER "Build ZLay renderer abstraction" ON)
option(ZLAY_BUILD_SHADERS "Build ZLay built-in shader sources" ON)
set(ZLAY_OS_BACKEND "auto" CACHE STRING "Platform backend: auto, x11, gtk, win32, winrt, cocoa")
set_property(CACHE ZLAY_OS_BACKEND PROPERTY STRINGS auto x11 gtk win32 winrt cocoa)

set(ZLAY_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/zlay)
set(ZLAY_SHADER_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/shader)

set(ZLAY_PUBLIC_INCLUDE_DIRS
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${ZLAY_SOURCE_DIR}
)

include_directories(${ZLAY_PUBLIC_INCLUDE_DIRS})

set(ZLAY_CORE_SOURCES
  ${ZLAY_SOURCE_DIR}/zlay_arena.c
  ${ZLAY_SOURCE_DIR}/zlay_core.c
  ${ZLAY_SOURCE_DIR}/zlay_hash.c
  ${ZLAY_SOURCE_DIR}/zlay_layout.c
  ${ZLAY_SOURCE_DIR}/zlay_render.c
  ${ZLAY_SOURCE_DIR}/zlay_style.c
)

set(ZLAY_CORE_HEADERS
  ${CMAKE_CURRENT_SOURCE_DIR}/zlay.h
  ${ZLAY_SOURCE_DIR}/zlay_internal.h
  ${ZLAY_SOURCE_DIR}/zlay_simd.h
)

set(ZLAY_COMPONENT_HEADERS
  ${ZLAY_SOURCE_DIR}/components/button.h
  ${ZLAY_SOURCE_DIR}/components/input.h
  ${ZLAY_SOURCE_DIR}/components/label.h
)

set(ZLAY_RENDERER_SOURCES
  ${ZLAY_SOURCE_DIR}/renderer/zlay_renderer.c
)

set(ZLAY_RENDERER_HEADERS
  ${ZLAY_SOURCE_DIR}/renderer/zlay_renderer.h
)

set(ZLAY_DRIVER_OPENGL_SOURCES
  ${ZLAY_SOURCE_DIR}/driver/opengl/zlay_opengl.c
)

set(ZLAY_DRIVER_OPENGL_HEADERS
  ${ZLAY_SOURCE_DIR}/driver/opengl/zlay_opengl.h
)

set(ZLAY_DRIVER_VULKAN_SOURCES
  ${ZLAY_SOURCE_DIR}/driver/vulkan/zlay_vulkan.c
)

set(ZLAY_DRIVER_VULKAN_HEADERS
  ${ZLAY_SOURCE_DIR}/driver/vulkan/zlay_vulkan.h
)

set(ZLAY_SHADER_LIBRARY_SOURCES
  ${ZLAY_SOURCE_DIR}/zlay_antialiasing.c
)

set(ZLAY_SHADER_LIBRARY_HEADERS
  ${ZLAY_SOURCE_DIR}/zlay_antialiasing.h
)

set(ZLAY_OS_HEADERS
  ${ZLAY_SOURCE_DIR}/os/zlay_os.h
)

set(ZLAY_OS_WIN32_SOURCES ${ZLAY_SOURCE_DIR}/os/win32/zlay_impl_win32.c)
set(ZLAY_OS_WINRT_SOURCES ${ZLAY_SOURCE_DIR}/os/winrt/zlay_impl_winrt.c)
set(ZLAY_OS_COCOA_SOURCES ${ZLAY_SOURCE_DIR}/os/cocoa/zlay_impl_cocoa.c)
set(ZLAY_OS_GTK_SOURCES ${ZLAY_SOURCE_DIR}/os/gtk/zlay_impl_gtk.c)
set(ZLAY_OS_X11_SOURCES ${ZLAY_SOURCE_DIR}/os/libx11/zlay_impl_x11.c)

set(ZLAY_OS_WIN32_HEADERS ${ZLAY_SOURCE_DIR}/os/win32/zlay_impl_win32.h)
set(ZLAY_OS_WINRT_HEADERS ${ZLAY_SOURCE_DIR}/os/winrt/zlay_impl_winrt.h)
set(ZLAY_OS_COCOA_HEADERS ${ZLAY_SOURCE_DIR}/os/cocoa/zlay_impl_cocoa.h)
set(ZLAY_OS_GTK_HEADERS ${ZLAY_SOURCE_DIR}/os/gtk/zlay_impl_gtk.h)
set(ZLAY_OS_X11_HEADERS ${ZLAY_SOURCE_DIR}/os/libx11/zlay_impl_x11.h)

if (ZLAY_BUILD_DRIVERS AND NOT ZLAY_BUILD_RENDERER)
  message(FATAL_ERROR "ZLAY_BUILD_DRIVERS requires ZLAY_BUILD_RENDERER because driver adapters implement the renderer interface")
endif()

if (ZLAY_BUILD_DRIVERS AND NOT ZLAY_BUILD_SHADERS)
  message(FATAL_ERROR "ZLAY_BUILD_DRIVERS requires ZLAY_BUILD_SHADERS because driver adapters use built-in shader sources")
endif()

add_library(zlay SHARED)
add_library(zlay::zlay ALIAS zlay)

target_compile_features(zlay PUBLIC c_std_99)
set_target_properties(zlay PROPERTIES
  C_STANDARD 99
  C_STANDARD_REQUIRED YES
  POSITION_INDEPENDENT_CODE ON
)

target_compile_definitions(zlay
  PRIVATE
    ZLAY_BUILDING
  PUBLIC
    ZLAY_SHARED
)

target_sources(zlay
  PRIVATE
    ${ZLAY_CORE_SOURCES}
    ${ZLAY_CORE_HEADERS}
    ${ZLAY_COMPONENT_HEADERS}
)

target_include_directories(zlay
  PUBLIC
    ${ZLAY_PUBLIC_INCLUDE_DIRS}
)

if (MSVC)
  target_compile_options(zlay PRIVATE /W4)
else()
  target_compile_options(zlay PRIVATE -Wall -Wextra -Wpedantic)
endif()

if (ZLAY_BUILD_RENDERER)
  target_sources(zlay PRIVATE ${ZLAY_RENDERER_SOURCES} ${ZLAY_RENDERER_HEADERS})

  add_library(zlay_renderer INTERFACE)
  add_library(zlay::renderer ALIAS zlay_renderer)
  target_link_libraries(zlay_renderer INTERFACE zlay)
endif()

if (ZLAY_BUILD_SHADERS)
  find_package(Python3 REQUIRED COMPONENTS Interpreter)

  set(ZLAY_SHADER_SOURCE_FILES
    ${ZLAY_SHADER_SOURCE_DIR}/common/antialiasing.glsl
    ${ZLAY_SHADER_SOURCE_DIR}/common/antialiasing.hlsl
    ${ZLAY_SHADER_SOURCE_DIR}/common/antialiasing.slang
    ${ZLAY_SHADER_SOURCE_DIR}/common/antialiasing.wgsl
    ${ZLAY_SHADER_SOURCE_DIR}/glsl/rect.vert.glsl
    ${ZLAY_SHADER_SOURCE_DIR}/glsl/rect.frag.glsl
    ${ZLAY_SHADER_SOURCE_DIR}/vulkan/rect.vert.glsl
    ${ZLAY_SHADER_SOURCE_DIR}/vulkan/rect.frag.glsl
    ${ZLAY_SHADER_SOURCE_DIR}/webgl/rect.vert.glsl
    ${ZLAY_SHADER_SOURCE_DIR}/webgl/rect.frag.glsl
    ${ZLAY_SHADER_SOURCE_DIR}/wgsl/rect.vert.wgsl
    ${ZLAY_SHADER_SOURCE_DIR}/wgsl/rect.frag.wgsl
    ${ZLAY_SHADER_SOURCE_DIR}/hlsl/rect.vert.hlsl
    ${ZLAY_SHADER_SOURCE_DIR}/hlsl/rect.frag.hlsl
    ${ZLAY_SHADER_SOURCE_DIR}/slang/rect.vert.slang
    ${ZLAY_SHADER_SOURCE_DIR}/slang/rect.frag.slang
  )

  set(ZLAY_GENERATED_DIR ${CMAKE_CURRENT_BINARY_DIR}/generated)
  set(ZLAY_SHADER_EMBED_FILE ${ZLAY_GENERATED_DIR}/zlay_shader_sources.inc)

  add_custom_command(
    OUTPUT ${ZLAY_SHADER_EMBED_FILE}
    COMMAND ${Python3_EXECUTABLE}
      ${CMAKE_CURRENT_SOURCE_DIR}/scripts/embed_shaders.py
      --root ${CMAKE_CURRENT_SOURCE_DIR}
      --output ${ZLAY_SHADER_EMBED_FILE}
    DEPENDS
      ${CMAKE_CURRENT_SOURCE_DIR}/scripts/embed_shaders.py
      ${ZLAY_SHADER_SOURCE_FILES}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Embedding ZLay shader sources"
    VERBATIM
  )

  target_sources(zlay PRIVATE ${ZLAY_SHADER_LIBRARY_SOURCES} ${ZLAY_SHADER_LIBRARY_HEADERS} ${ZLAY_SHADER_EMBED_FILE})
  target_include_directories(zlay PRIVATE ${ZLAY_GENERATED_DIR})

  add_library(zlay_shaders INTERFACE)
  add_library(zlay::shaders ALIAS zlay_shaders)
  target_link_libraries(zlay_shaders INTERFACE zlay)
endif()

if (ZLAY_BUILD_DRIVERS)
  target_sources(zlay PRIVATE ${ZLAY_DRIVER_OPENGL_SOURCES} ${ZLAY_DRIVER_OPENGL_HEADERS})
  target_sources(zlay PRIVATE ${ZLAY_DRIVER_VULKAN_SOURCES} ${ZLAY_DRIVER_VULKAN_HEADERS})

  add_library(zlay_driver_opengl INTERFACE)
  add_library(zlay::driver_opengl ALIAS zlay_driver_opengl)
  target_link_libraries(zlay_driver_opengl INTERFACE zlay)

  add_library(zlay_driver_vulkan INTERFACE)
  add_library(zlay::driver_vulkan ALIAS zlay_driver_vulkan)
  target_link_libraries(zlay_driver_vulkan INTERFACE zlay)
endif()

if (ZLAY_BUILD_OS)
  set(ZLAY_EFFECTIVE_OS_BACKEND "${ZLAY_OS_BACKEND}")
  if (ZLAY_EFFECTIVE_OS_BACKEND STREQUAL "auto")
    if (WIN32)
      set(ZLAY_EFFECTIVE_OS_BACKEND "win32")
    elseif(APPLE)
      set(ZLAY_EFFECTIVE_OS_BACKEND "cocoa")
    else()
      set(ZLAY_EFFECTIVE_OS_BACKEND "x11")
    endif()
  endif()

  if (ZLAY_EFFECTIVE_OS_BACKEND STREQUAL "win32")
    target_sources(zlay PRIVATE ${ZLAY_OS_HEADERS} ${ZLAY_OS_WIN32_SOURCES} ${ZLAY_OS_WIN32_HEADERS})
  elseif(ZLAY_EFFECTIVE_OS_BACKEND STREQUAL "winrt")
    target_sources(zlay PRIVATE ${ZLAY_OS_HEADERS} ${ZLAY_OS_WINRT_SOURCES} ${ZLAY_OS_WINRT_HEADERS})
  elseif(ZLAY_EFFECTIVE_OS_BACKEND STREQUAL "cocoa")
    target_sources(zlay PRIVATE ${ZLAY_OS_HEADERS} ${ZLAY_OS_COCOA_SOURCES} ${ZLAY_OS_COCOA_HEADERS})
  elseif(ZLAY_EFFECTIVE_OS_BACKEND STREQUAL "gtk")
    target_sources(zlay PRIVATE ${ZLAY_OS_HEADERS} ${ZLAY_OS_GTK_SOURCES} ${ZLAY_OS_GTK_HEADERS})
    if (UNIX AND NOT APPLE)
      target_link_libraries(zlay PRIVATE rt)
    endif()
  elseif(ZLAY_EFFECTIVE_OS_BACKEND STREQUAL "x11")
    target_sources(zlay PRIVATE ${ZLAY_OS_HEADERS} ${ZLAY_OS_X11_SOURCES} ${ZLAY_OS_X11_HEADERS})
    target_link_libraries(zlay PRIVATE rt)
  else()
    message(FATAL_ERROR "Unknown ZLAY_OS_BACKEND='${ZLAY_OS_BACKEND}'")
  endif()

  add_library(zlay_os INTERFACE)
  add_library(zlay::os ALIAS zlay_os)
  target_link_libraries(zlay_os INTERFACE zlay)
endif()

if (ZLAY_BUILD_EXAMPLES)
  add_executable(zlay_example_basic example/basic_cpp/main.cpp)
  target_compile_features(zlay_example_basic PRIVATE cxx_std_20)
  target_include_directories(zlay_example_basic PRIVATE ${ZLAY_PUBLIC_INCLUDE_DIRS})
  target_link_libraries(zlay_example_basic PRIVATE zlay)

  if (TARGET zlay_driver_opengl)
    target_link_libraries(zlay_example_basic PRIVATE zlay_driver_opengl)
  endif()

  if (TARGET zlay_renderer)
    target_link_libraries(zlay_example_basic PRIVATE zlay_renderer)
  endif()

  if (TARGET zlay_shaders)
    target_link_libraries(zlay_example_basic PRIVATE zlay_shaders)
  endif()

  if (TARGET zlay_os)
    target_link_libraries(zlay_example_basic PRIVATE zlay_os)
  endif()
endif()
