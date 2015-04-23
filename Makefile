CC=g++
CFLAGS=-c -std=c++11 -g -Wall
LDFLAGS=
LIBS=-lboost_system -lboost_thread
CLIENT_SOURCES=ClientMain.cpp
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

version:
	@echo $(VERSION)

$(CLIENT_EXEC): $(CLIENT_OBJECTS)
	$(CC) $(LDFLAGS) $(CLIENT_OBJECTS) -o $@ $(LIBS)

$(SERVER_EXEC): $(SERVER_OBJECTS)
	$(CC) $(LDFLAGS) $(SERVER_OBJECTS) -o $@ $(LIBS)

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@
