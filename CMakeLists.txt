cmake_minimum_required(VERSION 3.25)
project(adbd LANGUAGES C)


set(BFDEV_SOURCE_PATH ${PROJECT_SOURCE_DIR}/src)

add_subdirectory(src/bfdev)
include_directories(${PROJECT_SOURCE_DIR}/src/bfdev/include)
include_directories(${PROJECT_BINARY_DIR}/src/bfdev/generated)
include_directories(${PROJECT_SOURCE_DIR}/src)


file(GLOB HEADERS
    ${PROJECT_SOURCE_DIR}/include/*.h
    ${PROJECT_SOURCE_DIR}/src/*.h
)

file(GLOB SRCS
    ${HEADERS}
    ${PROJECT_SOURCE_DIR}/src/*.c
)


add_executable(adbd ${SRCS})

target_link_libraries(adbd bfdev)

install(TARGETS
    adbd
    DESTINATION ${CMAKE_INSTALL_BINDIR}
)
