# Set necessary files
file(GLOB WSCOMMON_STREAM_FILES streams/*.cc)
file(GLOB WSCOMMON_LOGGING_FILES logging/*.cc)
file(GLOB WSCOMMON_THREADING_FILES threading/*.cc)
file(GLOB WSCOMMON_IMAGING_FILES "")
file(GLOB WSCOMMON_PIXEL_FILES "")
file(GLOB WSCOMMON_COLOR_SPACES_FILES "")


if (WSCOMMON_BUILD_IMAGING)
    file(GLOB WSCOMMON_IMAGING_FILES imaging/*.cc)
    file(GLOB WSCOMMON_PIXEL_FILES pixel/*.cc)
    file(GLOB WSCOMMON_COLOR_SPACES_FILES pixel/color_spaces/*/*.cc)
endif()

# Add library
add_library(wscommon SHARED
    ${WSCOMMON_STREAM_FILES}
    ${WSCOMMON_LOGGING_FILES}
    ${WSCOMMON_THREADING_FILES}
    ${WSCOMMON_IMAGING_FILES}
    ${WSCOMMON_PIXEL_FILES}
    ${WSCOMMON_COLOR_SPACES_FILES}
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
install(
    DIRECTORY
        ${WSCOMMON_SOURCE_DIR}/include/file_handling
        ${WSCOMMON_SOURCE_DIR}/include/logging
        ${WSCOMMON_SOURCE_DIR}/include/streams
    DESTINATION include
)

if (WSCOMMON_BUILD_IMAGING)
    install(
        DIRECTORY
            ${WSCOMMON_SOURCE_DIR}/include/imaging
            ${WSCOMMON_SOURCE_DIR}/include/pixel
        DESTINATION include
    )
endif()
