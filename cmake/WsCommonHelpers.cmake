include(CMakeParseArguments)
include(WsCommonDll)

function(wscommon_cc_library)
  cmake_parse_arguments(WSCOMMON_CC_LIB
    "DISABLE_INSTALL;INTERFACE;PUBLIC"
    "NAME"
    "SRCS;HDRS;DEPS;COPTS;DEFINES;LINKOPTS"
    ${ARGN}
  )

  if(NOT WSCOMMON_CC_LIB_NAME)
    message(FATAL_ERROR "wscommon_cc_library: NAME is required")
  endif()

  string(REPLACE "::" "_" target_name "${WSCOMMON_CC_LIB_NAME}")
  string(REGEX REPLACE "^ws::" "" alias_target "${WSCOMMON_CC_LIB_NAME}")
  set(target_name "ws_${alias_target}")
  message(STATUS "wscommon_cc_library: WSCOMMON_SOURCE_DIR=${WSCOMMON_SOURCE_DIR}")

  set(is_header_only ${WSCOMMON_CC_LIB_INTERFACE})
  if(NOT WSCOMMON_CC_LIB_SRCS AND NOT WSCOMMON_CC_LIB_INTERFACE)
    set(is_header_only TRUE)
  endif()

  if(is_header_only)
    add_library(${target_name} INTERFACE)
    target_include_directories(${target_name} INTERFACE
      "$<BUILD_INTERFACE:${WSCOMMON_SOURCE_DIR}/src>"
      "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/ws>"
    )
    target_compile_definitions(${target_name} INTERFACE ${WSCOMMON_CC_LIB_DEFINES})
    target_compile_options(${target_name} INTERFACE ${WSCOMMON_CC_LIB_COPTS})
    target_link_libraries(${target_name} INTERFACE ${WSCOMMON_CC_LIB_DEPS})
  else()
    if(WSCOMMON_MODULAR_BUILD)
      if(BUILD_SHARED_LIBS)
        add_library(${target_name} SHARED ${WSCOMMON_CC_LIB_SRCS} ${WSCOMMON_CC_LIB_HDRS})
      else()
        add_library(${target_name} STATIC ${WSCOMMON_CC_LIB_SRCS} ${WSCOMMON_CC_LIB_HDRS})
      endif()
    else()
      add_library(${target_name} OBJECT ${WSCOMMON_CC_LIB_SRCS})
      set(_wscommon_dll_object_libs ${_wscommon_dll_object_libs}
        $<TARGET_OBJECTS:${target_name}> CACHE INTERNAL "DLL objs")
    endif()

    set_property(TARGET ${target_name} PROPERTY LINKER_LANGUAGE CXX)

    target_include_directories(${target_name} PUBLIC
      "$<BUILD_INTERFACE:${WSCOMMON_SOURCE_DIR}/src>"
      "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/ws>"
    )

    target_compile_definitions(${target_name} PUBLIC ${WSCOMMON_CC_LIB_DEFINES})
    target_compile_options(${target_name} PRIVATE ${WSCOMMON_CC_LIB_COPTS})
    target_link_libraries(${target_name} PUBLIC ${WSCOMMON_CC_LIB_DEPS} ${WSCOMMON_CC_LIB_LINKOPTS})

    if(WSCOMMON_BUILD_DLL AND NOT WSCOMMON_MODULAR_BUILD)
      wscommon_configure_visibility(${target_name})
    endif()
  endif()

  if(WSCOMMON_ENABLE_INSTALL AND NOT WSCOMMON_CC_LIB_DISABLE_INSTALL)
    if(WSCOMMON_MODULAR_BUILD)
      install(TARGETS ${target_name}
        EXPORT ${PROJECT_NAME}Targets
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/ws
      )
    endif()
  endif()

  add_library(${WSCOMMON_CC_LIB_NAME} ALIAS ${target_name})
endfunction()
