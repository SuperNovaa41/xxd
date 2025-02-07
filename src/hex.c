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
	flags->cols = 16;
	flags->octets = 4;
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
	bool newline;

	fprintf(stream, "%08x: %s", chunk->line * flags.cols, GREEN_TEXT_STR);
	newline = false;
	for (i = 0; i < (flags.cols * 2); i += (flags.octets * 2)) {
		for (j = 0; j < (flags.octets * 2); j += 2) {
			if (((chunk->hex + i) + j)[0] == '0' &&
					((chunk->hex + i) + j)[1] == 'a')
				newline = true;
				
			fprintf(stream, "%s%2.2s%s", (newline ? YELLOW_TEXT_STR : ""), chunk->hex + i + j, (newline ? GREEN_TEXT_STR : ""));

		}
		fprintf(stream, " ");
		newline = false;
	}

	for (i = 0; i < flags.cols; i++) {
		if (chunk->text[i] == '\n' || chunk->text[i] == EOF)
			newline = true;

		fprintf(stream, "%s%c%s", (newline ? YELLOW_TEXT_STR : ""),
				(newline ? '.' : chunk->text[i]), (newline ? GREEN_TEXT_STR : ""));
		newline = false;
	}
	fprintf(stream, RESET_TEXT_STR);
}
