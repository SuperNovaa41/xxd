#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "include/hex.h"
#include "include/file.h"

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

void init_flags(struct flags* flags)
{
	flags->autoskip = false;
	flags->binary = false;
	flags->cols = 16;
	flags->octets = 2;
	flags->len = -1; // -1 means til EOF
	flags->uppercase = false;
	flags->decimaloffset = false;

}

int get_hex_lines(int len)
{
	int out;

	out = len / 16;
	if (len % 16 != 0) // if we have a less than a full line remaining
		out++;

	return out;
}

int main(int argc, char* argv[])
{
	char* file_content;
	FILE* stream;
	bool outfile;
	int hex_lines, i;


	if (argc < 2) {
		fprintf(stderr, "Usage: %s [filename]\n", argv[0]);
		return EXIT_FAILURE;
	}

	stream = stdout;
	outfile = false;
	if (argc == 3) {
		stream = fopen(argv[2], "w");
		outfile = true;
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
		display_hex_chunk(&(lines[i]), stream);

	if (outfile)
		fclose(stream);
	free(file_content);	
	for (i = 0; i < hex_lines; i++)
		free_hex_chunk(&(lines[i]));
	free(lines);


	return 0;
}
