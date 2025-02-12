#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <argp.h>

#include "include/hex.h"
#include "include/file.h"

const char* argp_program_version = "xxd 0.2";
const char* argp_program_bug_address = "supernovaa41@gmx.com";
static char doc[] = "Recreation of the GNU util xxd. Creates a hex dump of the given input.";
static char args_doc[] = "[INFILE [OUTFILE]]";
static struct argp_option options[] = {
	{0, 'c', "cols", 0, "format <cols> octets per line. Default 16 (-i: 12, -p: 30).", 0},
	{0, 'g', "bytes", 0, "number of octets per group in normal output. Default 2.", 0},
	{0, 'l', "len", 0, "stop after <len> octets.", 0},
	{0, 'u', 0, 0, "use upper case hex letters.", 0},
	{0, 'd', 0, 0, "show offset in decimal instead of hex.", 0},
	{"ps", 'p', 0, 0, "output in postscript plain hexdump style.", 0},
	{0, 'R', "when", 0, "colorize the output; <when> can be 'always' or 'never'. Default: 'always'.", 0},
	{0, 'i', 0, 0, "output in C include file style.", 0},
	{0, 'C', 0, 0, "capitalize variable names in C include file style (-i).", 0},
	{0, 'n', "name", 0, "set the variable name used in C include output (-i).", 0},
	{0, 'o', "off", 0, "add <off> to the displayed file position.", 0},
	{0, 'e', 0, 0, "little-endian dump (incompatible with -ps,-i,-r).", 0},
	{0, 's', "seek", 0, "start at <[+][-] seek> bytes abs. infile offset.", 0},
	{0}
};

static error_t parse_opt(int key, char* arg, struct argp_state* state)
{
	struct flags* flags = state->input;
	switch(key) {
		case 'C':
			flags->cap_c_style = true;
			break;
		case 'c':
			flags->cols = atoi(arg);
			flags->customcols = true;
			break;
		case 'n':
			flags->c_filename = arg;
			break;
		case 'g':
			flags->octets = atoi(arg);
			flags->customoctets = true;
			break;
		case 'o':
			flags->offset += atoi(arg);
			break;
		case 's':
			flags->seek = atoi(arg);
			if (flags->seek >= 0)
				flags->offset += atoi(arg);
			break;
		case 'e':
			flags->littleendian = true;
			break;
		case 'R':
			if (strcmp(arg, "none") == 0)
				flags->coloured = false;
			else if (strcmp(arg, "always") == 0)
				flags->coloured = true;
			else
			 	flags->coloured = true;
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
		case 'p':
			flags->postscript = true;
			flags->coloured = false;
			break;
		case 'i':
			flags->c_style = true;
			flags->coloured = false;
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
	uint seek;
	
	if (interactive) {
		max_len = INT_MAX;
		getdelim(&file_content, &max_len, EOF, stdin);
	} else {
		read_file_to_buf(flags.files[0], &file_content);
	}

	if (flags.seek >= 0) {
		if (flags.seek > strlen(file_content))
			exit(EXIT_SUCCESS);
		seek = flags.seek;
	} else {
		if ((flags.seek * -1) > strlen(file_content)) {
			fprintf(stderr, "xxd: Sorry, cannot seek.\n");
			exit(EXIT_FAILURE);
		}
		seek = (strlen(file_content) - flags.seek);
	}

	filesize = (flags.len == -1) ? strlen(file_content + seek) : flags.len;

	flags.len = filesize;

	hex_lines = get_hex_lines(filesize);

	*lines = malloc(sizeof(hex_chunk_t) * (hex_lines + 1));	


	for (i = 0; i < hex_lines; i++) {
		(*lines)[i].line = i;
		add_text_to_chunk((file_content + seek) + (i * (flags.cols)), &((*lines)[i].text));
		convert_text_to_hex(&(*lines)[i]);
	} 
	
	(*lines)[hex_lines].line = -1;
	free(file_content);
}

static char parse_c_char(char c)
{
	if (c >= 'A' && c <= 'Z')
		return c;
	else if (c >= 'a' && c <= 'z')
		return (flags.cap_c_style) ? toupper(c) : c;
	return '_';
}

static void parse_c_filename(char** text, char** filename)
{
	uint i;

	if ((*text)[0] >= '0' && (*text)[0] <= '9')
		asprintf(filename, "__%c", (*text)[0]); 
	else
		asprintf(filename, "%c", parse_c_char((*text)[0]));

	for (i = 1; i < strlen(*text); i++) {
		asprintf(filename, "%s%c", *filename,
				parse_c_char((*text)[i]));
	}
}

static void do_display(hex_chunk_t** lines)
{
	int i;
	char* filename;


	bool fileout = false;
	FILE* stream = stdout;

	if (flags.files[1] != NULL) {
		stream = fopen(flags.files[1], "w");
		fileout = true;
	}

	i = 0;
	
	if (flags.c_style) {
		parse_c_filename((!flags.c_filename) ? &(flags.files[0]) : &(flags.c_filename), 
				&filename);

		printf("unsigned char %s[] = {\n", filename);
	}

	while ((*lines)[i].line != -1)
		display_hex_chunk(&((*lines)[i++]), stream);

	if (flags.c_style) {
		asprintf(&filename, "%s_%s", filename, (flags.cap_c_style) ? "LEN" : "len");

		printf("};\n");
		printf("unsigned int %s = %d;\n", filename, flags.len);
		free(filename);
	}

	if (fileout)
		fclose(stream);

}

int main(int argc, char* argv[])
{
	int i;
	hex_chunk_t* lines;

	init_flags(&flags);

	argp_parse(&argp, argc, argv, 0, 0, &flags);

	init_var_defaults(&flags);
	
	do_text_parse(&lines, (flags.files[0] == NULL ? true : false));

	do_display(&lines);

	i = 0;
	while (lines[i].line != -1)
		free_hex_chunk(&(lines[i++]));
	free(lines);

	return 0;
}
