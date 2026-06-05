CC := "gcc"
CFLAGS := "-std=c23 -Wall -Wextra -Wpedantic -O3"

SRC_DIR := "src"
LIB_DIR := "lib"
BUILD_DIR := "build"
TARGET := BUILD_DIR + "/game"

macOS_FRAMEWORKS := "-framework Cocoa -framework OpenGL -framework IOKit -framework CoreVideo"

all: fmt build launch

build:
	#!/usr/bin/env fish
	mkdir -p {{BUILD_DIR}}
	set SRCS {{SRC_DIR}}/**/*.c {{LIB_DIR}}/**/*.c
	set INCLUDES "-I{{LIB_DIR}}"
	for dir in {{LIB_DIR}}/**/
		if test -d $dir
			set INCLUDES $INCLUDES "-I$dir"
		end
	end
	set LDFLAGS {{LIB_DIR}}/**/*.a
	{{CC}} {{CFLAGS}} $INCLUDES $SRCS $LDFLAGS {{macOS_FRAMEWORKS}} -o {{TARGET}}

launch:
	./{{TARGET}}

fmt:
	#!/usr/bin/env fish
	astyle --options=astylerc -n -r "src/*.c"

clean:
	rm -rf {{BUILD_DIR}}