#.PHONY: all clean verify

PROJECT = testide
USE_F8087 = -f87

TCPP_DIR = P:\DOS\APPS\PROG\C_CPP\TCPP
TASM_DIR = P:\DOS\APPS\PROG\ASM\TASM

# Variables *_DIR set in C:USERHOOK.BAT
CC = $(TCPP_DIR)\BIN\TCC
LK = $(TCPP_DIR)\BIN\TLINK
ASM = $(TASM_DIR)\TASM.EXE

#-S -B -E$(ASM) 
#OPTIONS = -ms -r -M -w -S -I$(TCPP_DIR)\INCLUDE;..\INC
LK_OPTIONS = /c /x /L$(TCPP_DIR)\LIB;..\LIB
AS_OPTIONS = /l /m2

# Find all .cpp source files
SRCS = $(wildcard *.cpp)
# Define object files based on .cpp files
#OBJS = $(SRCS:.cpp=.obj)
OBJS = $(PROJECT).obj

CC = $(TCPP_DIR)\BIN\TCC

all: $(PROJECT).exe

$(PROJECT).exe: $(PROJECT).cpp IDE.cpp
	$(CC) @tcc_args.cfg $(PROJECT).cpp IDE.cpp

# Target to clean up generated files
clean:
	@del *.obj
	@del *.exe

load:
	cp $(PROJECT).exe /media/jpellet/NANO8088/utils

