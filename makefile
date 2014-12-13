LIBFLAGS = `pkg-config --libs gtk+-3.0`
CFLAG = `pkg-config --cflags gtk+-3.0`

INC_DIR = include
SRC_DIR = src

CC = g++
CFLAGS = --std=c++11

#sources for common
SOURCES_COM = TcpHandler.cpp TcpListener.cpp
HEADERS_COM = TcpHandler.h TcpListener.h
OBJECTS_COM = $(SOURCES_COM:.cpp=.o)
FSOURCES_COM := $(addprefix $(SRC_DIR)/common/,$(SOURCES_COM))
FHEADERS_COM := $(addprefix $(INC_DIR)/common/,$(HEADERS_FILE))

LDFLAGS_COM = -lboost_system -lboost_thread -lpthread

client:  tcplistener.o tcphandler.o
	$(CC) main.cpp -o $@ $(CFLAGS)

tcphandler.o : $(FHEADERS_COM)
	$(CC) -c $(CFLAGS) $(SRC_DIR)/common/TcpHandler.cpp -g -o $@ -I$(INC_DIR) $(LDFLAGS_COM) $(CFLAG)

tcplistener.o : $(FHEADERS_COM)
	$(CC) -c $(CFLAGS) $(SRC_DIR)/common/TcpListener.cpp -g -o $@ -I$(INC_DIR) $(LDFLAGS_COM) $(CFLAG)
