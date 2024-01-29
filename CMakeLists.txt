cmake_minimum_required(VERSION 3.25)
project(adbd LANGUAGES C)


include(cmake/bfdev.cmake)

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
