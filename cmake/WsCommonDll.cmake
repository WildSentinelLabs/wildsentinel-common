option(WSCOMMON_BUILD_DLL "Build WsHelpers as DLL (Windows)" OFF)

if(WIN32 AND WSCOMMON_BUILD_DLL)
  set(WSCOMMON_USE_DLL TRUE)
else()
  set(WSCOMMON_USE_DLL FALSE)
endif()

function(wscommon_configure_visibility target)
  if(WSCOMMON_USE_DLL)
    target_compile_definitions(${target} PUBLIC WSCOMMON_DLL)
  endif()
endfunction()

set(_wscommon_dll_object_libs "" CACHE INTERNAL "Object libraries for wscommon DLL")
function(wscommon_make_dll)
  set(dll_name "wscommon_dll")

  add_library(${dll_name} SHARED ${_wscommon_dll_object_libs})

  target_include_directories(${dll_name} PUBLIC
    "$<BUILD_INTERFACE:${WSCOMMON_SOURCE_DIR}/src>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/ws>"
  )

  target_compile_definitions(${dll_name} PUBLIC WSCOMMON_DLL)
  set_target_properties(${dll_name} PROPERTIES
    OUTPUT_NAME "wscommon"
    LINKER_LANGUAGE CXX
    SOVERSION ${PROJECT_VERSION}
  )

  install(TARGETS ${dll_name}
    EXPORT ${PROJECT_NAME}Targets
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  )

  add_library(ws::wscommon ALIAS ${dll_name})
endfunction()

