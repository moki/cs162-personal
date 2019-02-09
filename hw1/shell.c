#include <ctype.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>
#include <termios.h>
#include <unistd.h>

#include "tokenizer.h"

/* Convenience macro to silence compiler warnings about unused function parameters. */
#define unused __attribute__((unused))

/* Whether the shell is connected to an actual terminal or not. */
bool shell_is_interactive;

/* File descriptor for the shell input */
int shell_terminal;

/* Terminal mode settings for the shell */
struct termios shell_tmodes;

/* Process group id for the shell */
pid_t shell_pgid;

int cmd_exit(struct tokens *tokens);
int cmd_help(struct tokens *tokens);
int cmd_pwd(struct tokens *tokens);
int cmd_cd(struct tokens *tokens);

/* Built-in command functions take token array (see parse.h) and return int */
typedef int cmd_fun_t(struct tokens *tokens);

/* Built-in command struct and lookup table */
typedef struct fun_desc {
	cmd_fun_t *fun;
	char *cmd;
	char *doc;
} fun_desc_t;

fun_desc_t cmd_table[] = {
	{cmd_help, "?", "show this help menu"},
	{cmd_exit, "exit", "exit the command shell"},
	{cmd_pwd, "pwd", "print working directory"},
	{cmd_cd, "cd", "change directory to <path>"},
};

/* Prints a helpful description for the given command */
int cmd_help(unused struct tokens *tokens) {
	for (unsigned int i = 0; i < sizeof(cmd_table) / sizeof(fun_desc_t); i++)
		printf("%s - %s\n", cmd_table[i].cmd, cmd_table[i].doc);
	return 1;
}

/* Exits this shell */
int cmd_exit(unused struct tokens *tokens) {
	exit(0);
}

int cmd_pwd(unused struct tokens *tokens) {
	char *path_buf = NULL;

	if ((path_buf = getcwd(NULL, 0))) {
		fprintf(stdout, "%s\n", path_buf);
		free(path_buf);
	} else {
		return -1;
	}

  	return 1;
}

int cmd_cd(struct tokens *tokens) {
	if (chdir(tokens_get_token(tokens, 1)) == -1)
		return -1;
	return 1;
}

/* Looks up the built-in command, if it exists. */
int lookup(char cmd[]) {
	for (unsigned int i = 0; i < sizeof(cmd_table) / sizeof(fun_desc_t); i++)

	if (cmd && (strcmp(cmd_table[i].cmd, cmd) == 0))
		return i;

	return -1;
}

/* Intialization procedures for this shell */
void init_shell() {
	/* Our shell is connected to standard input. */
	shell_terminal = STDIN_FILENO;

	/* Check if we are running interactively */
	shell_is_interactive = isatty(shell_terminal);

	if (shell_is_interactive) {
		/* If the shell is not currently in the foreground, we must pause the shell until it becomes a
		 * foreground process. We use SIGTTIN to pause the shell. When the shell gets moved to the
		 * foreground, we'll receive a SIGCONT. */
		while (tcgetpgrp(shell_terminal) != (shell_pgid = getpgrp()))
			kill(-shell_pgid, SIGTTIN);

		/* Saves the shell's process id */
		shell_pgid = getpid();

		/* Take control of the terminal */
		tcsetpgrp(shell_terminal, shell_pgid);

		/* Save the current termios to a variable, so it can be restored later. */
		tcgetattr(shell_terminal, &shell_tmodes);
	}
}

int main(unused int argc, unused char *argv[]) {
	init_shell();

	static char line[4096];
	int line_num = 0;

	/* Please only print shell prompts when standard input is not a tty */
	if (shell_is_interactive)
		fprintf(stdout, "%d: ", line_num);

	while (fgets(line, 4096, stdin)) {
		/* Split our line into words. */
		struct tokens *tokens = tokenize(line);

		/* Find which built-in function to run. */
		int fundex = lookup(tokens_get_token(tokens, 0));

		if (fundex >= 0) {
			cmd_table[fundex].fun(tokens);
		} else {
			size_t i, j;

			char *path_buf = NULL;
			char *path = NULL;
			char  c;

			bool found_executable = false;
			bool clean_path_buf = false;
			bool clean_path = true;

			// check if absolute path.
			if (tokens_get_token(tokens, 0)[0] == '/') {
				path = tokens_get_token(tokens, 0);
				clean_path = false;

				if (!access(path, X_OK))
					goto execute;
				else
					goto clean;
			}

			// get path env
			path_buf = getenv("PATH");

			// alternatively get posix path
			// path_buf = NULL;
			if (!path_buf) {
				size_t n = confstr(_CS_PATH, NULL, (size_t) 0);

				path_buf = malloc(n);
				if (!path_buf)
					abort();

				clean_path_buf = true;

				confstr(_CS_PATH, path_buf, n);
			}

			// traverse path string
			for (i = 0, j = 0, c = path_buf[i], path = NULL; c != '\0'; i++) {
				path = realloc(path, j + 1);

				if (!path)
					abort();

				c = path_buf[i];

				if (c == ':' || c == '\0') {
					path = realloc(path, j + 2 + strlen(tokens_get_token(tokens, 0)));
					path[j] = '/';
					path[j + 1] = '\0';

					strcat(path, tokens_get_token(tokens, 0));

					if (!access(path, X_OK)) {
						found_executable = true;

						break;
					}

					j = 0;
				} else {
					path[j++] = c;
				}
			}

			// look in the current working directory
			if (!found_executable) {
				if (clean_path)
					free(path);

				path = getcwd(NULL, 0);
				i = strlen(path);
				path = realloc(path, i + 2 + strlen(tokens_get_token(tokens, 0)));

				path[i] = '/';
				path[i + 1] = '\0';

				strcat(path, tokens_get_token(tokens, 0));

				if (access(path, X_OK) == -1)
					goto clean;
			}
execute: ;
			char **_args = malloc(sizeof(char *));
			_args[0] = path;

			for (i = 1; i < tokens_get_length(tokens); i++) {
				_args = realloc(_args, sizeof(char *) * (i + 1));

				_args[i] = tokens_get_token(tokens, i);
			}

			_args = realloc(_args, sizeof(char *) * (i + 1));
			_args[i] = NULL;

			int status;
			pid_t pid = fork();
			if (pid == -1) {
				perror("fork");
				exit(EXIT_FAILURE);
			} else if (pid == 0) {
				if (execv(path, _args) == -1) {
					free(_args);
					perror("exec");
					exit(EXIT_FAILURE);
				}
			} else {
				wait(&status);
			}

			free(_args);
clean:
			if (clean_path)
				free(path);

			if (clean_path_buf)
				free(path_buf);
		}

		/* Please only print shell prompts when standard input is not a tty */
    		if (shell_is_interactive)
			fprintf(stdout, "%d: ", ++line_num);

		/* Clean up memory */
		tokens_destroy(tokens);
	}

	return 0;
}
