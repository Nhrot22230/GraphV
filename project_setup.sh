#!/bin/sh
set -e

# Compiler and linker flags
CFLAGS="-Wall -Wextra -ggdb $(pkg-config --cflags raylib)"
LIBS="$(pkg-config --libs raylib) -lm -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -lpthread"

# Directories
SRC_DIR="./src"
BUILD_DIR="./build"

# Output binaries
LIBRARY_OUTPUT="$BUILD_DIR/quick_lib.so"
EXECUTABLE_OUTPUT="$BUILD_DIR/musiV"

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
    color_msg "${GREEN}" "Building quick_lib.so..."
    clang $CFLAGS -o "$LIBRARY_OUTPUT" -fPIC -shared "$SRC_DIR/quick_lib.c" "$SRC_DIR/graph.c" $LIBS || {
        color_msg "${RED}" "Error: Failed to build quick_lib.so."
        exit 1
    }
    color_msg "${GREEN}" "quick_lib.so built successfully."
}

# Function to build the main program
build_program() {
    color_msg "${GREEN}" "Building musiV..."
    export LD_LIBRARY_PATH="$BUILD_DIR"
    clang $CFLAGS -o "$EXECUTABLE_OUTPUT" "$SRC_DIR/main.c" $LIBS -L"$BUILD_DIR" || {
        color_msg "${RED}" "Error: Failed to build musiV."
        exit 1
    }
    color_msg "${GREEN}" "Project built successfully."
}

# Parse command-line arguments
for arg in "$@"; do
    case $arg in
        --build)
            BUILD=true
            ;;
        --run)
            RUN=true
            ;;
        *)
            color_msg "${RED}" "Error: Unknown option: $arg"
            exit 1
            ;;
    esac
done

# Create build directory if necessary
[ "$BUILD" = true ] && mkdir -p "$BUILD_DIR"

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

