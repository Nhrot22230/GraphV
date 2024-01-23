cmake_minimum_required(VERSION 3.12)
project(GraphV)

# Compiler and linker flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -ggdb")
find_package(raylib REQUIRED)

# Directories
set(SRC_DIR "${CMAKE_SOURCE_DIR}/src")
set(BUILD_DIR "${CMAKE_SOURCE_DIR}/build")

# Output binaries
set(LIBRARY_OUTPUT "${BUILD_DIR}/qlib.so")
set(EXECUTABLE_OUTPUT "${BUILD_DIR}/graphV")

# Set output directories
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${BUILD_DIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${BUILD_DIR}")

# Function to build the shared library
function(build_library)
    message(STATUS "Building qlib.so...")
    add_library(qlib SHARED "${SRC_DIR}/qlib.c" "${SRC_DIR}/graph.c")
    target_link_libraries(qlib PRIVATE raylib)
    message(STATUS "qlib.so built successfully.")
endfunction()

# Function to build the main program
function(build_program)
    message(STATUS "Building graphV...")
    add_executable(graphV "${SRC_DIR}/main.c")
    target_link_libraries(graphV PRIVATE qlib raylib)
    message(STATUS "Project built successfully.")
endfunction()

# Parse command-line arguments
option(BUILD "Build the project" ON)
option(RUN "Run the project" OFF)
option(CLEAN "Clean the build directory" OFF)

# Create build directory if necessary
if(BUILD)
    file(MAKE_DIRECTORY ${BUILD_DIR})
endif()

# Clean build directory if requested
if(CLEAN)
    message(STATUS "Cleaning build directory...")
    file(REMOVE_RECURSE ${BUILD_DIR})
    message(STATUS "Build directory cleaned.")
endif()

# Build the shared library qlib.so if needed
if(BUILD)
    build_library()
endif()

# Build the main program graphV if needed
if(RUN)
    build_program()
    message(STATUS "Running graphV...")
    add_custom_command(TARGET graphV POST_BUILD COMMAND "${EXECUTABLE_OUTPUT}")
endif()

# Print build success message
if(BUILD)
    message(STATUS "Build completed successfully.")
endif()
