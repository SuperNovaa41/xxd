#include <stdio.h>
#include <stdlib.h>

#include "include/file.h"

void read_file_to_buf(const char* filename, char** buf)
{
	FILE* f;
	int err, bufsize, newlen;

	f = fopen(filename, "r");
	if (f == NULL) {
		perror("fopen");
		exit(EXIT_FAILURE);
	}

	err = fseek(f, 0L, SEEK_END);
	if (err != 0) {
		perror("fseek");
		fclose(f);
		exit(EXIT_FAILURE); }

	bufsize = ftell(f);
	if (bufsize == -1) {
		perror("ftell");
		fclose(f);
		exit(EXIT_FAILURE);
	}

	(*buf) = malloc(sizeof(char) * (bufsize + 1));
	if (*buf == NULL) {
		perror("malloc");
		fclose(f);
		exit(EXIT_FAILURE);
	}

	rewind(f); 

	newlen = fread(*buf, sizeof(char), bufsize, f);
	if (ferror(f) != 0) {
		perror("fread");
		fclose(f);
		exit(EXIT_FAILURE);
	} else {
		(*buf)[newlen++] = '\0'; // just to be safe	
	}

	fclose(f);
}
