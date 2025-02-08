#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <argp.h>

#include "include/hex.h"
#include "include/file.h"

const char* argp_program_version = "xxd 0.1";
const char* argp_program_bug_address = "supernovaa41@gmx.com";
static char doc[] = "Creates a hex dump of the given file.";
static char args_doc[] = "INFILE [OUTFILE]";
static struct argp_option options[] = {
	{"cols", 'c', "cols", 0, "format <cols> octets per line. Default 16.", 0},
	{"octets", 'g', "bytes", 0, "number of octets per group in normal output. Default 2.", 0},
	{"len", 'l', "len", 0, "stop after <len> octets.", 0},
	{"uppercase", 'u', 0, 0, "use upper case hex letters.", 0},
	{"decimaloffset", 'd', 0, 0, "show offset in decimal instead of hex.", 0},
	{0}
};

static error_t parse_opt(int key, char* arg, struct argp_state* state)
{
	struct flags* flags = state->input;
	switch(key) {
		case 'c':
			flags->cols = atoi(arg);
			break;
		case 'g':
			flags->octets = atoi(arg);
			break;
		case 'l':
			flags->len = atoi(arg);
			break;
		case 'u':
			flags->uppercase = true;
			break;
		case 'd':
			flags->decimaloffset = true;
			break;
		case ARGP_KEY_ARG:
			if (state->arg_num >= 2)
				argp_usage(state);
			flags->files[state->arg_num] = arg;
			flags->file_in = true;
			break;
		case ARGP_KEY_END:
			break;
		default:
			return ARGP_ERR_UNKNOWN;
	}
	return 0;
}

static struct argp argp = {options, parse_opt, args_doc, doc, 0, 0, 0};

struct flags flags;

static int get_hex_lines(int len)
{
	int out;

	out = len / (flags.cols);
	if (len % (flags.cols) != 0) // if we have a less than a full line remaining
		out++;

	return out;
}

static void do_text_parse(hex_chunk_t** lines, bool interactive)
{
	char* file_content = NULL;
	int hex_lines, i;
	size_t filesize, max_len;
	
	if (interactive) {
		max_len = INT_MAX;
		getdelim(&file_content, &max_len, EOF, stdin);
	} else {
		read_file_to_buf(flags.files[0], &file_content);
	}

	filesize = (flags.len == -1) ? strlen(file_content) : flags.len;
	hex_lines = get_hex_lines(filesize);

	*lines = malloc(sizeof(hex_chunk_t) * (hex_lines + 1));	


	for (i = 0; i < hex_lines; i++) {
		(*lines)[i].line = i;
		add_text_to_chunk(file_content + (i * (flags.cols)), &((*lines)[i].text));
		convert_text_to_hex(&(*lines)[i]);
	} 
	
	(*lines)[hex_lines].line = -1;
	free(file_content);
}

static void do_display(hex_chunk_t** lines)
{
	int i;
	bool fileout = false;

	FILE* stream = stdout;

	if (flags.files[1] != NULL) {
		stream = fopen(flags.files[1], "w");
		fileout = true;
	}

	i = 0;
	while ((*lines)[i].line != -1)
		display_hex_chunk(&((*lines)[i++]), stream);

	if (fileout)
		fclose(stream);

}

int main(int argc, char* argv[])
{
	int i;
	hex_chunk_t* lines;

	init_flags(&flags);

	argp_parse(&argp, argc, argv, 0, 0, &flags);

	
	do_text_parse(&lines, (flags.files[0] == NULL ? true : false));

	do_display(&lines);

	i = 0;
	while (lines[i].line != -1)
		free_hex_chunk(&(lines[i++]));
	free_hex_chunk(&(lines[i]));

	free(lines);


	return 0;
}
