include_guard(GLOBAL)

include(CMakePackageConfigHelpers)

set(ZLAY_INSTALL_CMAKE_DIR "${CMAKE_INSTALL_LIBDIR}/cmake/ZLay" CACHE STRING "Install path for ZLay CMake package files")

set(ZLAY_INSTALL_TARGETS zlay)
foreach(_ZLAY_TARGET IN ITEMS
  zlay_core
  zlay_lifecycle
  zlay_renderer
  zlay_parser
  zlay_shaders
  zlay_driver_opengl
  zlay_driver_vulkan
  zlay_os
)
  if(TARGET ${_ZLAY_TARGET})
    list(APPEND ZLAY_INSTALL_TARGETS ${_ZLAY_TARGET})
  endif()
endforeach()

install(
  TARGETS ${ZLAY_INSTALL_TARGETS}
  EXPORT ZLayTargets
  RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
  LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
  ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
  INCLUDES DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)

set(_ZLAY_ROOT_HEADERS)
foreach(_ZLAY_HEADER IN ITEMS
  "${CMAKE_CURRENT_SOURCE_DIR}/zlay.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/zlay_math.h"
)
  if(EXISTS "${_ZLAY_HEADER}")
    list(APPEND _ZLAY_ROOT_HEADERS "${_ZLAY_HEADER}")
  endif()
endforeach()

if(_ZLAY_ROOT_HEADERS)
  install(
    FILES ${_ZLAY_ROOT_HEADERS}
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
  )
endif()

install(
  DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/zlay/"
  DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/zlay"
  FILES_MATCHING
    PATTERN "*.h"
    PATTERN "zlay_internal.h" EXCLUDE
)

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/include")
  install(
    DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include/"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
    FILES_MATCHING
      PATTERN "*.h"
  )
endif()

configure_package_config_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake/ZLayConfig.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/ZLayConfig.cmake"
  INSTALL_DESTINATION "${ZLAY_INSTALL_CMAKE_DIR}"
  PATH_VARS CMAKE_INSTALL_INCLUDEDIR
)

write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/ZLayConfigVersion.cmake"
  VERSION "${PROJECT_VERSION}"
  COMPATIBILITY SameMajorVersion
)

install(
  EXPORT ZLayTargets
  FILE ZLayTargets.cmake
  NAMESPACE zlay::
  DESTINATION "${ZLAY_INSTALL_CMAKE_DIR}"
)

install(
  FILES
    "${CMAKE_CURRENT_BINARY_DIR}/ZLayConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/ZLayConfigVersion.cmake"
  DESTINATION "${ZLAY_INSTALL_CMAKE_DIR}"
)
