set(BFDEV_SOURCE_PATH ${PROJECT_SOURCE_DIR}/src)

# FIXME: support use system lib and user specific lib
add_subdirectory(src/bfdev)
include_directories(${PROJECT_SOURCE_DIR}/src/bfdev/include)
include_directories(${PROJECT_BINARY_DIR}/src/bfdev/generated)
include_directories(${PROJECT_SOURCE_DIR}/src)
