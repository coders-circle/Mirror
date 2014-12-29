INC_DIR := include
CPP_FILES := $(wildcard src/client/*.cpp) $(wildcard src/common/*.cpp) $(wildcard src/client/UI/*.cpp)
OBJ_FILES := $(addprefix obj/,$(CPP_FILES:src/%.cpp=%.o))
CC_FLAGS := -I $(INC_DIR) -MMD --std=c++11 `pkg-config --cflags gtk+-3.0 opencv`
LD_FLAGS := --std=c++11 `pkg-config --libs gtk+-3.0 opencv` -lboost_system -lboost_thread -lpthread

bin/client: $(OBJ_FILES)
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

-include $(OBJFILES:.o=.d)
