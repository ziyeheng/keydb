CC = g++
FLAGS = -g
INC = -I./libevent-1.4.14-stable -I./jsoncpp/include 
EXE = keydb
JSONLIB = libjsoncpp.a
LIBEVETNLIB = ./libevent-1.4.14-stable/.libs/libevent.a
CPPSOURCES=$(wildcard *.cpp)
CPPOBJS=$(patsubst %.cpp, %.o,$(CPPSOURCES))

SOURCES = $(CSOURCES) 
OBJS = $(CPPOBJS)

LIBS = ./libevent-1.4.14-stable/.libs/libevent.a ./libjsoncpp.a -lpthread -lrt

all: $(EXE)

%.o:%.c
	$(CC) -c $(FLAGS) $(INC) -o $@ $<
%.o:%.cpp
	$(CC) -c $(FLAGS) $(INC) -o $@ $<

$(JSONLIB): 
	$(CC) -c $(FLAGS)  -I./jsoncpp/include/ -o jsoncpp/src/lib_json/json_reader.o jsoncpp/src/lib_json/json_reader.cpp
	$(CC) -c $(FLAGS)  -I./jsoncpp/include/ -o jsoncpp/src/lib_json/json_value.o jsoncpp/src/lib_json/json_value.cpp
	$(CC) -c $(FLAGS)  -I./jsoncpp/include/ -o jsoncpp/src/lib_json/json_writer.o jsoncpp/src/lib_json/json_writer.cpp
	ar crv $(JSONLIB)  jsoncpp/src/lib_json/json_reader.o  jsoncpp/src/lib_json/json_value.o  jsoncpp/src/lib_json/json_writer.o

$(LIBEVETNLIB): 
	tar xvzf libevent-1.4.14-stable.tar.gz
	cd `pwd`/libevent-1.4.14-stable && ./configure
	cd `pwd`/libevent-1.4.14-stable && make
	cd `pwd`

$(EXE): $(JSONLIB) $(LIBEVETNLIB) $(OBJS) 
	$(CC) $(FLAGS) $(INC) -o $(EXE) $(OBJS) $(LIBS)

clean:
	rm -f $(OBJS) jsoncpp/src/lib_json/*.o $(JSONLIB) $(EXE) core.*
