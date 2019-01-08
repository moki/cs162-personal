#include <stdio.h>
#include "wc.h"

int main(int argc, char **argv) {
	int32_t	in_fd;
	ssize_t n_read;
	bool    is_in_fd_file;
	bool    in_word;
	char   *buffer;

	uint_fast64_t lines;
	uint_fast64_t words;
	uint_fast64_t chars;
	uint_fast32_t i;
	uint_fast8_t  err;

	in_fd = STDIN_FILENO;
	is_in_fd_file = 0;
	err = 0;

	if (argc > 2) {
		err = 1;
		goto clean;
	}

	if (argc == 2) { 
		in_fd = open(argv[1], O_RDONLY); 

		if (in_fd == -1) {
			err = 1;
			goto clean;
		}

		is_in_fd_file = 1;
	}

	buffer = malloc(sizeof(char) * (WC_BUFFER_SIZE + 1));

	lines = 0;
	words = 0;
	chars = 0;
	
	in_word = false;
	
	while ((n_read = read(in_fd, buffer, WC_BUFFER_SIZE)) > 0) {
		buffer[n_read] = '\0';

		for (i = 0; i < n_read; i++) {
			chars++;

			if (!isspace(buffer[i]) && !in_word) { 
				words++;
				in_word = true;
				continue;
			}

			if (isspace(buffer[i])) {
				if (buffer[i] == '\n')
					lines++;

				in_word = false;
			}
		}
	}

	printf(" %lu  %lu %lu %s\n", lines, words, chars, is_in_fd_file ? argv[1] : "");

clean:

	if (in_fd && is_in_fd_file)
		err = close(in_fd) == -1;

	if (buffer) {
		free(buffer);
		buffer = NULL;
	}

	return err;
}

