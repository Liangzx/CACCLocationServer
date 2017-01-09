CC = g++
CPP_VERSION=-std=c++11
CPPLAG=${CPP_VERSION}
DLIB=-lboost_system -lboost_filesystem -lrabbitmq -llog4cplus 

.SUFFIXES:.cpp .c

INC= -I.

PROJECT_HOME=/data/cxx/bin/CACCLocationServer/src
CACCLocationServer_OBJ=$(PROJECT_HOME)/CACCLocationServer.o	\
					  $(PROJECT_HOME)/cacc_location_session_nmea.o	\
					  $(PROJECT_HOME)/cacc_location_session_app.o	\
					  $(PROJECT_HOME)/cacc_location_session.o	\
					  $(PROJECT_HOME)/cacc_location_server.o	\
					  $(PROJECT_HOME)/cacc_location_config.o	\
					  $(PROJECT_HOME)/AMQP.o	\
					  $(PROJECT_HOME)/AMQPBase.o	\
					  $(PROJECT_HOME)/AMQPException.o	\
					  $(PROJECT_HOME)/AMQPExchange.o	\
					  $(PROJECT_HOME)/AMQPMessage.o	\
					  $(PROJECT_HOME)/AMQPQueue.o

all:CACCLocationServer

.c.o:
	$(CC) $(CPPLAG) $(DLIB) -c -w $< -o $@

.cpp.o:
	$(CC) $(CPPLAG) $(DLIB)  -c -w $< -o $@

CACCLocationServer:${CACCLocationServer_OBJ}
	${CC} $(CPPLAG) ${INC} -o $@ ${CACCLocationServer_OBJ} $(DLIB)

clean:
	rm -f ${CACCLocationServer_OBJ}
	rm -f CACCLocationServer