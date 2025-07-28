add_subdirectory(ws)

if(WSCOMMON_BUILD_DLL)
  wscommon_make_dll()
endif()
