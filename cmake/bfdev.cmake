set(BFDEV_SOURCE_PATH ${PROJECT_SOURCE_DIR}/src)

# FIXME: support use system lib and user specific lib
add_subdirectory(lib/bfdev)
include_directories(${PROJECT_SOURCE_DIR}/lib/bfdev/include)
include_directories(${PROJECT_BINARY_DIR}/lib/bfdev/generated)
include_directories(${CMAKE_SOURCE_DIR}/lib/bfdev/arch/generic/include)
