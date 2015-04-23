CC=g++
CFLAGS=-c -std=c++11 -g -Wall
LDFLAGS=
ifeq ($(OS), Windows_NT)
	LIBS=-lboost_system -lboost_thread
else
	LIBS=-lboost_system -lboost_thread-mt
endif
CLIENT_SOURCES=Message.cpp Client.cpp ClientMain.cpp
SERVER_SOURCES=Message.cpp Session.cpp SessionManager.cpp Server.cpp ServerMain.cpp
CLIENT_OBJECTS=$(CLIENT_SOURCES:.cpp=.o)
SERVER_OBJECTS=$(SERVER_SOURCES:.cpp=.o)
CLIENT_EXEC=client
SERVER_EXEC=server
VERSION=1.0.0

all: servermk clientmk

clientmk: $(CLIENT_SOURCES) $(CLIENT_EXEC)

servermk: $(SERVER_SOURCES) $(SERVER_EXEC)

clean: 
	-rm $(CLIENT_OBJECTS)
	-rm $(SERVER_OBJECTS)

cleanf: clean
	-rm $(CLIENT_EXEC)
	-rm $(SERVER_EXEC)

re: cleanf all

re-mac: cleanf mac

version:
	@echo $(VERSION)

dep:
	@echo $(LIBS)

$(CLIENT_EXEC): $(CLIENT_OBJECTS)
	$(CC) $(LDFLAGS) $(CLIENT_OBJECTS) -o $@ $(LIBS)
	@echo $$LD_LIBRARY_PATH

$(SERVER_EXEC): $(SERVER_OBJECTS)
	$(CC) $(LDFLAGS) $(SERVER_OBJECTS) -o $@ $(LIBS)

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@
