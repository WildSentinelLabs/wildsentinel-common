# Set neccesary files
file(GLOB STREAM_FILES streams/*.cc)
file(GLOB LOGGING_FILES logging/*.cc)
file(GLOB IMAGING_FILES imaging/*.cc)
file(GLOB PIXEL_FILES pixel/*.cc)
file(GLOB COLOR_SPACES_FILES pixel/color_spaces/*.cc)

# Add library
add_library(wscommon SHARED
    ${STREAM_FILES}
    ${LOGGING_FILES}
    ${IMAGING_FILES}
    ${PIXEL_FILES}
    ${COLOR_SPACES_FILES}
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
install(DIRECTORY ${WSCOMMON_SOURCE_DIR}/include/
    DESTINATION include
)
