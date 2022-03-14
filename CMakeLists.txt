cmake_minimum_required(VERSION 3.17)

project(conan_scratch)

set(CMAKE_CXX_STANDARD 20)

set(CONAN_DISABLE_CHECK_COMPILER Yes)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(KEEP_RPATHS)

add_executable(conan_scratch main.cpp)
target_link_libraries(conan_scratch
        ${CONAN_LIBS}
)
