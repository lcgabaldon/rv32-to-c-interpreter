#ifndef __PROCESS_FILE_H__
#define __PROCESS_FILE_H__

#include <stddef.h>  //size_t

#define LINE_SIZE ((size_t)19)

/* Given a file make a new one with the same information but that is seekable
 */
int process_file(const char *file);

/* Set up file reading. */
int open_file(void);

/* Get the i line from the process file and store it in buffer. Buffer is
 * expected to have at least size LINE_SIZE + 1
 */
void get_line(char *buffer, size_t i);

/* Close file. */
int close_file(void);

size_t get_n_lines(void);

#endif
