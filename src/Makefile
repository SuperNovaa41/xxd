TARGET=xxd
CC=gcc

OBJ = main.o hex.o file.o

$(TARGET): $(OBJ)
	mkdir -p ../build
	$(CC) -Wall -g -o $(TARGET) $(OBJ) -g
	mv $(TARGET) ../build/

main.o: include/hex.h include/file.h
hex.o: include/hex.h
file.o: include/file.h

.PHONY: clean
clean:
	rm -rf ../build $(OBJ)
