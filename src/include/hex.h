#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#ifndef HEX_H
#define HEX_H

#define TEXT_LINE_LEN 16
#define HEX_LINE_LEN 32

#define GREEN_TEXT_STR "\x1b[32m"
#define YELLOW_TEXT_STR "\x1b[33m"
#define RESET_TEXT_STR "\x1b[0m\n"

typedef struct {
	int line;
	char* text;
	char* hex;
} hex_chunk_t;

struct flags {
	bool file_in;
	char* files[2];
	uint cols; // choose the amount of columns to display (default 16) // done
	
	uint octets; // number of octets per line (default 2) // done
	int len; // max len to stop at //done 
	bool uppercase; // do uppercase hex chars // done
	bool decimaloffset; // do decimal offset instead of hex // done
};

// TODO: implement flags and stuff

void init_flags(struct flags* flags);

void free_hex_chunk(hex_chunk_t* chunk);
void add_text_to_chunk(char* src, char** dst);
void convert_text_to_hex(hex_chunk_t* chunk);
void display_hex_chunk(hex_chunk_t* chunk, FILE* stream);

#endif
