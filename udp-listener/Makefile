BIN_DIR ?= build
CMAKE_GENERATOR ?= Ninja
EXE := udp_listener

.PHONY: build run

all: build run

build:
	mkdir -p $(BIN_DIR)
	cmake . -G $(CMAKE_GENERATOR) -B $(BIN_DIR)
	cmake --build $(BIN_DIR)
	cp $(BIN_DIR)/compile_commands.json ./

run:
	./$(BIN_DIR)/$(EXE)
