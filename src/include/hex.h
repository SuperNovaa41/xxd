#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#ifndef HEX_H
#define HEX_H

#define TEXT_LINE_LEN 16
#define HEX_LINE_LEN 32

typedef struct {
	int line;
	char* text;
	char* hex;
} hex_chunk_t;

struct flags {
	bool autoskip; // should we condense null lines into *
	bool binary; // do binary dump instead of hex
	uint cols; // choose the amount of columns to display (default 16)
	
	uint octets; // number of octets per line (default 2)
	int len; // max len to stop at
	bool uppercase; // do uppercase hex chars
	bool decimaloffset; // do decimal offset instead of hex
};

// TODO: implement flags and stuff

void init_flags(struct flags* flags);

void free_hex_chunk(hex_chunk_t* chunk);
void add_text_to_chunk(char* src, char** dst);
void convert_text_to_hex(hex_chunk_t* chunk);
void display_hex_chunk(hex_chunk_t* chunk, FILE* stream);

#endif
