### Makefile to generate example `.ll` files for the newton LLVM IR passes. ###

ifndef VERBOSE
	QUIET:=@
endif

LLVM_CONFIG:=llvm-config
CC:=clang++ # Currently tested with clang-12/clang-13

vpath %.cpp adpcm dfadd dfdiv dfmul soft_float_api dfsin gsm motion

BIN?=application
COMMON_FLAGS=-Wall -Wextra

all: default

default: float64_add.ll float64_div.ll float64_mul.ll float64_sin.ll soft_float_api.ll

%.ll : %.cpp
	@echo Compiling $*.c
	$(QUIET)$(CC) -g -O0 -Xclang -disable-O0-optnone -S -emit-llvm $(COMMON_FLAGS) -o $@ $<
	opt $@ --mem2reg -S -o $@

clean::
	$(QUIET)rm -f *.ll *.bc
