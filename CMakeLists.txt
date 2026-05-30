cmake_minimum_required(VERSION 3.24)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE BOOL "Export compile_commands.json for clangd and other language servers")

project(Zlay VERSION 0.1.0 LANGUAGES C CXX)

include(GNUInstallDirs)

option(BUILD_SHARED_LIBS "Build ZLay as a shared library instead of static" ON)
option(ZLAY_BUILD_EXAMPLES "Build ZLay examples" ON)
option(ZLAY_BUILD_DRIVERS "Build OpenGL/Vulkan drivers (stubs by default)" ON)
option(ZLAY_BUILD_OS "Build ZLay OS helpers (timer, etc.)" ON)
option(ZLAY_BUILD_LIFECYCLE "Build ZLay lifecycle, autosave, journal, and recovery helpers" ON)
option(ZLAY_BUILD_PARSER "Build ZLay DSL parser frontend" ON)
option(ZLAY_BUILD_RENDERER "Build ZLay renderer abstraction" ON)
option(ZLAY_BUILD_SHADERS "Build ZLay built-in shader sources" ON)
option(ZLAY_ENABLE_COMPONENTS "Enable Zlay C++ component layer" ON)
option(ZLAY_ENABLE_COMPONENTS_META "Enable metaprogramming utilities" ON)
option(ZLAY_ENABLE_COMPONENTS_TOOLING "Enable engine tooling components" ON)
option(ZLAY_ENABLE_COMPONENTS_VIEWPORT "Enable viewport components" ON)
option(ZLAY_ENABLE_COMPONENTS_SCENE "Enable scene components" ON)
option(ZLAY_ENABLE_COMPONENTS_INSPECTOR "Enable inspector components" ON)
option(ZLAY_ENABLE_COMPONENTS_ASSETS "Enable asset components" ON)
option(ZLAY_ENABLE_COMPONENTS_GRAPH "Enable graph components" ON)
option(ZLAY_ENABLE_COMPONENTS_TIMELINE "Enable timeline components" ON)
option(ZLAY_ENABLE_BACKEND_CLAY "Enable Clay layout backend" ON)
option(ZLAY_ENABLE_RENDERER_OPENGL "Enable OpenGL renderer backend" ON)
option(ZLAY_ENABLE_RENDERER_VULKAN "Enable Vulkan renderer backend" OFF)
option(ZLAY_ENABLE_RENDERER_IMGUI "Enable Dear ImGui renderer backend" OFF)
option(ZLAY_ENABLE_RENDERER_SOKOL "Enable sokol renderer backend" OFF)
option(ZLAY_ENABLE_RENDERER_RAYLIB "Enable raylib renderer backend" OFF)
option(ZLAY_ENABLE_CANVAS "Enable Zlay Canvas rendering abstraction" ON)
option(ZLAY_ENABLE_CANVAS_PATH "Enable Canvas path drawing" ON)
option(ZLAY_ENABLE_CANVAS_TEXT "Enable Canvas text drawing" ON)
option(ZLAY_ENABLE_CANVAS_IMAGE "Enable Canvas image drawing" ON)
option(ZLAY_ENABLE_CANVAS_GRADIENT "Enable Canvas gradient drawing" OFF)
option(ZLAY_ENABLE_PLATFORM_GLFW "Enable GLFW platform backend" OFF)
option(ZLAY_ENABLE_PLATFORM_RAYLIB "Enable raylib platform backend" OFF)
option(ZLAY_ENABLE_PLATFORM_SOKOL "Enable sokol platform backend" OFF)
option(ZLAY_ENABLE_PLATFORM_WIN32 "Enable Win32 platform backend" ON)
option(ZLAY_ENABLE_PLATFORM_X11 "Enable X11 platform backend" ON)
option(ZLAY_ENABLE_PLATFORM_COCOA "Enable Cocoa platform backend" ON)
option(ZLAY_ENABLE_DRIVER_OPENGL "Enable OpenGL GPU driver" ON)
option(ZLAY_ENABLE_DRIVER_VULKAN "Enable Vulkan GPU driver" OFF)
option(ZLAY_ENABLE_DRIVER_WEBGPU "Enable WebGPU GPU driver" OFF)
option(ZLAY_ENABLE_DRIVER_METAL "Enable Metal GPU driver" OFF)
option(ZLAY_ENABLE_DRIVER_D3D12 "Enable Direct3D 12 GPU driver" OFF)
option(ZLAY_USE_CLAY_BACKEND "Deprecated alias for ZLAY_ENABLE_BACKEND_CLAY" ${ZLAY_ENABLE_BACKEND_CLAY})
option(ZLAY_VENDOR_CLAY "Use vendored Clay from third_party/clay/clay.h" ON)
option(ZLAY_CLANGD_COPY_COMPILE_COMMANDS "Copy compile_commands.json to the source root for clangd" ${PROJECT_IS_TOP_LEVEL})
set(ZLAY_HEADER_TEMPLATE_STYLE "ifndef" CACHE STRING "Default generated header style: ifndef, pragma_once, both")
set_property(CACHE ZLAY_HEADER_TEMPLATE_STYLE PROPERTY STRINGS ifndef pragma_once both)
set(ZLAY_HEADER_TEMPLATE_INCLUDE "zlay.h" CACHE STRING "Default include used by generated ZLay headers; empty disables the include")
option(ZLAY_HEADER_TEMPLATE_EXTERN_C "Wrap generated ZLay headers with extern C guards" ON)
set(ZLAY_NEW_HEADER_PATH "" CACHE FILEPATH "Header path used by the zlay_generate_header target")
set(ZLAY_COMPONENTS_CPP_STANDARD "11" CACHE STRING "C++ standard for components")
set_property(CACHE ZLAY_COMPONENTS_CPP_STANDARD PROPERTY STRINGS 11 14 17 20)
set(ZLAY_OS_BACKEND "auto" CACHE STRING "Platform backend: auto, x11, gtk, win32, cocoa")
set_property(CACHE ZLAY_OS_BACKEND PROPERTY STRINGS auto x11 gtk win32 cocoa)
option(ZLAY_ENABLE_WINRT_SUPPORT "Build Windows 10/11 modern system support helpers on top of Win32" ON)

set(ZLAY_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/zlay)
set(ZLAY_THIRD_PARTY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/third_party)
set(ZLAY_SHADER_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/shader)
set(ZLAY_PLATFORM_BACKEND_DIR ${ZLAY_SOURCE_DIR}/backends/platform)

set(ZLAY_EFFECTIVE_BACKEND_CLAY ${ZLAY_ENABLE_BACKEND_CLAY})
if (NOT ZLAY_USE_CLAY_BACKEND)
  set(ZLAY_EFFECTIVE_BACKEND_CLAY OFF)
endif()

set(ZLAY_EFFECTIVE_RENDERER_OPENGL OFF)
set(ZLAY_EFFECTIVE_RENDERER_VULKAN OFF)
set(ZLAY_EFFECTIVE_CANVAS OFF)
if (ZLAY_BUILD_RENDERER AND ZLAY_BUILD_DRIVERS AND ZLAY_ENABLE_RENDERER_OPENGL)
  set(ZLAY_EFFECTIVE_RENDERER_OPENGL ON)
endif()
if (ZLAY_BUILD_RENDERER AND ZLAY_BUILD_DRIVERS AND ZLAY_ENABLE_RENDERER_VULKAN)
  set(ZLAY_EFFECTIVE_RENDERER_VULKAN ON)
endif()
if (ZLAY_BUILD_RENDERER AND ZLAY_ENABLE_CANVAS)
  set(ZLAY_EFFECTIVE_CANVAS ON)
endif()

set(ZLAY_EFFECTIVE_DRIVER_OPENGL OFF)
set(ZLAY_EFFECTIVE_DRIVER_VULKAN OFF)
if (ZLAY_BUILD_DRIVERS AND ZLAY_ENABLE_DRIVER_OPENGL)
  set(ZLAY_EFFECTIVE_DRIVER_OPENGL ON)
endif()
if (ZLAY_BUILD_DRIVERS AND ZLAY_ENABLE_DRIVER_VULKAN)
  set(ZLAY_EFFECTIVE_DRIVER_VULKAN ON)
endif()

if (ZLAY_EFFECTIVE_RENDERER_OPENGL AND NOT ZLAY_EFFECTIVE_DRIVER_OPENGL)
  message(FATAL_ERROR "ZLAY_ENABLE_RENDERER_OPENGL requires ZLAY_ENABLE_DRIVER_OPENGL")
endif()

if (ZLAY_EFFECTIVE_RENDERER_VULKAN AND NOT ZLAY_EFFECTIVE_DRIVER_VULKAN)
  message(FATAL_ERROR "ZLAY_ENABLE_RENDERER_VULKAN requires ZLAY_ENABLE_DRIVER_VULKAN")
endif()

if ((ZLAY_EFFECTIVE_RENDERER_OPENGL OR ZLAY_EFFECTIVE_RENDERER_VULKAN) AND NOT ZLAY_BUILD_SHADERS)
  message(FATAL_ERROR "OpenGL/Vulkan renderer backends require ZLAY_BUILD_SHADERS")
endif()

if (ZLAY_ENABLE_RENDERER_IMGUI)
  message(FATAL_ERROR "ZLAY_ENABLE_RENDERER_IMGUI is reserved until the Dear ImGui renderer backend is implemented")
endif()

if (ZLAY_ENABLE_RENDERER_SOKOL)
  message(FATAL_ERROR "ZLAY_ENABLE_RENDERER_SOKOL is reserved until the sokol renderer backend is implemented")
endif()

if (ZLAY_ENABLE_RENDERER_RAYLIB)
  message(FATAL_ERROR "ZLAY_ENABLE_RENDERER_RAYLIB is reserved until the raylib renderer backend is implemented")
endif()

if (ZLAY_ENABLE_PLATFORM_GLFW)
  message(FATAL_ERROR "ZLAY_ENABLE_PLATFORM_GLFW is reserved until the GLFW platform backend is implemented")
endif()

if (ZLAY_ENABLE_PLATFORM_RAYLIB)
  message(FATAL_ERROR "ZLAY_ENABLE_PLATFORM_RAYLIB is reserved until the raylib platform backend is implemented")
endif()

if (ZLAY_ENABLE_PLATFORM_SOKOL)
  message(FATAL_ERROR "ZLAY_ENABLE_PLATFORM_SOKOL is reserved until the sokol platform backend is implemented")
endif()

if (ZLAY_ENABLE_DRIVER_WEBGPU)
  message(FATAL_ERROR "ZLAY_ENABLE_DRIVER_WEBGPU is reserved until the WebGPU GPU driver is implemented")
endif()

if (ZLAY_ENABLE_DRIVER_METAL)
  message(FATAL_ERROR "ZLAY_ENABLE_DRIVER_METAL is reserved until the Metal GPU driver is implemented")
endif()

if (ZLAY_ENABLE_DRIVER_D3D12)
  message(FATAL_ERROR "ZLAY_ENABLE_DRIVER_D3D12 is reserved until the Direct3D 12 GPU driver is implemented")
endif()

set(ZLAY_PUBLIC_INCLUDE_DIRS
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${ZLAY_SOURCE_DIR}
)

include_directories(${ZLAY_PUBLIC_INCLUDE_DIRS})

file(GLOB ZLAY_CORE_SOURCES CONFIGURE_DEPENDS
  ${ZLAY_SOURCE_DIR}/*.c
)
list(REMOVE_ITEM ZLAY_CORE_SOURCES
  ${ZLAY_SOURCE_DIR}/zlay_antialiasing.c
)
list(SORT ZLAY_CORE_SOURCES)

file(GLOB ZLAY_CORE_HEADERS CONFIGURE_DEPENDS
  ${CMAKE_CURRENT_SOURCE_DIR}/zlay.h
  ${ZLAY_SOURCE_DIR}/*.h
)
list(REMOVE_ITEM ZLAY_CORE_HEADERS
  ${ZLAY_SOURCE_DIR}/zlay_antialiasing.h
)
list(SORT ZLAY_CORE_HEADERS)

file(GLOB ZLAY_CORE_MODULE_SOURCES CONFIGURE_DEPENDS
  ${ZLAY_SOURCE_DIR}/core/*.c
)
list(SORT ZLAY_CORE_MODULE_SOURCES)

file(GLOB ZLAY_CORE_MODULE_HEADERS CONFIGURE_DEPENDS
  ${ZLAY_SOURCE_DIR}/core/*.h
)
list(SORT ZLAY_CORE_MODULE_HEADERS)

file(GLOB ZLAY_UI_HEADERS CONFIGURE_DEPENDS
  ${ZLAY_SOURCE_DIR}/ui/*.h
)
list(SORT ZLAY_UI_HEADERS)

file(GLOB ZLAY_UI_SOURCES CONFIGURE_DEPENDS
  ${ZLAY_SOURCE_DIR}/ui/*.c
)
list(SORT ZLAY_UI_SOURCES)

file(GLOB_RECURSE ZLAY_COMPONENT_HEADERS CONFIGURE_DEPENDS
  ${CMAKE_CURRENT_SOURCE_DIR}/include/zlay/components*.hpp
  ${ZLAY_SOURCE_DIR}/components/*.hpp
  ${ZLAY_SOURCE_DIR}/components/**/*.hpp
)
list(SORT ZLAY_COMPONENT_HEADERS)

file(GLOB ZLAY_BACKEND_INTERFACE_HEADERS CONFIGURE_DEPENDS
  ${ZLAY_SOURCE_DIR}/backends/*.h
)
list(SORT ZLAY_BACKEND_INTERFACE_HEADERS)

file(GLOB_RECURSE ZLAY_BACKEND_CLAY_SOURCES CONFIGURE_DEPENDS
  ${ZLAY_SOURCE_DIR}/backends/layout/clay/*.c
)
list(SORT ZLAY_BACKEND_CLAY_SOURCES)

file(GLOB_RECURSE ZLAY_BACKEND_CLAY_HEADERS CONFIGURE_DEPENDS
  ${ZLAY_SOURCE_DIR}/backends/layout/clay/*.h
)
list(SORT ZLAY_BACKEND_CLAY_HEADERS)

file(GLOB ZLAY_RENDERER_SOURCES CONFIGURE_DEPENDS
  ${ZLAY_SOURCE_DIR}/renderer/*.c
)
list(SORT ZLAY_RENDERER_SOURCES)

file(GLOB ZLAY_RENDERER_HEADERS CONFIGURE_DEPENDS
  ${ZLAY_SOURCE_DIR}/renderer/*.h
)
list(SORT ZLAY_RENDERER_HEADERS)

file(GLOB_RECURSE ZLAY_CANVAS_SOURCES CONFIGURE_DEPENDS
  ${ZLAY_SOURCE_DIR}/renderer/canvas/*.c
)
list(SORT ZLAY_CANVAS_SOURCES)

file(GLOB_RECURSE ZLAY_CANVAS_HEADERS CONFIGURE_DEPENDS
  ${ZLAY_SOURCE_DIR}/renderer/canvas/*.h
)
list(SORT ZLAY_CANVAS_HEADERS)

file(GLOB ZLAY_RENDERER_OPENGL_BACKEND_SOURCES CONFIGURE_DEPENDS
  ${ZLAY_SOURCE_DIR}/backends/renderer/opengl/*.c
)
list(SORT ZLAY_RENDERER_OPENGL_BACKEND_SOURCES)

file(GLOB ZLAY_RENDERER_OPENGL_BACKEND_HEADERS CONFIGURE_DEPENDS
  ${ZLAY_SOURCE_DIR}/backends/renderer/opengl/*.h
)
list(SORT ZLAY_RENDERER_OPENGL_BACKEND_HEADERS)

file(GLOB ZLAY_RENDERER_VULKAN_BACKEND_SOURCES CONFIGURE_DEPENDS
  ${ZLAY_SOURCE_DIR}/backends/renderer/vulkan/*.c
)
list(SORT ZLAY_RENDERER_VULKAN_BACKEND_SOURCES)

file(GLOB ZLAY_RENDERER_VULKAN_BACKEND_HEADERS CONFIGURE_DEPENDS
  ${ZLAY_SOURCE_DIR}/backends/renderer/vulkan/*.h
)
list(SORT ZLAY_RENDERER_VULKAN_BACKEND_HEADERS)

file(GLOB ZLAY_LIFECYCLE_SOURCES CONFIGURE_DEPENDS
  ${ZLAY_SOURCE_DIR}/lifecycle/*.c
)
list(SORT ZLAY_LIFECYCLE_SOURCES)

file(GLOB ZLAY_LIFECYCLE_HEADERS CONFIGURE_DEPENDS
  ${ZLAY_SOURCE_DIR}/lifecycle/*.h
)
list(SORT ZLAY_LIFECYCLE_HEADERS)

file(GLOB_RECURSE ZLAY_PARSER_SOURCES CONFIGURE_DEPENDS
  ${ZLAY_SOURCE_DIR}/parser/ast/*.c
  ${ZLAY_SOURCE_DIR}/parser/lexer/*.c
  ${ZLAY_SOURCE_DIR}/parser/resolver/*.c
  ${ZLAY_SOURCE_DIR}/parser/semantic/*.c
  ${ZLAY_SOURCE_DIR}/parser/syntax/*.c
)
list(SORT ZLAY_PARSER_SOURCES)

file(GLOB_RECURSE ZLAY_PARSER_HEADERS CONFIGURE_DEPENDS
  ${ZLAY_SOURCE_DIR}/parser/ast/*.h
  ${ZLAY_SOURCE_DIR}/parser/lexer/*.h
  ${ZLAY_SOURCE_DIR}/parser/resolver/*.h
  ${ZLAY_SOURCE_DIR}/parser/semantic/*.h
  ${ZLAY_SOURCE_DIR}/parser/syntax/*.h
)
list(SORT ZLAY_PARSER_HEADERS)

file(GLOB ZLAY_DRIVER_CORE_SOURCES CONFIGURE_DEPENDS
  ${ZLAY_SOURCE_DIR}/drivers/gpu/*.c
)
list(SORT ZLAY_DRIVER_CORE_SOURCES)

file(GLOB ZLAY_DRIVER_CORE_HEADERS CONFIGURE_DEPENDS
  ${ZLAY_SOURCE_DIR}/drivers/gpu/*.h
)
list(SORT ZLAY_DRIVER_CORE_HEADERS)

file(GLOB ZLAY_DRIVER_OPENGL_SOURCES CONFIGURE_DEPENDS
  ${ZLAY_SOURCE_DIR}/drivers/gpu/opengl/*.c
)
list(SORT ZLAY_DRIVER_OPENGL_SOURCES)

file(GLOB ZLAY_DRIVER_OPENGL_HEADERS CONFIGURE_DEPENDS
  ${ZLAY_SOURCE_DIR}/drivers/gpu/opengl/*.h
)
list(SORT ZLAY_DRIVER_OPENGL_HEADERS)

file(GLOB ZLAY_DRIVER_VULKAN_SOURCES CONFIGURE_DEPENDS
  ${ZLAY_SOURCE_DIR}/drivers/gpu/vulkan/*.c
)
list(SORT ZLAY_DRIVER_VULKAN_SOURCES)

file(GLOB ZLAY_DRIVER_VULKAN_HEADERS CONFIGURE_DEPENDS
  ${ZLAY_SOURCE_DIR}/drivers/gpu/vulkan/*.h
)
list(SORT ZLAY_DRIVER_VULKAN_HEADERS)

file(GLOB ZLAY_SHADER_LIBRARY_SOURCES CONFIGURE_DEPENDS
  ${ZLAY_SOURCE_DIR}/zlay_antialiasing.c
)
list(SORT ZLAY_SHADER_LIBRARY_SOURCES)

file(GLOB ZLAY_SHADER_LIBRARY_HEADERS CONFIGURE_DEPENDS
  ${ZLAY_SOURCE_DIR}/zlay_antialiasing.h
)
list(SORT ZLAY_SHADER_LIBRARY_HEADERS)

file(GLOB_RECURSE ZLAY_OS_HEADERS CONFIGURE_DEPENDS
  ${ZLAY_SOURCE_DIR}/os/*.h
)
file(GLOB ZLAY_OS_ROOT_SOURCES CONFIGURE_DEPENDS
  ${ZLAY_SOURCE_DIR}/os/*.c
)
file(GLOB_RECURSE ZLAY_OS_CORE_SOURCES CONFIGURE_DEPENDS
  ${ZLAY_SOURCE_DIR}/os/event/*.c
  ${ZLAY_SOURCE_DIR}/os/system/*.c
  ${ZLAY_SOURCE_DIR}/os/window/*.c
)
list(APPEND ZLAY_OS_CORE_SOURCES ${ZLAY_OS_ROOT_SOURCES})
list(SORT ZLAY_OS_HEADERS)
list(SORT ZLAY_OS_CORE_SOURCES)

file(GLOB ZLAY_OS_WIN32_SOURCES CONFIGURE_DEPENDS ${ZLAY_PLATFORM_BACKEND_DIR}/win32/*.c)
file(GLOB_RECURSE ZLAY_OS_WINRT_SOURCES CONFIGURE_DEPENDS
  ${ZLAY_PLATFORM_BACKEND_DIR}/winrt/*.c
  ${ZLAY_PLATFORM_BACKEND_DIR}/winrt/*.cpp
)
file(GLOB_RECURSE ZLAY_OS_COCOA_SOURCES CONFIGURE_DEPENDS
  ${ZLAY_PLATFORM_BACKEND_DIR}/cocoa/*.c
  ${ZLAY_PLATFORM_BACKEND_DIR}/cocoa/*.m
  ${ZLAY_PLATFORM_BACKEND_DIR}/cocoa/*.mm
)
file(GLOB ZLAY_OS_GTK_SOURCES CONFIGURE_DEPENDS ${ZLAY_PLATFORM_BACKEND_DIR}/gtk/*.c)
file(GLOB ZLAY_OS_X11_SOURCES CONFIGURE_DEPENDS
  ${ZLAY_PLATFORM_BACKEND_DIR}/x11/*.c
)

file(GLOB ZLAY_OS_WIN32_HEADERS CONFIGURE_DEPENDS ${ZLAY_PLATFORM_BACKEND_DIR}/win32/*.h)
file(GLOB_RECURSE ZLAY_OS_WINRT_HEADERS CONFIGURE_DEPENDS
  ${ZLAY_PLATFORM_BACKEND_DIR}/winrt/*.h
  ${ZLAY_PLATFORM_BACKEND_DIR}/winrt/*.hpp
)
file(GLOB_RECURSE ZLAY_OS_COCOA_HEADERS CONFIGURE_DEPENDS
  ${ZLAY_PLATFORM_BACKEND_DIR}/cocoa/*.h
  ${ZLAY_PLATFORM_BACKEND_DIR}/cocoa/*.hpp
)
file(GLOB ZLAY_OS_GTK_HEADERS CONFIGURE_DEPENDS ${ZLAY_PLATFORM_BACKEND_DIR}/gtk/*.h)
file(GLOB ZLAY_OS_X11_HEADERS CONFIGURE_DEPENDS
  ${ZLAY_PLATFORM_BACKEND_DIR}/x11/*.h
)

list(SORT ZLAY_OS_WIN32_SOURCES)
list(SORT ZLAY_OS_WINRT_SOURCES)
list(SORT ZLAY_OS_COCOA_SOURCES)
list(SORT ZLAY_OS_GTK_SOURCES)
list(SORT ZLAY_OS_X11_SOURCES)
list(SORT ZLAY_OS_WIN32_HEADERS)
list(SORT ZLAY_OS_WINRT_HEADERS)
list(SORT ZLAY_OS_COCOA_HEADERS)
list(SORT ZLAY_OS_GTK_HEADERS)
list(SORT ZLAY_OS_X11_HEADERS)

if (ZLAY_BUILD_DRIVERS AND NOT ZLAY_BUILD_RENDERER)
  message(FATAL_ERROR "ZLAY_BUILD_DRIVERS requires ZLAY_BUILD_RENDERER because driver adapters implement the renderer interface")
endif()

if (ZLAY_BUILD_DRIVERS AND NOT ZLAY_BUILD_SHADERS)
  message(FATAL_ERROR "ZLAY_BUILD_DRIVERS requires ZLAY_BUILD_SHADERS because driver adapters use built-in shader sources")
endif()

add_library(zlay)
add_library(zlay::zlay ALIAS zlay)

add_library(zlay_core INTERFACE)
add_library(zlay::core ALIAS zlay_core)
set_target_properties(zlay_core PROPERTIES EXPORT_NAME core)
target_link_libraries(zlay_core INTERFACE zlay)

if (ZLAY_ENABLE_COMPONENTS)
  add_library(zlay_components INTERFACE)
  add_library(zlay::components ALIAS zlay_components)
  set_target_properties(zlay_components PROPERTIES EXPORT_NAME components)
  target_link_libraries(zlay_components INTERFACE zlay)
  target_compile_features(zlay_components INTERFACE cxx_std_${ZLAY_COMPONENTS_CPP_STANDARD})
  target_compile_definitions(zlay_components
    INTERFACE
      ZLAY_ENABLE_COMPONENTS=1
      $<$<BOOL:${ZLAY_ENABLE_COMPONENTS_META}>:ZLAY_ENABLE_COMPONENTS_META=1>
      $<$<BOOL:${ZLAY_ENABLE_COMPONENTS_TOOLING}>:ZLAY_ENABLE_COMPONENTS_TOOLING=1>
      $<$<BOOL:${ZLAY_ENABLE_COMPONENTS_VIEWPORT}>:ZLAY_ENABLE_COMPONENTS_VIEWPORT=1>
      $<$<BOOL:${ZLAY_ENABLE_COMPONENTS_SCENE}>:ZLAY_ENABLE_COMPONENTS_SCENE=1>
      $<$<BOOL:${ZLAY_ENABLE_COMPONENTS_INSPECTOR}>:ZLAY_ENABLE_COMPONENTS_INSPECTOR=1>
      $<$<BOOL:${ZLAY_ENABLE_COMPONENTS_ASSETS}>:ZLAY_ENABLE_COMPONENTS_ASSETS=1>
      $<$<BOOL:${ZLAY_ENABLE_COMPONENTS_GRAPH}>:ZLAY_ENABLE_COMPONENTS_GRAPH=1>
      $<$<BOOL:${ZLAY_ENABLE_COMPONENTS_TIMELINE}>:ZLAY_ENABLE_COMPONENTS_TIMELINE=1>
  )
endif()

target_compile_features(zlay PUBLIC c_std_11)
set_target_properties(zlay PROPERTIES
  C_STANDARD 11
  C_STANDARD_REQUIRED YES
  EXPORT_NAME zlay
  POSITION_INDEPENDENT_CODE ON
  SOVERSION ${PROJECT_VERSION_MAJOR}
  VERSION ${PROJECT_VERSION}
)

target_compile_definitions(zlay
  PRIVATE
    ZLAY_BUILDING
  PUBLIC
    $<$<BOOL:${BUILD_SHARED_LIBS}>:ZLAY_SHARED>
    $<$<BOOL:${ZLAY_EFFECTIVE_BACKEND_CLAY}>:ZLAY_ENABLE_BACKEND_CLAY=1>
    $<$<BOOL:${ZLAY_EFFECTIVE_CANVAS}>:ZLAY_ENABLE_CANVAS=1>
    $<$<AND:$<BOOL:${ZLAY_EFFECTIVE_CANVAS}>,$<BOOL:${ZLAY_ENABLE_CANVAS_PATH}>>:ZLAY_ENABLE_CANVAS_PATH=1>
    $<$<AND:$<BOOL:${ZLAY_EFFECTIVE_CANVAS}>,$<BOOL:${ZLAY_ENABLE_CANVAS_TEXT}>>:ZLAY_ENABLE_CANVAS_TEXT=1>
    $<$<AND:$<BOOL:${ZLAY_EFFECTIVE_CANVAS}>,$<BOOL:${ZLAY_ENABLE_CANVAS_IMAGE}>>:ZLAY_ENABLE_CANVAS_IMAGE=1>
    $<$<AND:$<BOOL:${ZLAY_EFFECTIVE_CANVAS}>,$<BOOL:${ZLAY_ENABLE_CANVAS_GRADIENT}>>:ZLAY_ENABLE_CANVAS_GRADIENT=1>
    $<$<BOOL:${ZLAY_EFFECTIVE_RENDERER_OPENGL}>:ZLAY_ENABLE_RENDERER_OPENGL=1>
    $<$<BOOL:${ZLAY_EFFECTIVE_RENDERER_VULKAN}>:ZLAY_ENABLE_RENDERER_VULKAN=1>
    $<$<BOOL:${ZLAY_EFFECTIVE_DRIVER_OPENGL}>:ZLAY_ENABLE_DRIVER_OPENGL=1>
    $<$<BOOL:${ZLAY_EFFECTIVE_DRIVER_VULKAN}>:ZLAY_ENABLE_DRIVER_VULKAN=1>
)

add_custom_target(zlay_generate_header
  COMMAND ${CMAKE_COMMAND}
    -DZLAY_HEADER_PATH=${ZLAY_NEW_HEADER_PATH}
    -DZLAY_HEADER_GUARD_STYLE=${ZLAY_HEADER_TEMPLATE_STYLE}
    -DZLAY_HEADER_INCLUDE=${ZLAY_HEADER_TEMPLATE_INCLUDE}
    -DZLAY_HEADER_EXTERN_C=${ZLAY_HEADER_TEMPLATE_EXTERN_C}
    -P ${CMAKE_CURRENT_SOURCE_DIR}/cmake/new_header.cmake
  COMMENT "Generating a ZLay header from template"
  VERBATIM
)

if (ZLAY_CLANGD_COPY_COMPILE_COMMANDS)
  add_custom_target(zlay_clangd_database ALL
    COMMAND ${CMAKE_COMMAND}
      -DZLAY_COMPILE_COMMANDS_IN=${CMAKE_BINARY_DIR}/compile_commands.json
      -DZLAY_COMPILE_COMMANDS_OUT=${CMAKE_CURRENT_SOURCE_DIR}/compile_commands.json
      -P ${CMAKE_CURRENT_SOURCE_DIR}/cmake/copy_compile_commands.cmake
    COMMENT "Refreshing root compile_commands.json for clangd"
    VERBATIM
  )
endif()

target_sources(zlay
  PRIVATE
    ${ZLAY_CORE_SOURCES}
    ${ZLAY_CORE_HEADERS}
    ${ZLAY_CORE_MODULE_SOURCES}
    ${ZLAY_CORE_MODULE_HEADERS}
    ${ZLAY_UI_SOURCES}
    ${ZLAY_UI_HEADERS}
    ${ZLAY_BACKEND_INTERFACE_HEADERS}
)

if (ZLAY_ENABLE_COMPONENTS)
  target_sources(zlay PRIVATE ${ZLAY_COMPONENT_HEADERS})
endif()

if (ZLAY_EFFECTIVE_BACKEND_CLAY)
  message(STATUS "Zlay: Clay backend enabled")
  if (ZLAY_VENDOR_CLAY)
    message(STATUS "Zlay: Using vendored Clay")
  endif()
  target_sources(zlay PRIVATE ${ZLAY_BACKEND_CLAY_SOURCES} ${ZLAY_BACKEND_CLAY_HEADERS})

  add_library(zlay_backend_clay INTERFACE)
  add_library(zlay::backend_clay ALIAS zlay_backend_clay)
  set_target_properties(zlay_backend_clay PROPERTIES EXPORT_NAME backend_clay)
  target_link_libraries(zlay_backend_clay INTERFACE zlay)

  add_library(z_clay INTERFACE)
  add_library(zlay::clay ALIAS z_clay)
  set_target_properties(z_clay PROPERTIES EXPORT_NAME clay)
  target_link_libraries(z_clay INTERFACE zlay_backend_clay)
endif()

target_include_directories(zlay
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${ZLAY_SOURCE_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/zlay>
)

if (MSVC)
  target_compile_options(zlay PRIVATE /W4)
else()
  target_compile_options(zlay PRIVATE -Wall -Wextra -Wpedantic)
endif()

if (UNIX AND NOT APPLE)
  target_link_libraries(zlay PRIVATE m)
endif()

if (ZLAY_BUILD_LIFECYCLE)
  target_sources(zlay PRIVATE ${ZLAY_LIFECYCLE_SOURCES} ${ZLAY_LIFECYCLE_HEADERS})

  add_library(zlay_lifecycle INTERFACE)
  add_library(zlay::lifecycle ALIAS zlay_lifecycle)
  set_target_properties(zlay_lifecycle PROPERTIES EXPORT_NAME lifecycle)
  target_link_libraries(zlay_lifecycle INTERFACE zlay)
endif()

if (ZLAY_BUILD_RENDERER)
  target_sources(zlay PRIVATE ${ZLAY_RENDERER_SOURCES} ${ZLAY_RENDERER_HEADERS})

  add_library(zlay_renderer INTERFACE)
  add_library(zlay::renderer ALIAS zlay_renderer)
  set_target_properties(zlay_renderer PROPERTIES EXPORT_NAME renderer)
  target_link_libraries(zlay_renderer INTERFACE zlay)

  if (ZLAY_EFFECTIVE_CANVAS)
    target_sources(zlay PRIVATE ${ZLAY_CANVAS_SOURCES} ${ZLAY_CANVAS_HEADERS})

    add_library(zlay_renderer_canvas INTERFACE)
    add_library(zlay::renderer_canvas ALIAS zlay_renderer_canvas)
    set_target_properties(zlay_renderer_canvas PROPERTIES EXPORT_NAME renderer_canvas)
    target_link_libraries(zlay_renderer_canvas INTERFACE zlay_renderer)
  endif()

  if (ZLAY_EFFECTIVE_RENDERER_OPENGL)
    target_sources(zlay PRIVATE ${ZLAY_RENDERER_OPENGL_BACKEND_SOURCES} ${ZLAY_RENDERER_OPENGL_BACKEND_HEADERS})

    add_library(zlay_renderer_backend_opengl INTERFACE)
    add_library(zlay::renderer_backend_opengl ALIAS zlay_renderer_backend_opengl)
    set_target_properties(zlay_renderer_backend_opengl PROPERTIES EXPORT_NAME renderer_backend_opengl)
    target_link_libraries(zlay_renderer_backend_opengl INTERFACE zlay_renderer)
  endif()

  if (ZLAY_EFFECTIVE_RENDERER_VULKAN)
    target_sources(zlay PRIVATE ${ZLAY_RENDERER_VULKAN_BACKEND_SOURCES} ${ZLAY_RENDERER_VULKAN_BACKEND_HEADERS})

    add_library(zlay_renderer_backend_vulkan INTERFACE)
    add_library(zlay::renderer_backend_vulkan ALIAS zlay_renderer_backend_vulkan)
    set_target_properties(zlay_renderer_backend_vulkan PROPERTIES EXPORT_NAME renderer_backend_vulkan)
    target_link_libraries(zlay_renderer_backend_vulkan INTERFACE zlay_renderer)
  endif()
endif()

if (ZLAY_BUILD_PARSER)
  target_sources(zlay PRIVATE ${ZLAY_PARSER_SOURCES} ${ZLAY_PARSER_HEADERS})

  add_library(zlay_parser INTERFACE)
  add_library(zlay::parser ALIAS zlay_parser)
  set_target_properties(zlay_parser PROPERTIES EXPORT_NAME parser)
  target_link_libraries(zlay_parser INTERFACE zlay)
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
  set_target_properties(zlay_shaders PROPERTIES EXPORT_NAME shaders)
  target_link_libraries(zlay_shaders INTERFACE zlay)
endif()

if (ZLAY_BUILD_DRIVERS)
  target_sources(zlay PRIVATE ${ZLAY_DRIVER_CORE_SOURCES} ${ZLAY_DRIVER_CORE_HEADERS})

  add_library(zlay_driver INTERFACE)
  add_library(zlay::driver ALIAS zlay_driver)
  set_target_properties(zlay_driver PROPERTIES EXPORT_NAME driver)
  target_link_libraries(zlay_driver INTERFACE zlay)

  if (ZLAY_EFFECTIVE_DRIVER_OPENGL)
    target_sources(zlay PRIVATE ${ZLAY_DRIVER_OPENGL_SOURCES} ${ZLAY_DRIVER_OPENGL_HEADERS})

    add_library(zlay_driver_opengl INTERFACE)
    add_library(zlay::driver_opengl ALIAS zlay_driver_opengl)
    set_target_properties(zlay_driver_opengl PROPERTIES EXPORT_NAME driver_opengl)
    target_link_libraries(zlay_driver_opengl INTERFACE zlay_driver)
  endif()

  if (ZLAY_EFFECTIVE_DRIVER_VULKAN)
    target_sources(zlay PRIVATE ${ZLAY_DRIVER_VULKAN_SOURCES} ${ZLAY_DRIVER_VULKAN_HEADERS})

    add_library(zlay_driver_vulkan INTERFACE)
    add_library(zlay::driver_vulkan ALIAS zlay_driver_vulkan)
    set_target_properties(zlay_driver_vulkan PROPERTIES EXPORT_NAME driver_vulkan)
    target_link_libraries(zlay_driver_vulkan INTERFACE zlay_driver)
  endif()
endif()

if (ZLAY_BUILD_OS)
  find_package(Threads REQUIRED)
  target_link_libraries(zlay PRIVATE Threads::Threads)

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
    if (NOT ZLAY_ENABLE_PLATFORM_WIN32)
      message(FATAL_ERROR "ZLAY_OS_BACKEND=win32 requires ZLAY_ENABLE_PLATFORM_WIN32")
    endif()
    target_compile_definitions(zlay PRIVATE ZLAY_OS_BACKEND_SELECTED_WIN32=1)
    target_sources(zlay PRIVATE ${ZLAY_OS_HEADERS} ${ZLAY_OS_CORE_SOURCES} ${ZLAY_OS_WIN32_SOURCES} ${ZLAY_OS_WIN32_HEADERS})
    if (WIN32 AND ZLAY_ENABLE_WINRT_SUPPORT)
      target_compile_definitions(zlay PRIVATE ZLAY_ENABLE_WINRT_SUPPORT=1)
      target_compile_definitions(zlay PUBLIC ZLAY_ENABLE_WINRT=1 ZLAY_ENABLE_WINRT_MODERN_UI=1)
      target_sources(zlay PRIVATE ${ZLAY_OS_WINRT_SOURCES} ${ZLAY_OS_WINRT_HEADERS})
      target_compile_features(zlay PRIVATE cxx_std_17)
      target_link_libraries(zlay PRIVATE ole32)
    endif()
  elseif(ZLAY_EFFECTIVE_OS_BACKEND STREQUAL "cocoa")
    if(NOT APPLE)
      message(FATAL_ERROR "ZLAY_OS_BACKEND=cocoa requires an Apple target")
    endif()
    if (NOT ZLAY_ENABLE_PLATFORM_COCOA)
      message(FATAL_ERROR "ZLAY_OS_BACKEND=cocoa requires ZLAY_ENABLE_PLATFORM_COCOA")
    endif()
    enable_language(OBJCXX)
    target_compile_definitions(zlay PRIVATE ZLAY_OS_BACKEND_SELECTED_COCOA=1)
    target_sources(zlay PRIVATE ${ZLAY_OS_HEADERS} ${ZLAY_OS_CORE_SOURCES} ${ZLAY_OS_COCOA_SOURCES} ${ZLAY_OS_COCOA_HEADERS})
    find_library(FOUNDATION_FRAMEWORK Foundation REQUIRED)
    find_library(QUARTZCORE_FRAMEWORK QuartzCore REQUIRED)
    if(CMAKE_SYSTEM_NAME STREQUAL "iOS")
      find_library(UIKIT_FRAMEWORK UIKit REQUIRED)
      target_link_libraries(zlay PRIVATE ${FOUNDATION_FRAMEWORK} ${QUARTZCORE_FRAMEWORK} ${UIKIT_FRAMEWORK})
    else()
      find_library(COCOA_FRAMEWORK Cocoa REQUIRED)
      target_link_libraries(zlay PRIVATE ${FOUNDATION_FRAMEWORK} ${QUARTZCORE_FRAMEWORK} ${COCOA_FRAMEWORK})
    endif()
  elseif(ZLAY_EFFECTIVE_OS_BACKEND STREQUAL "gtk")
    target_compile_definitions(zlay PRIVATE ZLAY_OS_BACKEND_SELECTED_GTK=1)
    target_sources(zlay PRIVATE ${ZLAY_OS_HEADERS} ${ZLAY_OS_CORE_SOURCES} ${ZLAY_OS_GTK_SOURCES} ${ZLAY_OS_GTK_HEADERS})
    if (UNIX AND NOT APPLE)
      target_link_libraries(zlay PRIVATE rt)
    endif()
  elseif(ZLAY_EFFECTIVE_OS_BACKEND STREQUAL "x11")
    if (NOT ZLAY_ENABLE_PLATFORM_X11)
      message(FATAL_ERROR "ZLAY_OS_BACKEND=x11 requires ZLAY_ENABLE_PLATFORM_X11")
    endif()
    find_package(X11 REQUIRED)
    target_compile_definitions(zlay PRIVATE ZLAY_OS_BACKEND_SELECTED_X11=1)
    target_sources(zlay PRIVATE ${ZLAY_OS_HEADERS} ${ZLAY_OS_CORE_SOURCES} ${ZLAY_OS_X11_SOURCES} ${ZLAY_OS_X11_HEADERS})
    target_include_directories(zlay PRIVATE ${X11_INCLUDE_DIR})
    target_link_libraries(zlay PRIVATE rt ${X11_LIBRARIES})
  else()
    message(FATAL_ERROR "Unknown ZLAY_OS_BACKEND='${ZLAY_OS_BACKEND}'")
  endif()

  add_library(zlay_os INTERFACE)
  add_library(zlay::os ALIAS zlay_os)
  set_target_properties(zlay_os PROPERTIES EXPORT_NAME os)
  target_link_libraries(zlay_os INTERFACE zlay)
endif()

if (ZLAY_BUILD_EXAMPLES)
  add_executable(zlay_example_basic example/basic_cpp/main.cpp)
  target_compile_features(zlay_example_basic PRIVATE cxx_std_20)
  target_include_directories(zlay_example_basic PRIVATE ${ZLAY_PUBLIC_INCLUDE_DIRS})
  target_link_libraries(zlay_example_basic PRIVATE zlay)

  add_executable(zlay_example_basic_c example/basic_c/main.c)
  target_compile_features(zlay_example_basic_c PRIVATE c_std_11)
  target_include_directories(zlay_example_basic_c PRIVATE ${ZLAY_PUBLIC_INCLUDE_DIRS})
  target_link_libraries(zlay_example_basic_c PRIVATE zlay)

  add_executable(zlay_example_wrapper example/zlay_wrapper/main.c)
  target_compile_features(zlay_example_wrapper PRIVATE c_std_11)
  target_include_directories(zlay_example_wrapper PRIVATE ${ZLAY_PUBLIC_INCLUDE_DIRS})
  target_link_libraries(zlay_example_wrapper PRIVATE zlay)

  add_executable(zlay_example_wrapper_cpp example/zlay_wrapper/main.cpp)
  target_compile_features(zlay_example_wrapper_cpp PRIVATE cxx_std_20)
  target_include_directories(zlay_example_wrapper_cpp PRIVATE ${ZLAY_PUBLIC_INCLUDE_DIRS})
  target_link_libraries(zlay_example_wrapper_cpp PRIVATE zlay)

  if (ZLAY_ENABLE_COMPONENTS)
    add_executable(zlay_example_components example/components/main.cpp)
    target_compile_features(zlay_example_components PRIVATE cxx_std_${ZLAY_COMPONENTS_CPP_STANDARD})
    target_include_directories(zlay_example_components PRIVATE ${ZLAY_PUBLIC_INCLUDE_DIRS})
    target_link_libraries(zlay_example_components PRIVATE zlay_components)
  endif()

  find_package(X11 QUIET)
  if (X11_FOUND)
    if (TARGET zlay_example_wrapper_cpp)
      target_include_directories(zlay_example_wrapper_cpp PRIVATE ${X11_INCLUDE_DIR})
      target_link_libraries(zlay_example_wrapper_cpp PRIVATE ${X11_LIBRARIES})
    endif()

    add_executable(zlay_example_gui_x11
      example/simply_gui/gui_x11.c
      include/simply_gui/gui_x11.h
    )
    target_compile_features(zlay_example_gui_x11 PRIVATE c_std_11)
    target_include_directories(zlay_example_gui_x11 PRIVATE ${ZLAY_PUBLIC_INCLUDE_DIRS} ${X11_INCLUDE_DIR})
    target_link_libraries(zlay_example_gui_x11 PRIVATE zlay ${X11_LIBRARIES})
  endif()

  if (WIN32)
    add_executable(zlay_example_gui_win32
      example/simply_gui/gui_win32.c
      include/simply_gui/gui_win32.h
    )
    target_compile_features(zlay_example_gui_win32 PRIVATE c_std_11)
    target_include_directories(zlay_example_gui_win32 PRIVATE ${ZLAY_PUBLIC_INCLUDE_DIRS})
    target_link_libraries(zlay_example_gui_win32 PRIVATE zlay user32 gdi32)
  endif()

  if (APPLE)
    enable_language(OBJCXX)
    find_library(COCOA_FRAMEWORK Cocoa REQUIRED)
    add_executable(zlay_example_gui_cocoa
      example/simply_gui/gui_cocoa.mm
      include/simply_gui/gui_cocoa.h
    )
    target_include_directories(zlay_example_gui_cocoa PRIVATE ${ZLAY_PUBLIC_INCLUDE_DIRS})
    target_link_libraries(zlay_example_gui_cocoa PRIVATE zlay ${COCOA_FRAMEWORK})
  endif()

  if (TARGET zlay_driver_opengl)
    target_link_libraries(zlay_example_basic PRIVATE zlay_driver_opengl)
    target_link_libraries(zlay_example_basic_c PRIVATE zlay_driver_opengl)
    target_link_libraries(zlay_example_wrapper PRIVATE zlay_driver_opengl)
    target_link_libraries(zlay_example_wrapper_cpp PRIVATE zlay_driver_opengl)
  endif()

  if (TARGET zlay_renderer)
    target_link_libraries(zlay_example_basic PRIVATE zlay_renderer)
    target_link_libraries(zlay_example_basic_c PRIVATE zlay_renderer)
    target_link_libraries(zlay_example_wrapper PRIVATE zlay_renderer)
    target_link_libraries(zlay_example_wrapper_cpp PRIVATE zlay_renderer)
  endif()

  if (TARGET zlay_renderer_backend_opengl)
    target_link_libraries(zlay_example_basic PRIVATE zlay_renderer_backend_opengl)
    target_link_libraries(zlay_example_basic_c PRIVATE zlay_renderer_backend_opengl)
    target_link_libraries(zlay_example_wrapper PRIVATE zlay_renderer_backend_opengl)
    target_link_libraries(zlay_example_wrapper_cpp PRIVATE zlay_renderer_backend_opengl)
  endif()

  if (TARGET zlay_shaders)
    target_link_libraries(zlay_example_basic PRIVATE zlay_shaders)
    target_link_libraries(zlay_example_basic_c PRIVATE zlay_shaders)
    target_link_libraries(zlay_example_wrapper PRIVATE zlay_shaders)
    target_link_libraries(zlay_example_wrapper_cpp PRIVATE zlay_shaders)
  endif()

  if (TARGET zlay_os)
    target_link_libraries(zlay_example_basic PRIVATE zlay_os)
    target_link_libraries(zlay_example_basic_c PRIVATE zlay_os)
    target_link_libraries(zlay_example_wrapper PRIVATE zlay_os)
    target_link_libraries(zlay_example_wrapper_cpp PRIVATE zlay_os)
    if (TARGET zlay_example_gui_x11)
      target_link_libraries(zlay_example_gui_x11 PRIVATE zlay_os)
    endif()
    if (TARGET zlay_example_gui_win32)
      target_link_libraries(zlay_example_gui_win32 PRIVATE zlay_os)
    endif()
    if (TARGET zlay_example_gui_cocoa)
      target_link_libraries(zlay_example_gui_cocoa PRIVATE zlay_os)
    endif()
  endif()
endif()

include(cmake/install.cmake)
