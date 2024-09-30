# Variables
CC = gcc
CFLAGS = -Wall -Wl,-rpath=./raylib/src
LDFLAGS = -L./raylib/src -lraylib -lm -lpthread -ldl -lrt -lX11
INCLUDES = -I./raylib/src -Isrc
SRC = src/main.c
BUILD_DIR = build
OUT = $(BUILD_DIR)/graphv
QLIB_SO = $(BUILD_DIR)/qlib.so

RAYLIB_SHARED = ./raylib/src/libraylib.so.5.5.0

all: $(BUILD_DIR) $(OUT) $(QLIB_SO) mandelbrot

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(OUT): $(SRC) $(RAYLIB_SHARED)
	$(CC) $(CFLAGS) $(INCLUDES) $(SRC) -o $(OUT) $(LDFLAGS)

$(QLIB_SO): src/graph.c src/qlib.c $(RAYLIB_SHARED)
	$(CC) -shared -fPIC -o $(QLIB_SO) src/graph.c src/qlib.c $(CFLAGS) $(INCLUDES) $(LDFLAGS)

mandelbrot: $(BUILD_DIR) $(OUT) $(QLIB_SO) src/mandelbrot.c
	$(CC) $(CFLAGS) $(INCLUDES) src/mandelbrot.c -o $(BUILD_DIR)/mandelbrot $(LDFLAGS)
clean:
	rm -rf $(BUILD_DIR)
