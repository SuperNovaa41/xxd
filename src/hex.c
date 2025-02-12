#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#include "include/hex.h"

extern struct flags flags;

void init_flags(struct flags* flags)
{
	flags->file_in = false;
	flags->files[0] = NULL;
	flags->files[1] = NULL;	

	flags->customcols = false;
	flags->coloured = true;

	flags->offset = 0;

	flags->octets = 2;
	flags->len = -1; // -1 means til EOF
	flags->uppercase = false;
	flags->decimaloffset = false;

	flags->postscript = false;

	flags->c_style = false;
	flags->c_filename = NULL;
	flags->cap_c_style = false;
}

void init_cols(struct flags* flags)
{
	if (flags->customcols)
		return;

	if (flags->postscript == true)
		flags->cols = 30;
	else if (flags->c_style == true)
		flags->cols = 12;
	else
		flags->cols = 16;
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

	(*dst)[flags.cols] = '\0';
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

static bool is_newline(bool hex, char c)
{
	if (hex) {
		if (c == 'a' || c == 'A')
			return true;
	} else {
		if (c == '\n' || c == EOF)
			return true;
	}
	return false;
}

static void write_offset(int num, FILE* stream)
{
	if (flags.decimaloffset)
		fprintf(stream, "%08d", num);
	else
	 	fprintf(stream, "%08x", num);
	fprintf(stream, ": %s", (flags.coloured) ? GREEN_TEXT_STR : "");
}

static void write_text(char** text, FILE* stream)
{
	uint i;
	bool newline;
	const char* yellow_str = (flags.coloured) ? YELLOW_TEXT_STR : "";
	const char* green_str = (flags.coloured) ? GREEN_TEXT_STR : "";


	fprintf(stream, " ");

	for (i = 0; i < flags.cols; i++) {
		newline = is_newline(false, (*text)[i]);

		fprintf(stream, "%s%c%s", (newline ? yellow_str : ""),
				(newline ? '.' : (*text)[i]), 
				(newline ? green_str : ""));
	}
}

static void write_octet(char a, char b, FILE* stream)
{
	bool newline;
	const char* yellow_str = (flags.coloured) ? YELLOW_TEXT_STR : "";
	const char* green_str = (flags.coloured) ? GREEN_TEXT_STR : "";

	newline = (a == '0') && is_newline(true, b);
	
	fprintf(stream, "%s%c%c%s", (newline ? yellow_str : ""),
			a, b, (newline ? green_str : ""));
}

static void standard_output(char** text, FILE* stream)
{
	uint i, j;

	for (i = 0; i < (flags.cols * 2); i += (flags.octets * 2)) {
		for (j = 0; j < (flags.octets * 2); j += 2)
			write_octet(((*text) + i + j)[0], ((*text) + i + j)[1], stream);
		if (!flags.postscript)
			fprintf(stream, " ");
	}

}

static void c_output(char** text, FILE* stream)
{
	uint i;

	printf("\t");

	printf("0x");
	write_octet((*text)[0], (*text)[1], stream);
	for (i = 2; i < (flags.cols * 2); i += 2) {
		if ((*text)[i] == ' ')
			continue;
		printf(", 0x");
		write_octet((*text)[i], (*text)[i + 1], stream);
	}
}

void display_hex_chunk(hex_chunk_t* chunk, FILE* stream)
{
	if (!flags.postscript && !flags.c_style)
		write_offset((chunk->line * flags.cols) + flags.offset, stream);

	if (flags.c_style)
		c_output(&(chunk->hex), stream);
	else
		standard_output(&(chunk->hex), stream);

	if (!flags.postscript && !flags.c_style)
		write_text(&(chunk->text), stream);


	fprintf(stream, RESET_TEXT_STR);
}
