#include <errno.h>     //errno
#include <fcntl.h>     //open()
#include <stdio.h>     //FILE, fopen(), fclose()
#include <stdlib.h>    //malloc(), free()
#include <string.h>    //strerror()
#include <sys/mman.h>  //mmap(), munmap()
#include <unistd.h>    //close(), lseek()

#include "process_file.h"

#define SEEK_FILE_NAME ("seekable_file.txt")

typedef struct {
  char line[LINE_SIZE];
  char newline;
} entry_t;

size_t n_lines;
entry_t *file_lines;
int fd;

static void __close_file(int __fd) {
  if (close(__fd) < 0)
    fprintf(stderr, "Error closing file \"%s\": %s\n", SEEK_FILE_NAME,
            strerror(errno));
}

int open_file(void) {
  size_t filesize;
  off_t lseek_res;
  void *ptr;

  fd = open(SEEK_FILE_NAME, O_RDONLY);

  if (fd < 0) {
    fprintf(stderr, "Error opening file \"%s\": %s\n", SEEK_FILE_NAME,
            strerror(errno));
    return 1;
  }

  lseek_res = lseek(fd, (off_t)0, SEEK_END);

  if (lseek_res == (off_t)-1) {
    fprintf(stderr, "Error seeking in file \"%s\": %s\n", SEEK_FILE_NAME,
            strerror(errno));
    __close_file(fd);
    return 1;
  }

  filesize = (size_t)lseek_res;

  if ((filesize % sizeof(entry_t)) != ((size_t)0)) {
    fprintf(stderr, "The file \"%s\" is not properly formatted.\n",
            SEEK_FILE_NAME);
    __close_file(fd);
    return 1;
  }

  ptr = mmap(NULL, filesize, PROT_READ, MAP_SHARED, fd, 0);
  if (ptr == MAP_FAILED) {
    fprintf(stderr, "Error mapping file \"%s\" in memory : %s\n",
            SEEK_FILE_NAME, strerror(errno));
    __close_file(fd);
    return 1;
  }

  file_lines = ((entry_t *)ptr);
  n_lines = filesize / sizeof(entry_t);

  return 0;
}

int close_file(void) {
  if (munmap((void *)file_lines, n_lines * sizeof(entry_t)) < 0) {
    fprintf(stderr, "Error unmapping file \"%s\" in memory : %s\n",
            SEEK_FILE_NAME, strerror(errno));
    __close_file(fd);
    return 1;
  }

  __close_file(fd);

  return 0;
}

void get_line(char *buffer, size_t k) {
  // Copy line from file into buffer
  memcpy(buffer, file_lines[k].line, LINE_SIZE);

  // Set the null character in buffer
  buffer[LINE_SIZE] = '\0';

  // Trim line in buffer
  for (int i = LINE_SIZE - 1; i > 0; --i) {
    if (buffer[i] != ' ') break;
    buffer[i] = '\0';
  }

  return;
}

int process_file(const char *file) {
  // Files
  FILE *read_f;
  FILE *write_f;
  size_t n_lines;

  // Buffer variables
  char *buffer;
  size_t BUF_SIZE = LINE_SIZE * 2;
  ssize_t n_chars;

  // Allocate buffer memory
  buffer = (char *)malloc(BUF_SIZE * sizeof(char));
  if (buffer == NULL) {
    fprintf(stderr, "Could not allocate memory for buffer.\nError: %s\n",
            strerror(errno));
    return 1;
  }

  // Open file given as read only
  if ((read_f = fopen(file, "r")) == NULL) {
    fprintf(stderr, "Could not open file: %s\nError: %s\n", file,
            strerror(errno));
    free(buffer);
    return 1;
  }

  // Open a new file name SEEK_FILE_NAME as write only
  if ((write_f = fopen(SEEK_FILE_NAME, "w")) == NULL) {
    fprintf(stderr, "Could not open file: %s\nError: %s\n", SEEK_FILE_NAME,
            strerror(errno));
    free(buffer);
    fclose(read_f);
    return 1;
  }

  // Count number of lines to add one more at the end that overwrite the PC to
  // -1
  n_lines = (size_t)0;

  while ((n_chars = getline(&buffer, &BUF_SIZE, read_f)) > ((ssize_t)0)) {
    // n_chars - 1 because \n would be replace for \0
    --n_chars;
    buffer[n_chars] = '\0';

    if ((size_t)n_chars > LINE_SIZE) {
      fprintf(stderr,
              "The following line is longer than %zu characters: %s\n Increase "
              "LINE_SIZE in seek_file.c file or shrink the line.",
              LINE_SIZE, buffer);
      free(buffer);
      fclose(read_f);
      fclose(write_f);
      return 1;
    }

    // Write first n_chars to file
    fprintf(write_f, "%s", buffer);

    // Complete the remaining LINE_SIZE-n_chars characters with ' '
    fprintf(write_f, "%*s\n", (int)(LINE_SIZE - (size_t)n_chars), "");

    // Update number of lines read in the file
    n_lines++;
  }

  // Write a line at the end of file that overwrite PC to -1
  n_lines++;
  n_chars = fprintf(write_f, "J -%zu", 1 + n_lines * 4);
  fprintf(write_f, "%*s\n", (int)(LINE_SIZE - (size_t)n_chars), "");

  free(buffer);
  fclose(read_f);
  fclose(write_f);

  return 0;
}

size_t get_n_lines(void) {
    return n_lines;
}
