#ifndef FILE_H
#define FILE_H

/**
 * read_file_to_buf
 *
 * - const char* filename - Name of the file we're opening
 * - char** buf - pointer to the buffer to hold the file in
 *
 *   read the file into a buffer to prevent unnecessary file ops
 */
void read_file_to_buf(const char* filename, char** buf);

#endif
