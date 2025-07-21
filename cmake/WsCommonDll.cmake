include(CMakeParseArguments)
include(GNUInstallDirs)

set(WSCOMMON_INTERNAL_DLL_TARGETS
  "concurrency"
  "imaging"
  "io"
  "logging"
  "pooling"
  "status"
  "string"
  "threading"
  "core"
)

set(WSCOMMON_INTERNAL_DLL_FILES "" CACHE INTERNAL "Internal DLL source files")

function(wscommon_add_dll_sources)
  cmake_parse_arguments(PARSE_ARGV 0 ARG "" "TARGET" "SOURCES")

  if(ARG_TARGET)
    list(APPEND WSCOMMON_INTERNAL_DLL_FILES $<TARGET_OBJECTS:${ARG_TARGET}>)
    set(WSCOMMON_INTERNAL_DLL_FILES ${WSCOMMON_INTERNAL_DLL_FILES} CACHE INTERNAL "Internal DLL source files")
  endif()

  if(ARG_SOURCES)
    list(APPEND WSCOMMON_INTERNAL_DLL_FILES ${ARG_SOURCES})
    set(WSCOMMON_INTERNAL_DLL_FILES ${WSCOMMON_INTERNAL_DLL_FILES} CACHE INTERNAL "Internal DLL source files")
  endif()
endfunction()

function(wscommon_internal_dll_contains)
  cmake_parse_arguments(WSCOMMON_INTERNAL_DLL
    ""
    "OUTPUT;TARGET"
    ""
    ${ARGN}
  )

  STRING(REGEX REPLACE "^ws::" "" _target ${WSCOMMON_INTERNAL_DLL_TARGET})

  if (_target IN_LIST WSCOMMON_INTERNAL_DLL_TARGETS)
    set(${WSCOMMON_INTERNAL_DLL_OUTPUT} 1 PARENT_SCOPE)
  else()
    set(${WSCOMMON_INTERNAL_DLL_OUTPUT} 0 PARENT_SCOPE)
  endif()
endfunction()

function(wscommon_internal_dll_targets)
  cmake_parse_arguments(WSCOMMON_INTERNAL_DLL
  ""
  "OUTPUT"
  "DEPS"
  ${ARGN}
  )

  set(_deps "")
  foreach(dep IN LISTS WSCOMMON_INTERNAL_DLL_DEPS)
    wscommon_internal_dll_contains(TARGET ${dep} OUTPUT _dll_contains)
    if (_dll_contains)
      list(APPEND _deps wscommon_dll)
    else()
      list(APPEND _deps ${dep})
    endif()
  endforeach()

  list(REMOVE_DUPLICATES _deps)
  set(${WSCOMMON_INTERNAL_DLL_OUTPUT} "${_deps}" PARENT_SCOPE)
endfunction()

function(wscommon_make_dll)
  set(_dll_name "wscommon_dll")
  set(_dll_files ${WSCOMMON_INTERNAL_DLL_FILES})
  set(_dll_libs
      Threads::Threads
      # minimum CMake version >= 3.24
      $<$<PLATFORM_ID:Darwin>:-Wl,-framework,CoreFoundation>
  )
  set(_dll_compile_definitions "")
  set(_dll_includes "")
  set(_dll_consume "WSCOMMON_CONSUME_DLL")
  set(_dll_build "WSCOMMON_BUILD_DLL")

  add_library(
    ${_dll_name}
    SHARED
      ${_dll_files}
  )

  target_link_libraries(
    ${_dll_name}
    PRIVATE
      ${_dll_libs}
      ${WSCOMMON_DEFAULT_LINKOPTS}
      $<$<BOOL:${ANDROID}>:-llog>
  )

  set_target_properties(${_dll_name} PROPERTIES
    LINKER_LANGUAGE "CXX"
    SOVERSION ${WSCOMMON_SOVERSION}
  )

  target_include_directories(
    ${_dll_name}
    PUBLIC
      "$<BUILD_INTERFACE:${WSCOMMON_INCLUDE_DIRS}>"
      "$<INSTALL_INTERFACE:${WSCOMMON_INSTALL_INCLUDEDIR}>"
    PRIVATE
      ${_dll_includes}
  )

  target_compile_options(
    ${_dll_name}
    PRIVATE
      ${WSCOMMON_DEFAULT_COPTS}
  )

  foreach(cflag ${WSCOMMON_CC_LIB_COPTS})
    if(${cflag} MATCHES "^(-Wno|/wd)")
      set(PC_CFLAGS "${PC_CFLAGS} ${cflag}")
    elseif(${cflag} MATCHES "^(-W|/w[1234eo])")
    else()
      set(PC_CFLAGS "${PC_CFLAGS} ${cflag}")
    endif()
  endforeach()
  string(REPLACE ";" " " PC_LINKOPTS "${WSCOMMON_CC_LIB_LINKOPTS}")

  FILE(GENERATE OUTPUT "${CMAKE_BINARY_DIR}/lib/pkgconfig/${_dll_name}.pc" CONTENT "\
prefix=${CMAKE_INSTALL_PREFIX}\n\
exec_prefix=\${prefix}\n\
libdir=${CMAKE_INSTALL_FULL_LIBDIR}\n\
includedir=${CMAKE_INSTALL_FULL_INCLUDEDIR}\n\
\n\
Name: ${_dll_name}\n\
Description: ${PROJECT_NAME} DLL library\n\
URL: https://wildsentinel.io/\n\
Version: ${WSCOMMON_VERSION}\n\
Libs: -L\${libdir} $<$<NOT:$<BOOL:${WSCOMMON_CC_LIB_IS_INTERFACE}>>:-l${_dll_name}> ${PC_LINKOPTS}\n\
Cflags: -I\${includedir}${PC_CFLAGS}\n")
  INSTALL(FILES "${CMAKE_BINARY_DIR}/lib/pkgconfig/${_dll_name}.pc"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/pkgconfig")

  target_compile_definitions(
    ${_dll_name}
    PUBLIC
      ${_dll_compile_definitions}
    PRIVATE
      ${_dll_build}
      NOMINMAX
    INTERFACE
      ${WSCOMMON_CC_LIB_DEFINES}
      ${_dll_consume}
  )

  install(TARGETS ${_dll_name} EXPORT ${PROJECT_NAME}Targets
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  )

  add_library(ws::${_dll_name} ALIAS ${_dll_name})
endfunction()

