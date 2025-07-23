include(CMakeParseArguments)
include(WsCommonDll)

if(WSCOMMON_USE_SYSTEM_INCLUDES)
  set(WSCOMMON_INTERNAL_INCLUDE_WARNING_GUARD SYSTEM)
else()
  set(WSCOMMON_INTERNAL_INCLUDE_WARNING_GUARD "")
endif()

function(wscommon_cc_library)
  cmake_parse_arguments(WSCOMMON_CC_LIB
    "DISABLE_INSTALL;PUBLIC;TESTONLY"
    "NAME"
    "HDRS;SRCS;COPTS;DEFINES;LINKOPTS;DEPS"
    ${ARGN}
  )

  if(WSCOMMON_CC_LIB_TESTONLY AND NOT (BUILD_TESTING AND WSCOMMON_BUILD_TESTING))
    return()
  endif()

  if(WSCOMMON_ENABLE_INSTALL)
    set(_NAME "${WSCOMMON_CC_LIB_NAME}")
  else()
    set(_NAME "wscommon_${WSCOMMON_CC_LIB_NAME}")
  endif()

  set(WSCOMMON_CC_SRCS "${WSCOMMON_CC_LIB_SRCS}")
  foreach(src_file IN LISTS WSCOMMON_CC_SRCS)
    if(${src_file} MATCHES ".*\\.(h|inc)$")
      list(REMOVE_ITEM WSCOMMON_CC_SRCS "${src_file}")
    endif()
  endforeach()

  if(WSCOMMON_CC_SRCS STREQUAL "")
    set(WSCOMMON_CC_LIB_IS_INTERFACE 1)
  else()
    set(WSCOMMON_CC_LIB_IS_INTERFACE 0)
  endif()

  if(${WSCOMMON_BUILD_DLL})
    if(WSCOMMON_ENABLE_INSTALL)
      wscommon_internal_dll_contains(TARGET ${_NAME} OUTPUT _in_dll)
    else()
      wscommon_internal_dll_contains(TARGET ${WSCOMMON_CC_LIB_NAME} OUTPUT _in_dll)
    endif()
    if (${_in_dll})
      set(_build_type "dll")
      set(WSCOMMON_CC_LIB_IS_INTERFACE 1)
    else()
      set(_build_type "dll_dep")
    endif()
  elseif(BUILD_SHARED_LIBS)
    set(_build_type "shared")
  else()
    set(_build_type "static")
  endif()

  if(WSCOMMON_ENABLE_INSTALL)
    if(WSCOMMON_VERSION)
      set(PC_VERSION "${WSCOMMON_VERSION}")
    else()
      set(PC_VERSION "head")
    endif()
    if(NOT _build_type STREQUAL "dll")
      set(LNK_LIB "${LNK_LIB} -lwscommon_${_NAME}")
    endif()
    foreach(dep ${WSCOMMON_CC_LIB_DEPS})
      if(${dep} MATCHES "^ws::(.*)")
        if(_build_type STREQUAL "dll")
          if(NOT PC_DEPS MATCHES "wscommon_dll")
            if(PC_DEPS)
              set(PC_DEPS "${PC_DEPS},")
            endif()
            set(PC_DEPS "${PC_DEPS} wscommon_dll = ${PC_VERSION}")
            set(LNK_LIB "${LNK_LIB} -lwscommon_dll")
          endif()
        else()
          if(PC_DEPS)
            set(PC_DEPS "${PC_DEPS},")
          endif()
          set(PC_DEPS "${PC_DEPS} wscommon_${CMAKE_MATCH_1} = ${PC_VERSION}")
        endif()
      endif()
    endforeach()
    foreach(cflag ${WSCOMMON_CC_LIB_COPTS})
      string(REGEX REPLACE "^SHELL:" "" cflag "${cflag}")
      if(${cflag} MATCHES "^-Xarch_")
      elseif(${cflag} MATCHES "^(-Wno-|/wd)")
        set(PC_CFLAGS "${PC_CFLAGS} ${cflag}")
      elseif(${cflag} MATCHES "^(-W|/w[1234eo])")
      elseif(${cflag} MATCHES "^-m")
      else()
        set(PC_CFLAGS "${PC_CFLAGS} ${cflag}")
      endif()
    endforeach()
    string(REPLACE ";" " " PC_LINKOPTS "${WSCOMMON_CC_LIB_LINKOPTS}")

    FILE(GENERATE OUTPUT "${CMAKE_BINARY_DIR}/lib/pkgconfig/wscommon_${_NAME}.pc" CONTENT "\
prefix=${CMAKE_INSTALL_PREFIX}\n\
exec_prefix=\${prefix}\n\
libdir=${CMAKE_INSTALL_FULL_LIBDIR}\n\
includedir=${CMAKE_INSTALL_FULL_INCLUDEDIR}\n\
\n\
Name: wscommon_${_NAME}\n\
Description: ${PROJECT_NAME} ${_NAME} library\n\
URL: https://wildsentinel.io/\n\
Version: ${PC_VERSION}\n\
Requires:${PC_DEPS}\n\
Libs: -L\${libdir} $<$<NOT:$<BOOL:${WSCOMMON_CC_LIB_IS_INTERFACE}>>:${LNK_LIB}> ${PC_LINKOPTS}\n\
Cflags: -I\${includedir}${PC_CFLAGS}\n")
    INSTALL(FILES "${CMAKE_BINARY_DIR}/lib/pkgconfig/wscommon_${_NAME}.pc"
            DESTINATION "${CMAKE_INSTALL_LIBDIR}/pkgconfig")
  endif()

  if(NOT WSCOMMON_CC_LIB_IS_INTERFACE)
    if(_build_type STREQUAL "dll_dep")
      add_library(${_NAME} STATIC "")
      target_sources(${_NAME} PRIVATE ${WSCOMMON_CC_LIB_SRCS} ${WSCOMMON_CC_LIB_HDRS})
      wscommon_internal_dll_targets(
        DEPS ${WSCOMMON_CC_LIB_DEPS}
        OUTPUT _dll_deps
      )
      target_link_libraries(${_NAME}
        PUBLIC ${_dll_deps}
        PRIVATE
          ${WSCOMMON_CC_LIB_LINKOPTS}
          ${WSCOMMON_DEFAULT_LINKOPTS}
      )

      if (WSCOMMON_CC_LIB_TESTONLY)
        set(_gtest_link_define "GTEST_LINKED_AS_SHARED_LIBRARY=1")
      else()
        set(_gtest_link_define)
      endif()

      target_compile_definitions(${_NAME}
        PUBLIC
          WSCOMMON_CONSUME_DLL
          "${_gtest_link_define}"
      )
    elseif(_build_type STREQUAL "static" OR _build_type STREQUAL "shared")
      add_library(${_NAME} "")
      target_sources(${_NAME} PRIVATE ${WSCOMMON_CC_LIB_SRCS} ${WSCOMMON_CC_LIB_HDRS})
      if(APPLE)
        set_target_properties(${_NAME} PROPERTIES
          INSTALL_RPATH "@loader_path")
      elseif(UNIX)
        set_target_properties(${_NAME} PROPERTIES
          INSTALL_RPATH "$ORIGIN")
      endif()
      target_link_libraries(${_NAME}
      PUBLIC ${WSCOMMON_CC_LIB_DEPS}
      PRIVATE
        ${WSCOMMON_CC_LIB_LINKOPTS}
        ${WSCOMMON_DEFAULT_LINKOPTS}
      )
    else()
      message(FATAL_ERROR "Invalid build type: ${_build_type}")
    endif()

    set_property(TARGET ${_NAME} PROPERTY LINKER_LANGUAGE "CXX")
    target_include_directories(${_NAME} ${WSCOMMON_INTERNAL_INCLUDE_WARNING_GUARD}
      PUBLIC
        "$<BUILD_INTERFACE:${WSCOMMON_INCLUDE_DIRS}>"
        $<INSTALL_INTERFACE:${WSCOMMON_INSTALL_INCLUDEDIR}>
    )
    target_compile_options(${_NAME}
      PRIVATE ${WSCOMMON_CC_LIB_COPTS})
    target_compile_definitions(${_NAME} PUBLIC ${WSCOMMON_CC_LIB_DEFINES})

    if(WSCOMMON_CC_LIB_PUBLIC)
      set_property(TARGET ${_NAME} PROPERTY FOLDER ${WSCOMMON_IDE_FOLDER})
    elseif(WSCOMMON_CC_LIB_TESTONLY)
      set_property(TARGET ${_NAME} PROPERTY FOLDER ${WSCOMMON_IDE_FOLDER}/test)
    else()
      set_property(TARGET ${_NAME} PROPERTY FOLDER ${WSCOMMON_IDE_FOLDER}/internal)
    endif()

    if(WSCOMMON_ENABLE_INSTALL)
      set_target_properties(${_NAME} PROPERTIES
        OUTPUT_NAME "wscommon_${_NAME}"
        SOVERSION "${WSCOMMON_SOVERSION}"
      )
    endif()
  else()
    add_library(${_NAME} INTERFACE)
    target_include_directories(${_NAME} ${WSCOMMON_INTERNAL_INCLUDE_WARNING_GUARD}
      INTERFACE
        "$<BUILD_INTERFACE:${WSCOMMON_INCLUDE_DIRS}>"
        $<INSTALL_INTERFACE:${WSCOMMON_INSTALL_INCLUDEDIR}>
      )

    if (_build_type STREQUAL "dll")
        set(WSCOMMON_CC_LIB_DEPS wscommon_dll)
    endif()

    target_link_libraries(${_NAME}
      INTERFACE
        ${WSCOMMON_CC_LIB_DEPS}
        ${WSCOMMON_CC_LIB_LINKOPTS}
        ${WSCOMMON_DEFAULT_LINKOPTS}
    )
    target_compile_definitions(${_NAME} INTERFACE ${WSCOMMON_CC_LIB_DEFINES})
  endif()

  if(WSCOMMON_BUILD_DLL AND _build_type STREQUAL "dll")
    set(_dll_sources "")
    foreach(_src IN LISTS WSCOMMON_CC_SRCS)
      if(IS_ABSOLUTE ${_src})
        list(APPEND _dll_sources ${_src})
      else()
        file(RELATIVE_PATH _rel_path "${WSCOMMON_COMMON_INCLUDEDIR}" "${CMAKE_CURRENT_SOURCE_DIR}")
        list(APPEND _dll_sources "${_rel_path}/${_src}")
      endif()
    endforeach()
    wscommon_add_dll_sources(SOURCES ${_dll_sources})
  endif()

  if(WSCOMMON_ENABLE_INSTALL)
    install(TARGETS ${_NAME} EXPORT ${PROJECT_NAME}Targets
      RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
      LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
      ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )
  endif()

  add_library(ws::${WSCOMMON_CC_LIB_NAME} ALIAS ${_NAME})
endfunction()
