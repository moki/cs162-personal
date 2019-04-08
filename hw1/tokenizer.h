#pragma once

/* A struct that represents a list of words. */
struct tokens;

/* Turn a string into a list of words. */
struct tokens *tokenize(const char *line);

/* How many words are there? */
size_t tokens_get_length(struct tokens *tokens);

/* Get me the Nth word (zero-indexed) */
char *tokens_get_token(struct tokens *tokens, size_t n);

/* Free the memory */
void tokens_destroy(struct tokens *tokens);

/* Get token id */
ssize_t tokens_get_token_id(struct tokens *tokens, char *target);

