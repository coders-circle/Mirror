INC_DIR = include
SRC_DIR = src

CFLAG = `pkg-config --cflags gtk+-3.0`
CFLAGS = --std=c++11

CC = g++

#for common
SOURCES_COMM = TcpHandler.cpp HttpHandler.cpp
HEADERS_COMM = TcpHandler.h HttpHandler.h
OBJECTS_COMM = $(SOURCES_COMM:.cpp=.o)
FSOURCES_COMM := $(addprefix $(SRC_DIR)/common/, $(SOURCES_COMM))
FHEADERS_COMM := $(addprefix $(INC_DIR)/common/, $(HEADERS_COMM))

LDFLAGS_COMM = -lboost_system -lboost_thread -lpthread

all: common
	$(CC) -o output $(SRC_DIR)/common/abc.cpp *.o -Iinclude/ $(CFLAGS) $(CFLAG) $(LDFLAGS_COMM)

common: $(OBJECTS_COMM)
	$(CC) -c -o common.o $<

%.o :  src/common/%.cpp 
	$(CC) -c -o $@ $< $(LDFLAGS_COMM) -Iinclude/ $(CFLAG) $(CFLAGS)

client.o: src/client/TcpClient.cpp
	$(CC)  -c -o $@ $< -Iinclude/ $(CFLAG) $(CFLAGS)
	
clean:
	rm *.o
