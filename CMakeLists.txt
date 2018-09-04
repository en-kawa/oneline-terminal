cmake_minimum_required(VERSION 2.8.4)

project(terminal_port)

set(COMMON_ROOT ${PROJECT_SOURCE_DIR})
set(COMPORT_ROOT ${COMMON_ROOT}/comport)
set(UTIL_ROOT ${COMMON_ROOT}/util)

# Set the directories where include files can be found.
include_directories(${COMPORT_ROOT}/include)
include_directories(${UTIL_ROOT}/include)

# Set the source files.
aux_source_directory(${PROJECT_SOURCE_DIR} MAIN_SRC)
aux_source_directory(${COMPORT_ROOT}/src COMPORT_SRC)

# Set the output directory.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY bin)

# Set CPP flag
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")


# Build the target.
add_executable(terminal_port ${MAIN_SRC} ${COMPORT_SRC} ${UTIL_SRC})
target_link_libraries(terminal_port pthread)
