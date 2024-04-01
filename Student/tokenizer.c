#include <stdio.h>

#include "my_string.h"  //str_chr() str_sep()
#include "tokenizer.h"

/* Return the number of tokens in str by using delims for the
 * special character to tokenize. str and delims are expected
 * to be null terminated.
 */
static size_t __n_tokens(const char *str, const char *delims) {
  size_t k_tokens;

  for (k_tokens = 1; *str != '\0'; ++str)
    if (str_chr(delims, *str) != NULL) ++k_tokens;

  return k_tokens;
}

char **tokenize(char *str, const char *delims) {
  char **tokens;
  char *token;
  char **p;

  tokens = (char **)malloc((__n_tokens(str, delims) + 1) * sizeof(char *));
  if (tokens == NULL) return NULL;

  p = tokens;
  while ((token = str_sep(&str, delims)) != NULL) *p++ = token;

  *p++ = str;
  *p = NULL;

  return tokens;
}
