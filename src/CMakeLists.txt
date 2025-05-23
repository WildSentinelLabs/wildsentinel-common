# Set necessary files
file(GLOB WSCOMMON_IO_FILES io/*/*.cc io/*.cc)
file(GLOB WSCOMMON_IMAGING_FILES "")
file(GLOB WSCOMMON_LOGGING_FILES "")
file(GLOB WSCOMMON_THREADING_FILES threading/*.cc)

if (WSCOMMON_BUILD_IMAGING)
    file(GLOB WSCOMMON_IMAGING_FILES imaging/*.cc imaging/pixel/*.cc imaging/pixel/color_formats/*.cc imaging/pixel/color_formats/*/*.cc )
endif()

if (WSCOMMON_BUILD_LOGGING)
    file(GLOB WSCOMMON_LOGGING_FILES logging/*.cc logging/*/*.cc)
endif()

# Add library
add_library(wscommon SHARED
    ${WSCOMMON_IO_FILES}
    ${WSCOMMON_IMAGING_FILES}
    ${WSCOMMON_LOGGING_FILES}
    ${WSCOMMON_THREADING_FILES}
)

# Include directories for wscommon library
target_include_directories(wscommon
    PUBLIC
        ${WSCOMMON_SOURCE_DIR}/include
)

set_target_properties(wscommon PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${WSCOMMON_LIB_DIR}
    LIBRARY_OUTPUT_DIRECTORY ${WSCOMMON_LIB_DIR}
    RUNTIME_OUTPUT_DIRECTORY ${WSCOMMON_BIN_DIR}
)

target_compile_features(wscommon PUBLIC cxx_std_23)

# Install the shared library
install(TARGETS wscommon
    EXPORT wscommonTargets
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    INCLUDES DESTINATION include
)

# Install public headers
file(GLOB INCLUDE_BASE_FILES ${WSCOMMON_SOURCE_DIR}/include/*.h)
install(FILES
    ${INCLUDE_BASE_FILES}
    DESTINATION include
)

install(
    DIRECTORY
        ${WSCOMMON_SOURCE_DIR}/include/arch
        ${WSCOMMON_SOURCE_DIR}/include/concurrency
        ${WSCOMMON_SOURCE_DIR}/include/io
        ${WSCOMMON_SOURCE_DIR}/include/threading
    DESTINATION include
)

if (WSCOMMON_BUILD_IMAGING)
    install(
        DIRECTORY
            ${WSCOMMON_SOURCE_DIR}/include/imaging
        DESTINATION include
    )
endif()

if (WSCOMMON_BUILD_LOGGING)
    install(
        DIRECTORY
            ${WSCOMMON_SOURCE_DIR}/include/logging
        DESTINATION include
    )
endif()

if (WSCOMMON_BUILD_POOLING)
    install(
        DIRECTORY
            ${WSCOMMON_SOURCE_DIR}/include/pooling
        DESTINATION include
    )
endif()
