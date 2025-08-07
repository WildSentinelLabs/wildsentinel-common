# Based on Abseil (https://github.com/abseil/abseil-cpp)
# See THIRD-PARTY-NOTICES

include(CMakeParseArguments)
include(CMakePackageConfigHelpers)
include(WsCommonConfigureCopts)

if(NOT DEFINED WSCOMMON_IDE_FOLDER)
  set(WSCOMMON_IDE_FOLDER WsCommon)
endif()

if(WSCOMMON_USE_SYSTEM_INCLUDES)
  set(WSCOMMON_INTERNAL_INCLUDE_WARNING_GUARD SYSTEM)
else()
  set(WSCOMMON_INTERNAL_INCLUDE_WARNING_GUARD "")
endif()

set(WSCOMMON_INTERNAL_DLL_TARGETS "" CACHE INTERNAL "Internal list of target object generators for wscommon_dll")
function(wscommon_add_dll_library target_obj)
  set(_old "${WSCOMMON_INTERNAL_DLL_TARGETS}")
  list(APPEND _old "$<TARGET_OBJECTS:${target_obj}>")
  set(WSCOMMON_INTERNAL_DLL_TARGETS "${_old}" CACHE INTERNAL "" FORCE)
endfunction()

function(wscommon_generate_pc_file target)
  get_target_property(_lib_name ${target} OUTPUT_NAME)
  if(NOT _lib_name)
    set(_lib_name ${target})
  endif()

  set(pc_version $<IF:$<BOOL:${WSCOMMON_VERSION}>,${WSCOMMON_VERSION},head>)
  string(REGEX MATCH "^wscommon_" has_prefix "${_lib_name}")
  if(has_prefix)
    set(pc_name "${_lib_name}")
  else()
    set(pc_name "wscommon_${_lib_name}")
  endif()

  get_target_property(_deps ${target} INTERFACE_LINK_LIBRARIES)
  list(FILTER _deps INCLUDE REGEX "^ws::")
  set(_pc_requires_list "")
  foreach(dep IN LISTS _deps)
    string(REGEX REPLACE "^ws::" "" _d ${dep})
    list(APPEND _pc_requires_list "wscommon_${_d}")
  endforeach()
  string(JOIN ", " pc_requires ${_pc_requires_list})

  get_target_property(_cflags ${target} INTERFACE_COMPILE_OPTIONS)
  get_target_property(_ldflags ${target} INTERFACE_LINK_OPTIONS)

  string(REGEX REPLACE "^wscommon_" "" _link_name "${pc_name}")
  set(pc_libs   "-L\${libdir} ${_ldflags} -l${_link_name}")
  set(pc_cflags "-I\${includedir} ${_cflags}")

  FILE(GENERATE OUTPUT "${CMAKE_BINARY_DIR}/lib/pkgconfig/${pc_name}.pc" CONTENT
"prefix=${CMAKE_INSTALL_PREFIX}\n\
exec_prefix=\${prefix}\n\
libdir=${CMAKE_INSTALL_FULL_LIBDIR}\n\
includedir=${CMAKE_INSTALL_FULL_INCLUDEDIR}\n\
\n\
Name: ${pc_name}\n\
Description: ${PROJECT_NAME} ${pc_name} library\n\
URL: https://wildsentinel.io/\n\
Version: ${pc_version}\n\
Requires: ${pc_requires}\n\
Libs: ${pc_libs}\n\
Cflags: ${pc_cflags}\n")
  INSTALL(FILES "${CMAKE_BINARY_DIR}/lib/pkgconfig/${pc_name}.pc"
            DESTINATION "${CMAKE_INSTALL_LIBDIR}/pkgconfig")
endfunction()

function(wscommon_install_target target_name)
  install(TARGETS ${target_name} EXPORT ${PROJECT_NAME}Targets
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  )

  get_target_property(_target_type ${target_name} TYPE)
  if(_target_type STREQUAL "EXECUTABLE" OR
     _target_type STREQUAL "SHARED_LIBRARY" OR
     _target_type STREQUAL "MODULE_LIBRARY")
    install(FILES
      $<TARGET_RUNTIME_DLLS:${target_name}>
      DESTINATION ${CMAKE_INSTALL_BINDIR}
      OPTIONAL
    )
  endif()
endfunction()

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

  set(_NAME "wscommon_${WSCOMMON_CC_LIB_NAME}")

  set(_srcs ${WSCOMMON_CC_LIB_SRCS})
  list(FILTER _srcs EXCLUDE REGEX ".*\\.(h|inc|hpp)$")
  list(LENGTH _srcs _n)

  if(_n EQUAL 0)
    add_library(${_NAME} INTERFACE)
    target_include_directories(${_NAME}
      INTERFACE
        "$<BUILD_INTERFACE:${WSCOMMON_INCLUDE_DIRS}>"
        "$<INSTALL_INTERFACE:${WSCOMMON_INSTALL_INCLUDEDIR}>"
    )
    target_link_libraries(${_NAME}
      INTERFACE
        ${WSCOMMON_CC_LIB_DEPS}
        ${WSCOMMON_CC_LIB_LINKOPTS}
        ${WSCOMMON_DEFAULT_LINKOPTS}
    )
    target_compile_definitions(${_NAME} INTERFACE ${WSCOMMON_CC_LIB_DEFINES})

    if(WSCOMMON_PROPAGATE_CXX_STD)
      target_compile_features(${_NAME} INTERFACE ${WSCOMMON_INTERNAL_CXX_STD_FEATURE})
    endif()

    target_precompile_headers(${_NAME} INTERFACE
      "$<$<COMPILE_LANGUAGE:CXX>:pch.h>"
    )
  else()
    if(WSCOMMON_BUILD_DLL)
      set(_build_type OBJECT)
    elseif(BUILD_SHARED_LIBS)
      set(_build_type SHARED)
    else()
      set(_build_type STATIC)
    endif()

    add_library(${_NAME} ${_build_type} ${WSCOMMON_CC_LIB_SRCS})
    target_include_directories(${_NAME}
      PRIVATE ${WSCOMMON_INTERNAL_INCLUDE_WARNING_GUARD}
      PUBLIC
        "$<BUILD_INTERFACE:${WSCOMMON_INCLUDE_DIRS}>"
        "$<INSTALL_INTERFACE:${WSCOMMON_INSTALL_INCLUDEDIR}>"
    )
    target_sources(${_NAME}
      PRIVATE
        ${WSCOMMON_CC_LIB_HDRS}
    )
    target_compile_options(${_NAME} PRIVATE ${WSCOMMON_CC_LIB_COPTS})
    target_compile_definitions(${_NAME} PUBLIC ${WSCOMMON_CC_LIB_DEFINES})

    if(_build_type STREQUAL "OBJECT")
      target_link_libraries(${_NAME} PUBLIC ${WSCOMMON_CC_LIB_DEPS})

      target_compile_definitions(${_NAME} PUBLIC
        $<$<BOOL:${WSCOMMON_CC_LIB_TESTONLY}>:GTEST_LINKED_AS_SHARED_LIBRARY=1>
        WSCOMMON_CONSUME_DLL
      )

      set_property(TARGET ${_NAME} PROPERTY FOLDER ${WSCOMMON_IDE_FOLDER}/internal/objects)

      wscommon_add_dll_library(${_NAME})
    else()
      set_target_properties(${_NAME} PROPERTIES LINKER_LANGUAGE "CXX")

      if(_build_type STREQUAL "SHARED")
        if(APPLE)
          set_target_properties(${_NAME} PROPERTIES INSTALL_RPATH "@loader_path")
        elseif(UNIX)
          set_target_properties(${_NAME} PROPERTIES INSTALL_RPATH "$ORIGIN")
        endif()
      endif()

      target_link_libraries(${_NAME}
        PUBLIC  ${WSCOMMON_CC_LIB_DEPS}
        PRIVATE ${WSCOMMON_CC_LIB_LINKOPTS} ${WSCOMMON_DEFAULT_LINKOPTS}
      )

      if(WSCOMMON_CC_LIB_PUBLIC)
        set_property(TARGET ${_NAME} PROPERTY FOLDER ${WSCOMMON_IDE_FOLDER})
      elseif(WSCOMMON_CC_LIB_TESTONLY)
        set_property(TARGET ${_NAME} PROPERTY FOLDER ${WSCOMMON_IDE_FOLDER}/test)
      else()
        set_property(TARGET ${_NAME} PROPERTY FOLDER ${WSCOMMON_IDE_FOLDER}/internal)
      endif()

      if(WSCOMMON_ENABLE_INSTALL)
        set_target_properties(${_NAME} PROPERTIES
          OUTPUT_NAME "${_NAME}"
          SOVERSION "${WSCOMMON_SOVERSION}"
        )
      endif()
    endif()

    if(WSCOMMON_PROPAGATE_CXX_STD)
      target_compile_features(${_NAME} PUBLIC ${WSCOMMON_INTERNAL_CXX_STD_FEATURE})
    endif()

    target_precompile_headers(${_NAME} PRIVATE
      "$<$<COMPILE_LANGUAGE:CXX>:pch.h>"
    )
  endif()

  if(WSCOMMON_ENABLE_INSTALL)
    wscommon_install_target(${_NAME})
  endif()

  add_library(ws::${WSCOMMON_CC_LIB_NAME} ALIAS ${_NAME})
endfunction()
