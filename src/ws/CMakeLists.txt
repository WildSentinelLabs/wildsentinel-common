wscommon_cc_library(
  NAME
    core
  HDRS
    array.h
    config.h
    delegate.h
    machine.h
    reflection.h
    span.h
    system.h
    types.h
  PCHS
    ws/config.h
  DEPS
    Threads::Threads
  PUBLIC
)

add_subdirectory(concurrency)
add_subdirectory(string)
add_subdirectory(status)

option(WSCOMMON_BUILD_IMAGING "Build imaging and pixel modules" ON)
option(WSCOMMON_BUILD_IO "Build IO module" ON)
option(WSCOMMON_BUILD_LOGGING "Build logging module" ON)
option(WSCOMMON_BUILD_POOLING "Build pooling module" ON)
option(WSCOMMON_BUILD_THREADING "Build threading module" ON)

if(WSCOMMON_BUILD_IMAGING)
  add_subdirectory(imaging)
endif()

if(WSCOMMON_BUILD_IO OR WSCOMMON_BUILD_IMAGING)
  add_subdirectory(io)
endif()

if(WSCOMMON_BUILD_LOGGING OR WSCOMMON_BUILD_IMAGING)
  add_subdirectory(logging)
endif()

if(WSCOMMON_BUILD_POOLING)
  add_subdirectory(pooling)
endif()

if(WSCOMMON_BUILD_THREADING)
  add_subdirectory(threading)
endif()
