## Directories to search include and source files
INC_DIR := include
COMMON_SRC_DIRS := common
CLIENT_SRC_DIRS := client client/UI client/MediaStream
SERVER_SRC_DIRS := server

## Directories to build files into
OBJ_DIR := obj
BIN_DIR := bin

## List of all c++ files to compile
COMMON_CPP_FILES := $(filter-out src/common/JSONHandler.cpp, $(wildcard $(COMMON_SRC_DIRS:%=src/%/*.cpp)))
CLIENT_CPP_FILES := $(wildcard $(CLIENT_SRC_DIRS:%=src/%/*.cpp)) $(COMMON_CPP_FILES)
SERVER_CPP_FILES := $(wildcard $(SERVER_SRC_DIRS:%=src/%/*.cpp)) $(COMMON_CPP_FILES)

## List of all object files to generate
CLIENT_OBJ_FILES := $(addprefix $(OBJ_DIR)/,$(CLIENT_CPP_FILES:src/%.cpp=%.o))
SERVER_OBJ_FILES := $(addprefix $(OBJ_DIR)/,$(SERVER_CPP_FILES:src/%.cpp=%.o))

## Compiler, compiler and linker flags and libaries to use
CXX := g++
CXXLIBS := `pkg-config --cflags gtk+-3.0`
LDLIBS := `pkg-config --libs gtk+-3.0` -lboost_system -lboost_thread -lpthread -lboost_timer -lavformat -lavdevice -lavcodec -lswscale -lavutil
CXXFLAGS := -I $(INC_DIR) -MMD --std=c++11 $(CXXLIBS) -Wno-deprecated-declarations
LDFLAGS := --std=c++11 $(LDLIBS)

## Build client and server applications
all: bin/client bin/server

$(BIN_DIR)/client: $(CLIENT_OBJ_FILES) | $(BIN_DIR)
	$(CXX) -o $@ $^ $(LDFLAGS)
$(BIN_DIR)/server: $(SERVER_OBJ_FILES) | $(BIN_DIR)
	$(CXX) -o $@ $^ $(LDFLAGS)

$(OBJ_DIR)/%.o: src/%.cpp | $(OBJ_DIR)
	$(CXX) -c -o $@ $< $(CXXFLAGS) 

$(BIN_DIR):
	mkdir $(BIN_DIR)

$(OBJ_DIR):
	mkdir $(OBJ_DIR) $(CLIENT_SRC_DIRS:%=$(OBJ_DIR)/%) $(COMMON_SRC_DIRS:%=$(OBJ_DIR)/%) $(SERVER_SRC_DIRS:%=$(OBJ_DIR)/%)
	
## Clean up everything and make necessary directories
clean:
	rm -rf obj
	rm -rf bin

## Include auto-generated dependencies rules
OBJFILES := $(CLIENT_OBJ_FILES) $(SERVER_OBJ_FILES)
-include $(OBJFILES:.o=.d)
