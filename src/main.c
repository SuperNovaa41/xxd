#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "include/hex.h"

/**
 * read_file_to_buf
 *
 * - const char* filename - Name of the file we're opening
 * - char** buf - pointer to the buffer to hold the file in
 *
 *   read the file into a buffer to prevent unnecessary file ops
 */
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

int get_hex_lines(int len)
{
	int out;

	out = len / 16;
	if (len % 16 != 0)
		out++;

	return out;
}

int main(int argc, char* argv[])
{
	char* file_content;
	int hex_lines, i;

	if (argc != 2) {
		fprintf(stderr, "Usage: %s [filename]\n", argv[0]);
		return EXIT_FAILURE;
	}

	read_file_to_buf(argv[1], &file_content);
	hex_lines = get_hex_lines(strlen(file_content));

	hex_chunk_t* lines = malloc(sizeof(hex_chunk_t) * hex_lines);

	for (i = 0; i < hex_lines; i++) {
		lines[i].line = i;
	
		add_text_to_chunk(file_content + (i * 16), &(lines[i].text));
		convert_text_to_hex(&lines[i]);
	}

	for (i = 0; i < hex_lines; i++)
		display_hex_chunk(&(lines[i]));


		
	free(file_content);	
	for (i = 0; i < hex_lines; i++)
		free_hex_chunk(&(lines[i]));
	free(lines);


	return 0;
}
