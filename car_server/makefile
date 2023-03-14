CC=g++ -o $@ $^
STD=-std=c++11
LIB=-ljsoncpp -lpthread
COMPILER=CARServer

all:$(COMPILER)
$(COMPILER):$(COMPILER).cc
	$(CC) $(STD) $(LIB)

.PHONY:clean
clean:
	rm -f $(COMPILER)

