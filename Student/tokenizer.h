#ifndef __TOKENIZER_H__
#define __TOKENIZER_H__

/* Return null terminated array with k pointers pointing to
 * memory places in str where k is the number of characters
 * from delims in str.
 */
char **tokenize(char *str, const char *delims);

#endif
