.DEFAULT_GOAL := debug
TASK = "build completed"

SRCDIR = .
BINDIR = ../bin/
OBJDIR = ../obj/
PRODUCT_FLAGS =

SRC_COMMON = \
	logger.cpp\
	timespec.cpp\
	cloop.cpp\
	device.cpp\
	fifo.cpp\
	remote.cpp\
	vocabulary.cpp

SRC_SERVER= \
	$(SRC_COMMON)\
	server/fifo_server.cpp\
	server/serial.cpp\
	server/io.cpp\
	server/discrete_device_server.cpp\
	server/analog_device_server.cpp\
	server/serial_device_server.cpp\
	server/tools_srv.cpp\
	server/main_server.cpp

SRC_CLIENT= \
	$(SRC_COMMON)\
	client/fifo_client.cpp\
	client/window.cpp\
	client/tools_cli.cpp\
	client/journal.cpp\
	client/discrete_device_client.cpp\
	client/analog_device_client.cpp\
	client/serial_device_client.cpp\
	client/main_client.cpp

OBJ_SERVER = $(SRC_SERVER:%.cpp=../obj/server/%.o)
OBJ_CLIENT = $(SRC_CLIENT:%.cpp=../obj/client/%.o)

CXX = g++

INCLUDES = 	-I $(SRCDIR)/include \
			-I $(SRCDIR)/client/include \
			-I $(SRCDIR)/server/include

CXXFLAGS = -Wall -Wextra ${INCLUDES} `pkg-config gtk+-2.0 --cflags`

LDFLAGS = -lm -lrt -lpthread

LDLIBS = `pkg-config gtk+-2.0 --libs`

../obj/client/%.o: %.cpp
	$(CXX) -DCLIENT $(PRODUCT_FLAGS) $(CXXFLAGS) -c -o $@ $<

../obj/server/%.o: %.cpp
	$(CXX) -DSERVER $(PRODUCT_FLAGS) $(CXXFLAGS) -c -o $@ $<

../obj/%.o: %.cpp
	$(CXX) -DCLIENT $(PRODUCT_FLAGS) -Wall -Wextra $(INCLUDES) -c -o $@ $<

debug: client server loc

rem: PRODUCT_FLAGS := -DREMOTE
rem: clean_remote client_remote rmt

client: $(OBJ_CLIENT)
	$(CXX) -o $(BINDIR)$@ $(OBJ_CLIENT) $(LDLIBS) $(LDFLAGS)

client_remote: $(OBJ_CLIENT)
	$(CXX) -o $(BINDIR)$@ $(OBJ_CLIENT) $(LDLIBS) $(LDFLAGS)

server: $(OBJ_SERVER)
	$(CXX) -o $(BINDIR)$@ $(OBJ_SERVER) $(LDLIBS) $(LDFLAGS)

loc:
	@echo "==========================================================================="; \
	echo "$(TASK): server client\n"

rmt:
	@echo "==========================================================================="; \
	echo "$(TASK): client_remote\n"

clean_remote:
	/bin/rm -f $(OBJ_CLIENT) $(BINDIR)client_eth

clean:
	/bin/rm -f $(OBJ_CLIENT) $(OBJ_SERVER) $(OBJ_STREAMER) $(BINDIR)client $(BINDIR)server $(BINDIR)streamer  *~

	
