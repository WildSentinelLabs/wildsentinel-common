add_subdirectory(ws)

if(WSCOMMON_BUILD_DLL)
  wscommon_make_dll()
endif()

if(WSCOMMON_ENABLE_INSTALL)
  install(DIRECTORY ws
  DESTINATION ${WSCOMMON_INSTALL_INCLUDEDIR}
  FILES_MATCHING
    PATTERN "*.h"
    PATTERN "detail" EXCLUDE
    PATTERN "*/detail/*" EXCLUDE
  )
endif()
