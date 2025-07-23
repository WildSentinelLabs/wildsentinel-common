include(CMakeParseArguments)
include(GNUInstallDirs)
include(WsCommonHelpers)

function(wscommon_make_dll)
  set(_dll_name "wscommon_dll")
  set(_dll_libs
      Threads::Threads
      $<$<PLATFORM_ID:Darwin>:-Wl,-framework,CoreFoundation>
  )
  set(_dll_compile_definitions "")
  set(_dll_includes "")
  set(_dll_consume "WSCOMMON_CONSUME_DLL")
  set(_dll_build "WSCOMMON_BUILD_DLL")
  set(_dll_objs "${WSCOMMON_INTERNAL_DLL_TARGETS}")

  add_library(
    ${_dll_name}
    SHARED
      ${WSCOMMON_INTERNAL_DLL_TARGETS}
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

  wscommon_generate_pc_file(${_dll_name})

  install(TARGETS ${_dll_name} EXPORT ${PROJECT_NAME}Targets
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  )

  add_library(ws::${_dll_name} ALIAS ${_dll_name})
endfunction()
