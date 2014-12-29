INC_DIR := include

COMMON_CPP_FILES := $(wildcard src/common/*.cpp) 
CLIENT_CPP_FILES := $(wildcard src/client/*.cpp) $(wildcard src/client/UI/*.cpp) $(COMMON_CPP_FILES)
CLIENT_OBJ_FILES := $(addprefix obj/,$(CLIENT_CPP_FILES:src/%.cpp=%.o))

SERVER_CPP_FILES := $(wildcard src/server/*.cpp) $(COMMON_CPP_FILES)
SERVER_OBJ_FILES := $(addprefix obj/,$(SERVER_CPP_FILES:src/%.cpp=%.o))

CC_FLAGS := -I $(INC_DIR) -MMD --std=c++11 `pkg-config --cflags gtk+-3.0 opencv`
LD_FLAGS := --std=c++11 `pkg-config --libs gtk+-3.0 opencv` -lboost_system -lboost_thread -lpthread

all: bin/client bin/server

bin/client: $(CLIENT_OBJ_FILES)
	g++ -o $@ $^ $(LD_FLAGS)
bin/server: $(SERVER_OBJ_FILES)
	g++ -o $@ $^ $(LD_FLAGS)

obj/%.o: src/%.cpp
	g++ -c -o $@ $< $(CC_FLAGS) 

clean:
	rm -rf obj
	rm -rf bin
	mkdir obj
	mkdir bin
	mkdir obj/client
	mkdir obj/client/UI
	mkdir obj/common
	mkdir obj/server

-include $(OBJFILES:.o=.d)
