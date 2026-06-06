SHELL := /bin/zsh

CC := gcc
CFLAGS := -std=c23 -Wall -Wextra -Wpedantic -O3

SRC_DIR := src
LIB_DIR := lib
BUILD_DIR := build
TARGET := $(BUILD_DIR)/game
HEADER_DIR := $(BUILD_DIR)/auto-headers
PREPROC_DIR := $(BUILD_DIR)/src

macOS_FRAMEWORKS := -framework Cocoa -framework OpenGL -framework IOKit -framework CoreVideo

SRCS := $(wildcard $(PREPROC_DIR)/**/*.c) $(wildcard $(PREPROC_DIR)/*.c) \
	$(wildcard $(LIB_DIR)/**/*.c) $(wildcard $(LIB_DIR)/*.c)

LDFLAGS := $(wildcard $(LIB_DIR)/**/*.a) $(wildcard $(LIB_DIR)/*.a) -lc++ -lz

LIB_SUBDIRS := $(dir $(wildcard $(LIB_DIR)/**/.)) $(dir $(wildcard $(LIB_DIR)/./))
INCLUDES := -I$(HEADER_DIR) -I$(LIB_DIR) $(addprefix -I,$(LIB_SUBDIRS))

C_FILES := $(wildcard src/*.c)
MHDR_ARGS = $(foreach src,$(C_FILES),$(src):$(HEADER_DIR)/$(notdir $(basename $(src))).h)

.PHONY: all fmt build launch clean

all: fmt build launch

build:
	rm -rf $(HEADER_DIR)
	mkdir -p $(BUILD_DIR) $(HEADER_DIR) $(PREPROC_DIR)
	$(CC) -O3 preprocess.c -o $(BUILD_DIR)/preprocess
	for file in $(SRC_DIR)/*.c; do \
		filename=$$(basename $$file); \
		$(BUILD_DIR)/preprocess "$$filename" "$$file" "$(PREPROC_DIR)/$$filename" || exit 1; \
	done
	@for file in $(PREPROC_DIR)/*.c; do \
		filename=$$(basename $$file .c); \
		./makeheaders $$file:$(HEADER_DIR)/$$filename.h; \
	done
	find $(HEADER_DIR) -name "*.h" -exec sed -i '' '1s/^/#pragma once\n\n/' {} +
	$(CC) $(CFLAGS) $(INCLUDES) $(SRCS) $(LDFLAGS) $(macOS_FRAMEWORKS) -o $(TARGET)

launch:
	./$(TARGET)

fmt:

clean:
	rm -rf $(BUILD_DIR)