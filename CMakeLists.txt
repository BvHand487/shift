cmake_minimum_required(VERSION 3.10)

project(shift)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS_DEBUG "-g")

set(CMAKE_BUILD_TYPE Debug)
file(GLOB SOURCES "src/*.cpp")
add_executable(shift ${SOURCES})

find_package(LLVM REQUIRED CONFIG)
include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

target_include_directories(shift PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(shift PRIVATE LLVM)


