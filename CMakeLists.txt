cmake_minimum_required(VERSION 3.24)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE BOOL "Export compile_commands.json for clangd and other language servers")

project(ZLay VERSION 0.1.0 LANGUAGES C CXX)

include(GNUInstallDirs)

option(BUILD_SHARED_LIBS "Build ZLay as a shared library instead of static" ON)
option(ZLAY_BUILD_EXAMPLES "Build ZLay examples" ON)
option(ZLAY_BUILD_DRIVERS "Build OpenGL/Vulkan drivers (stubs by default)" ON)
option(ZLAY_BUILD_OS "Build ZLay OS helpers (timer, etc.)" ON)
option(ZLAY_BUILD_LIFECYCLE "Build ZLay lifecycle, autosave, journal, and recovery helpers" ON)
option(ZLAY_BUILD_PARSER "Build ZLay DSL parser frontend" ON)
option(ZLAY_BUILD_RENDERER "Build ZLay renderer abstraction" ON)
option(ZLAY_BUILD_SHADERS "Build ZLay built-in shader sources" ON)
option(ZLAY_CLANGD_COPY_COMPILE_COMMANDS "Copy compile_commands.json to the source root for clangd" ${PROJECT_IS_TOP_LEVEL})
set(ZLAY_HEADER_TEMPLATE_STYLE "ifndef" CACHE STRING "Default generated header style: ifndef, pragma_once, both")
set_property(CACHE ZLAY_HEADER_TEMPLATE_STYLE PROPERTY STRINGS ifndef pragma_once both)
set(ZLAY_HEADER_TEMPLATE_INCLUDE "zlay.h" CACHE STRING "Default include used by generated ZLay headers; empty disables the include")
option(ZLAY_HEADER_TEMPLATE_EXTERN_C "Wrap generated ZLay headers with extern C guards" ON)
set(ZLAY_NEW_HEADER_PATH "" CACHE FILEPATH "Header path used by the zlay_generate_header target")
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

file(GLOB ZLAY_RENDERER_SOURCES CONFIGURE_DEPENDS
  ${ZLAY_SOURCE_DIR}/renderer/*.c
)
list(SORT ZLAY_RENDERER_SOURCES)

file(GLOB ZLAY_RENDERER_HEADERS CONFIGURE_DEPENDS
  ${ZLAY_SOURCE_DIR}/renderer/*.h
)
list(SORT ZLAY_RENDERER_HEADERS)

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
  ${ZLAY_SOURCE_DIR}/driver/*.c
)
list(SORT ZLAY_DRIVER_CORE_SOURCES)

file(GLOB ZLAY_DRIVER_CORE_HEADERS CONFIGURE_DEPENDS
  ${ZLAY_SOURCE_DIR}/driver/*.h
)
list(SORT ZLAY_DRIVER_CORE_HEADERS)

file(GLOB ZLAY_DRIVER_OPENGL_SOURCES CONFIGURE_DEPENDS
  ${ZLAY_SOURCE_DIR}/driver/opengl/*.c
)
list(SORT ZLAY_DRIVER_OPENGL_SOURCES)

file(GLOB ZLAY_DRIVER_OPENGL_HEADERS CONFIGURE_DEPENDS
  ${ZLAY_SOURCE_DIR}/driver/opengl/*.h
)
list(SORT ZLAY_DRIVER_OPENGL_HEADERS)

file(GLOB ZLAY_DRIVER_VULKAN_SOURCES CONFIGURE_DEPENDS
  ${ZLAY_SOURCE_DIR}/driver/vulkan/*.c
)
list(SORT ZLAY_DRIVER_VULKAN_SOURCES)

file(GLOB ZLAY_DRIVER_VULKAN_HEADERS CONFIGURE_DEPENDS
  ${ZLAY_SOURCE_DIR}/driver/vulkan/*.h
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

file(GLOB ZLAY_OS_HEADERS CONFIGURE_DEPENDS
  ${ZLAY_SOURCE_DIR}/os/*.h
)
list(SORT ZLAY_OS_HEADERS)

file(GLOB ZLAY_OS_WIN32_SOURCES CONFIGURE_DEPENDS ${ZLAY_SOURCE_DIR}/os/win32/*.c)
file(GLOB ZLAY_OS_WINRT_SOURCES CONFIGURE_DEPENDS ${ZLAY_SOURCE_DIR}/os/winrt/*.c)
file(GLOB ZLAY_OS_COCOA_SOURCES CONFIGURE_DEPENDS ${ZLAY_SOURCE_DIR}/os/cocoa/*.c)
file(GLOB ZLAY_OS_GTK_SOURCES CONFIGURE_DEPENDS ${ZLAY_SOURCE_DIR}/os/gtk/*.c)
file(GLOB ZLAY_OS_X11_SOURCES CONFIGURE_DEPENDS ${ZLAY_SOURCE_DIR}/os/libx11/*.c)

file(GLOB ZLAY_OS_WIN32_HEADERS CONFIGURE_DEPENDS ${ZLAY_SOURCE_DIR}/os/win32/*.h)
file(GLOB ZLAY_OS_WINRT_HEADERS CONFIGURE_DEPENDS ${ZLAY_SOURCE_DIR}/os/winrt/*.h)
file(GLOB ZLAY_OS_COCOA_HEADERS CONFIGURE_DEPENDS ${ZLAY_SOURCE_DIR}/os/cocoa/*.h)
file(GLOB ZLAY_OS_GTK_HEADERS CONFIGURE_DEPENDS ${ZLAY_SOURCE_DIR}/os/gtk/*.h)
file(GLOB ZLAY_OS_X11_HEADERS CONFIGURE_DEPENDS ${ZLAY_SOURCE_DIR}/os/libx11/*.h)

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
)

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
  target_sources(zlay PRIVATE ${ZLAY_DRIVER_OPENGL_SOURCES} ${ZLAY_DRIVER_OPENGL_HEADERS})
  target_sources(zlay PRIVATE ${ZLAY_DRIVER_VULKAN_SOURCES} ${ZLAY_DRIVER_VULKAN_HEADERS})

  add_library(zlay_driver INTERFACE)
  add_library(zlay::driver ALIAS zlay_driver)
  set_target_properties(zlay_driver PROPERTIES EXPORT_NAME driver)
  target_link_libraries(zlay_driver INTERFACE zlay)

  add_library(zlay_driver_opengl INTERFACE)
  add_library(zlay::driver_opengl ALIAS zlay_driver_opengl)
  set_target_properties(zlay_driver_opengl PROPERTIES EXPORT_NAME driver_opengl)
  target_link_libraries(zlay_driver_opengl INTERFACE zlay_driver)

  add_library(zlay_driver_vulkan INTERFACE)
  add_library(zlay::driver_vulkan ALIAS zlay_driver_vulkan)
  set_target_properties(zlay_driver_vulkan PROPERTIES EXPORT_NAME driver_vulkan)
  target_link_libraries(zlay_driver_vulkan INTERFACE zlay_driver)
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
