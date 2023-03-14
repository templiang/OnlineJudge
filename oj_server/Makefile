CC=g++ 
STD=-std=c++11
LIB=-lpthread -lctemplate -ljsoncpp -lmysqlclient -lmysqlclient 
LIBPATH=-L./mysql_lib
SERVER=OJServer

all:$(SERVER)
$(SERVER):$(SERVER).cc
	$(CC) -o $@ $^ $(STD) $(LIB) $(LIBPATH)

.PHONY:clean
clean:
	rm -f $(SERVER)
