#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#include "include/hex.h"

void free_hex_chunk(hex_chunk_t* chunk)
{
	free(chunk->text);
	free(chunk->hex);
}

void add_text_to_chunk(char* src, char** dst)
{
	*dst = malloc(sizeof(char) * (TEXT_LINE_LEN + 1));

	strncpy(*dst, src, 16);
	(*dst)[TEXT_LINE_LEN] = '\0';
}

void convert_text_to_hex(hex_chunk_t* chunk)
{
	int i, j;
	chunk->hex = malloc(sizeof(char) * (HEX_LINE_LEN + 1));

	for (i = 0, j = 0; i < HEX_LINE_LEN; i += 2, j += 1) {
		if (chunk->text[j] == '\0')
			snprintf(chunk->hex + i, 3, "  ");
		else
			snprintf(chunk->hex + i, 3, "%02x", chunk->text[j]);
	}
}

// TODO: clean up the text formatting to not print it every time
void display_hex_chunk(hex_chunk_t* chunk)
{
	int i, j;

	printf("%08x: ", chunk->line * 16);
	for (i = 0; i < HEX_LINE_LEN; i += 4) {
		for (j = 0; j < 4; j += 2) {
			if (((chunk->hex + i) + j)[0] == '0' && ((chunk->hex + i) + j)[1] == 'a')
				printf("\x1b[33m");
			else
				printf("\x1b[32m");
			printf("%2.2s", chunk->hex + i + j);
		}
		printf(" ");
	}

	for (i = 0; i < TEXT_LINE_LEN; i++) {
		if (chunk->text[i] == '\n' || chunk->text[i] == EOF)
			printf("\x1b[33m.");
		else
			printf("\x1b[32m%c", chunk->text[i]);
	}
	puts("\x1b[0m");
}
