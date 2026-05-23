if(NOT DEFINED ZLAY_COMPILE_COMMANDS_IN OR NOT DEFINED ZLAY_COMPILE_COMMANDS_OUT)
  message(FATAL_ERROR "ZLAY_COMPILE_COMMANDS_IN and ZLAY_COMPILE_COMMANDS_OUT are required")
endif()

if(EXISTS "${ZLAY_COMPILE_COMMANDS_IN}")
  file(COPY_FILE
    "${ZLAY_COMPILE_COMMANDS_IN}"
    "${ZLAY_COMPILE_COMMANDS_OUT}"
    ONLY_IF_DIFFERENT
  )
else()
  message(STATUS "ZLay clangd: compile_commands.json is not available for this generator")
endif()
