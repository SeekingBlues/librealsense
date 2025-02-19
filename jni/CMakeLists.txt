cmake_minimum_required(VERSION 3.13)

if (NOT CMAKE_TOOLCHAIN_FILE)
    message(FATAL_ERROR "Please specify the NDK toolchain with -DCMAKE_TOOLCHAIN_FILE=...")
endif()

set(ANDROID_STL c++_shared)
set(ANDROID_PLATFORM 28)
set(ANDROID_ABI arm64-v8a)

set(SUPER_PROJECT ${CMAKE_CURRENT_SOURCE_DIR}/../..)
set(LIBRARY_OUTPUT_DIR ${SUPER_PROJECT}/libs/${ANDROID_ABI})
if (NOT IS_DIRECTORY ${LIBRARY_OUTPUT_DIR})
    message(FATAL_ERROR "The expected library output directory (${LIBRARY_OUTPUT_DIR}) does not exist")
endif()
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${LIBRARY_OUTPUT_DIR})

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/.. ${CMAKE_CURRENT_SOURCE_DIR}/../build)

project(t265)
add_library(t265 SHARED main.cpp)

target_link_libraries(t265 PRIVATE realsense2 log)
target_compile_options(t265 PRIVATE -g0 -fno-rtti -fdiagnostics-color=always -Wall -Wextra -Werror -O3 -DNDEBUG -std=c++17)
target_link_options(t265 PRIVATE -s -Wl,--gc-sections)

file(COPY "${ANDROID_NDK}/sources/cxx-stl/llvm-libc++/libs/${ANDROID_ABI}/libc++_shared.so" DESTINATION "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}")
