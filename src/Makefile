TARGET=xxd
CC=gcc

OBJ = main.o hex.o

$(TARGET): $(OBJ)
	mkdir -p ../build
	$(CC) -Wall -g -o $(TARGET) $(OBJ) -g
	mv $(TARGET) ../build/

main.o: include/hex.h
hex.o: include/hex.h

.PHONY: clean
clean:
	rm -rf ../build $(OBJ)
