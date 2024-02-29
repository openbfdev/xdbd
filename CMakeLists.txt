cmake_minimum_required(VERSION 3.21)
project(adbd LANGUAGES C)

include(cmake/bfdev.cmake)
include(cmake/sanitize.cmake)

set(HEADERS
    ${PROJECT_SOURCE_DIR}/include/xdbd.h
    ${PROJECT_SOURCE_DIR}/include/xdbd_config.h
)

set(SRCS
    ${HEADERS}
    ${PROJECT_SOURCE_DIR}/src/xdbd.c
    ${PROJECT_SOURCE_DIR}/src/adb/adb.c
    ${PROJECT_SOURCE_DIR}/src/adb/xdbd_adb.c
    ${PROJECT_SOURCE_DIR}/src/adb/xdbd_adb_request.c
    ${PROJECT_SOURCE_DIR}/src/adb/command.c
    ${PROJECT_SOURCE_DIR}/src/adb/packet.c
    ${PROJECT_SOURCE_DIR}/src/connection/connection.c
    ${PROJECT_SOURCE_DIR}/src/event/xdbd_event.c
    ${PROJECT_SOURCE_DIR}/src/event/select/xdbd_select.c
    ${PROJECT_SOURCE_DIR}/src/core/xdbd_pool.c
    ${PROJECT_SOURCE_DIR}/src/core/xdbd_buf.c
)

set(
    XADB_INCLUDES
    ${PROJECT_SOURCE_DIR}/include/
    ${PROJECT_SOURCE_DIR}/src/adb/
    ${PROJECT_SOURCE_DIR}/src/event/
    ${PROJECT_SOURCE_DIR}/src/connection/
    ${PROJECT_SOURCE_DIR}/src/core/
)

include_directories(${XADB_INCLUDES})

add_executable(adbd ${SRCS})

target_link_libraries(adbd bfdev)

install(TARGETS
    adbd
    DESTINATION ${CMAKE_INSTALL_BINDIR}
)
