# Compiler
CC = gcc

# Binary
TARGET = synth

# Directories
SRC_DIR = src
INC_DIR = include
BIN_DIR = bin
OBJ_DIR = obj

# Files
CORE_SRCS = $(wildcard $(SRC_DIR)/core/*.c)
STANDALONE_SRCS = $(wildcard $(SRC_DIR)/standalone/*.c)
CLAP_SRCS = $(wildcard $(SRC_DIR)/clap/*.c) $(wildcard $(SRC_DIR)/clap/gui/*.c)
SRCS = 

# OS Detection
ifeq ($(OS),Windows_NT)
	DETECTED_OS = Windows
else
	DETECTED_OS = $(shell uname -s)
endif

# Common flags 
CFLAGS = -Wall -Wextra -O2 -I$(INC_DIR) -MMD -MP
LDFLAGS =

# OS specific flags
ifeq ($(DETECTED_OS),Windows)
	RM = powershell -NoProfile -Command Remove-Item -Recurse -Force -ErrorAction SilentlyContinue
	MKDIR = powershell -NoProfile -Command "New-Item -ItemType Directory -Force -Path '$(1)' | Out-Null"
else
	RM = rm -rf
	MKDIR = mkdir -p $(1)
endif

COMPILE_MODE=STANDALONE
ifeq ($(COMPILE_MODE),CLAP)
	TARGET := $(TARGET).clap
	CFLAGS += -D__CLAP__ -fPIC -Iexternal/
	LDFLAGS += -shared
	SRCS = $(CORE_SRCS) $(CLAP_SRCS)

	# Libs for plugin GUI 
	ifeq ($(DETECTED_OS),Windows)
		LDFLAGS += -lgdi32 -luser32
	else ifeq ($(DETECTED_OS),Linux)
		LDFLAGS += -lX11
	else ifeq ($(DETECTED_OS),Darwin)
		LDFLAGS += -framework Cocoa
	endif
else ifeq ($(DETECTED_OS),Windows)
	TARGET := $(TARGET).exe
	CFLAGS += -D_WIN32_WINNT=0x0601 -D__WINDOWS__ -Iexternal/ -Iexternal/libxml/include/
	LDFLAGS += -Lexternal/raylib/src/ -lraylib -Lexternal/libxml/lib/ -lxml2 -lm -lksuser -lwinmm -lgdi32 -lopengl32 -lole32 -luuid -lshell32 -lws2_32
	SRCS = $(CORE_SRCS) $(STANDALONE_SRCS)
else ifeq ($(DETECTED_OS),Linux)
	CFLAGS += -I/usr/include/libxml2 -D__LINUX__
	LDFLAGS += -lasound -lm -lraylib -lxml2 -lX11 -lpthread
	SRCS = $(CORE_SRCS) $(STANDALONE_SRCS)
else ifeq ($(DETECTED_OS),Darwin)
	CFLAGS += -I/usr/local/include/libxml2 -I/opt/homebrew/include/libxml2 -D__MACOS__
	LDFLAGS += -lm -lraylib -lxml2
	SRCS = $(CORE_SRCS) $(STANDALONE_SRCS)
endif


OBJS = $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
DEPS = $(OBJS:.o=.d)


# Default
all: $(BIN_DIR)/$(TARGET)

# Link
$(BIN_DIR)/$(TARGET): $(OBJS) | $(BIN_DIR)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

# Compile
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	-@$(call MKDIR,$(dir $@))
	$(CC) $(CFLAGS) -c $< -o $@

# Create directories if needed
$(BIN_DIR):
	$(call MKDIR,$(BIN_DIR))

$(OBJ_DIR):
	$(call MKDIR,$(OBJ_DIR))

# Clean
clean:
	$(RM) $(BIN_DIR)
	$(RM) $(OBJ_DIR)

# Rebuild
re: clean all

run:
	./$(BIN_DIR)/$(TARGET)

-include $(DEPS)
.PHONY: all clean re run