#!/bin/sh
set -e

# Compiler and linker flags
CFLAGS="-Wall -Wextra -ggdb $(pkg-config --cflags raylib)"
LIBS="$(pkg-config --libs raylib) -lm -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -lpthread"

# Directories
SRC_DIR="./src"
BUILD_DIR="./build"

# Output binaries
LIBRARY_OUTPUT="$BUILD_DIR/qlib.so"
EXECUTABLE_OUTPUT="$BUILD_DIR/graphV"

# Color codes
GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m' # No Color

# Function to display colored messages
color_msg() {
    echo -e "$1$2${NC}"
}

# Function to build the shared library
build_library() {
    color_msg "${GREEN}" "Building qlib.so..."
    clang $CFLAGS -o "$LIBRARY_OUTPUT" -fPIC -shared "$SRC_DIR/qlib.c" "$SRC_DIR/graph.c" $LIBS || {
        color_msg "${RED}" "Error: Failed to build qlib.so."
        exit 1
    }
    color_msg "${GREEN}" "qlib.so built successfully."
}

# Function to build the main program
build_program() {
    color_msg "${GREEN}" "Building graphV..."
    export LD_LIBRARY_PATH="$BUILD_DIR"
    clang $CFLAGS -o "$EXECUTABLE_OUTPUT" "$SRC_DIR/main.c" $LIBS -L"$BUILD_DIR" || {
        color_msg "${RED}" "Error: Failed to build graphV."
        exit 1
    }
    color_msg "${GREEN}" "Project built successfully."
}

# Parse command-line arguments
while [ "$#" -gt 0 ]; do
    case $1 in
        --build)
            BUILD=true
            ;;
        --run)
            RUN=true
            ;;
        --clean)
            CLEAN=true
            ;;
        *)
            color_msg "${RED}" "Error: Unknown option: $1"
            exit 1
            ;;
    esac
    shift
done

# Create build directory if necessary
[ "$BUILD" = true ] && mkdir -p "$BUILD_DIR"

# Clean build directory if requested
if [ "$CLEAN" = true ]; then
    color_msg "${GREEN}" "Cleaning build directory..."
    rm -rf "$BUILD_DIR"
    color_msg "${GREEN}" "Build directory cleaned."
fi

# Build the shared library libplug.so if needed
if [ "$BUILD" = true ]; then
    set -xe
    build_library
fi

# Build the main program audioVisualizer if needed
if [ "$RUN" = true ]; then
    build_program
    "$EXECUTABLE_OUTPUT"
fi

# Print build success message
[ "$BUILD" = true ] && color_msg "${GREEN}" "Build completed successfully."
