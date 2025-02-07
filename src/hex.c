#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#include "include/hex.h"

extern struct flags flags;

void init_flags(struct flags* flags)
{
	flags->autoskip = false;
	flags->binary = false;
	flags->cols = 4;
	flags->octets = 2;
	flags->len = -1; // -1 means til EOF
	flags->uppercase = false;
	flags->decimaloffset = false;

}

void free_hex_chunk(hex_chunk_t* chunk)
{
	free(chunk->text);
	free(chunk->hex);
}

void add_text_to_chunk(char* src, char** dst)
{
	*dst = malloc(sizeof(char) * (flags.cols + 1));

	strncpy(*dst, src, flags.cols);
	(*dst)[TEXT_LINE_LEN] = '\0';
}

void convert_text_to_hex(hex_chunk_t* chunk)
{
	uint i, j;
	chunk->hex = malloc(sizeof(char) * ((flags.cols * 2) + 1));
	
	for (i = 0, j = 0; i < (flags.cols * 2); i += 2, j += 1) {
		if (chunk->text[j] == '\0') {
			snprintf(chunk->hex + i, 3, "  ");
		} else {
			snprintf(chunk->hex + i, 3, flags.uppercase ? "%02X ": "%02x", chunk->text[j]);
		}
	}
}

void display_hex_chunk(hex_chunk_t* chunk, FILE* stream)
{
	uint i, j;

	fprintf(stream, "%08x: \x1b[32m", chunk->line * flags.cols);
	for (i = 0; i < (flags.cols * 2); i += 4) {
		for (j = 0; j < 4; j += 2) {
			if (((chunk->hex + i) + j)[0] == '0' && ((chunk->hex + i) + j)[1] == 'a')
				fprintf(stream, "\x1b[33m%2.2s\x1b[32m", chunk->hex + i + j);
			else
				fprintf(stream, "%2.2s", chunk->hex + i + j);
		}
		fprintf(stream, " ");
	}

	for (i = 0; i < flags.cols; i++) {
		if (chunk->text[i] == '\n' || chunk->text[i] == EOF)
			fprintf(stream, "\x1b[33m.\x1b[32m");
		else
			fprintf(stream, "%c", chunk->text[i]);
	}
	fprintf(stream, "\x1b[0m\n");
}
