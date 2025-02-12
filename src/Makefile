TARGET=xxd
CC=gcc

OBJ = main.o hex.o file.o
FILES = main.c hex.c file.c

$(TARGET): $(OBJ)
	mkdir -p ../build
	$(CC) -g -o $(TARGET) $(OBJ)
	mv $(TARGET) ../build/

debug:
	gcc -g $(FILES)
	mv a.out ../build/

main.o: include/hex.h include/file.h
hex.o: include/hex.h
file.o: include/file.h

.PHONY: clean
clean:
	rm -rf ../build $(OBJ)
