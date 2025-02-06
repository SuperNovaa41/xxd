#include <stdio.h>

#ifndef HEX_H
#define HEX_H

#define TEXT_LINE_LEN 16
#define HEX_LINE_LEN 32

typedef struct {
	int line;
	char* text;
	char* hex;
} hex_chunk_t;

void free_hex_chunk(hex_chunk_t* chunk);
void add_text_to_chunk(char* src, char** dst);
void convert_text_to_hex(hex_chunk_t* chunk);
void display_hex_chunk(hex_chunk_t* chunk, FILE* stream);

#endif
