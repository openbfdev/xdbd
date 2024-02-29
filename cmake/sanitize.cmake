# SPDX-License-Identifier: GPL-2.0-or-later */
#
# Copyright(c) 2024 John Sanpe <sanpeqf@gmail.com>
#

option(XDBD_ASAN "Enable Address Sanitizer" OFF)
option(XDBD_UBSAN "Enable Undefined Behaviour Sanitizer" OFF)
option(XDBD_GCOV "Enable Code Coverage Test" OFF)

if(XDBD_ASAN)
    set(CMAKE_C_FLAGS
        "${CMAKE_C_FLAGS} \
         -fsanitize=address \
         -fsanitize=undefined \
         -fsanitize-recover=all \
         -fno-omit-frame-pointer \
         -fno-stack-protector"
    )
    if(NOT APPLE)
        set(CMAKE_C_FLAGS
            "${CMAKE_C_FLAGS} \
             -fsanitize=leak"
        )
    endif()
endif()

if(XDBD_UBSAN)
    set(CMAKE_C_FLAGS
        "${CMAKE_C_FLAGS} \
         -fsanitize=alignment \
         -fsanitize=bounds \
         -fsanitize=shift \
         -fsanitize=integer-divide-by-zero \
         -fsanitize=unreachable \
         -fsanitize=bool \
         -fsanitize=enum \
         -fsanitize-undefined-trap-on-error"
    )
endif()

if(XDBD_GCOV)
    set(CMAKE_C_FLAGS
        "${CMAKE_C_FLAGS} \
         -fprofile-arcs \
         -ftest-coverage"
    )
endif()
