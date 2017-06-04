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
	dev.cpp\
	fifo.cpp\
	remote.cpp\
	transport.cpp\
	vocabulary.cpp

SRC_SERVER= \
	$(SRC_COMMON)\
	server/fifo_server.cpp\
	server/serial.cpp\
	server/io.cpp\
	server/discrete.cpp\
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

SRC_EMULATOR= \
	emulator/emulator.cpp\
	emulator/test.cpp

SRC_TEST= \
	features.cpp

OBJ_SERVER = $(SRC_SERVER:%.cpp=../obj/server/%.o)
OBJ_CLIENT = $(SRC_CLIENT:%.cpp=../obj/client/%.o)
OBJ_EMULATOR = $(SRC_EMULATOR:%.cpp=../obj/emulator/%.o)
OBJ_TEST = $(SRC_TEST:%.cpp=../obj/test/%.o)
CXX = g++

INCLUDES = 	-I $(SRCDIR)/include \
			-I $(SRCDIR)/client/include \
			-I $(SRCDIR)/server/include

CXXFLAGS = -Wall -Wextra ${INCLUDES} `pkg-config gtk+-2.0 --cflags`

CXXFLAGS_SERVER = -Wall -Wextra ${INCLUDES} 

LDFLAGS = -lm -lrt -lpthread

LDLIBS = `pkg-config gtk+-2.0 --libs`

../obj/client/%.o: %.cpp
	$(CXX) -DCLIENT $(PRODUCT_FLAGS) $(CXXFLAGS) -c -o $@ $<

../obj/server/%.o: %.cpp
	$(CXX) -DSERVER $(PRODUCT_FLAGS) $(CXXFLAGS_SERVER) -c -o $@ $<

../obj/%.o: %.cpp
	$(CXX) -DCLIENT $(PRODUCT_FLAGS) -Wall -Wextra $(INCLUDES) -c -o $@ $<

../obj/emulator/%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

../obj/test/%.o: %.cpp
	$(CXX) $(CXXFLAGS_SERVER) -c -o $@ $<

debug: client server debug_footer

rem: PRODUCT_FLAGS := -DREMOTE
rem: clean_remote client_remote rem_footer


emu: emul

feat: test


client: $(OBJ_CLIENT)
	$(CXX) -o $(BINDIR)$@ $(OBJ_CLIENT) $(LDLIBS) $(LDFLAGS)

client_remote: $(OBJ_CLIENT)
	$(CXX) -o $(BINDIR)$@ $(OBJ_CLIENT) $(LDLIBS) $(LDFLAGS)

server: $(OBJ_SERVER)
	$(CXX) -o $(BINDIR)$@ $(OBJ_SERVER) $(LDFLAGS)

emul: $(OBJ_EMULATOR)
	$(CXX) -o $(BINDIR)$@ $(OBJ_EMULATOR) $(LDLIBS) $(LDFLAGS)

test: $(OBJ_TEST)
	$(CXX) -o $(BINDIR)$@ $(OBJ_TEST) 


debug_footer:
	@echo "==========================================================================="; \
	echo "$(TASK): server client\n"

rem_footer:
	@echo "==========================================================================="; \
	echo "$(TASK): client_remote\n"

emu_footer:
	@echo "==========================================================================="; \
	echo "$(TASK): emulator\n"

clean_remote:
	/bin/rm -f $(OBJ_CLIENT) $(BINDIR)client_eth

clean:
	/bin/rm -f $(OBJ_CLIENT) $(OBJ_SERVER) $(OBJ_EMULATOR) $(OBJ_TEST) $(BINDIR)client $(BINDIR)server $(BINDIR)emulator $(BINDIR)test *~

