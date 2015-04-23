CC=g++
CFLAGS=-c -std=c++11 -g -Wall
LDFLAGS=
LIBS=-lboost_system -lboost_thread
MAC_LIBS=-lboost_system -lboost_thread-mt
CLIENT_SOURCES=ClientMain.cpp
SERVER_SOURCES=Message.cpp Session.cpp SessionManager.cpp Server.cpp ServerMain.cpp
CLIENT_OBJECTS=$(CLIENT_SOURCES:.cpp=.o)
SERVER_OBJECTS=$(SERVER_SOURCES:.cpp=.o)
CLIENT_EXEC=client
SERVER_EXEC=server
CLIENT_MAC_EXEC=client
SERVER_MAC_EXEC=server
VERSION=1.0.0

all: servermk clientmk

mac: server-mac client-mac

clientmk: $(CLIENT_SOURCES) $(CLIENT_EXEC)

servermk: $(SERVER_SOURCES) $(SERVER_EXEC)

client-mac: $(CLIENT_SOURCES) $(CLIENT_MAC_EXEC)

server-mac: $(SERVER_SOURCES) $(SERVER_MAC_EXEC)

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

$(CLIENT_EXEC): $(CLIENT_OBJECTS)
	$(CC) $(LDFLAGS) $(CLIENT_OBJECTS) -o $@ $(LIBS)

$(SERVER_EXEC): $(SERVER_OBJECTS)
	$(CC) $(LDFLAGS) $(SERVER_OBJECTS) -o $@ $(LIBS)

$(CLIENT_MAC_EXEC): $(CLIENT_OBJECTS)
	$(CC) $(LDFLAGS) $(CLIENT_OBJECTS) -o $@ $(MAC_LIBS)

$(SERVER_MAC_EXEC): $(SERVER_OBJECTS)
	$(CC) $(LDFLAGS) $(SERVER_OBJECTS) -o $@ $(MAC_LIBS)

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@
