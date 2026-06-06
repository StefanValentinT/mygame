CC := gcc
CFLAGS := -std=c23 -Wall -Wextra -Wpedantic -O3

SRC_DIR := src
LIB_DIR := lib
BUILD_DIR := build
TARGET := $(BUILD_DIR)/game

macOS_FRAMEWORKS := -framework Cocoa -framework OpenGL -framework IOKit -framework CoreVideo

SRCS := $(wildcard $(SRC_DIR)/**/*.c) $(wildcard $(SRC_DIR)/*.c) \
	$(wildcard $(LIB_DIR)/**/*.c) $(wildcard $(LIB_DIR)/*.c)

LDFLAGS := $(wildcard $(LIB_DIR)/**/*.a) $(wildcard $(LIB_DIR)/*.a)

LIB_SUBDIRS := $(dir $(wildcard $(LIB_DIR)/**/.)) $(dir $(wildcard $(LIB_DIR)/./))
INCLUDES := -I$(SRC_DIR) -I$(LIB_DIR) $(addprefix -I,$(LIB_SUBDIRS))

.PHONY: all fmt build launch clean

all: fmt build launch

build:
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) $(SRCS) $(LDFLAGS) $(macOS_FRAMEWORKS) -o $(TARGET)

launch:
	./$(TARGET)

fmt:

clean:
	rm -rf $(BUILD_DIR)