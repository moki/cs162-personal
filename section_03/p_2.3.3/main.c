#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char **argv) {
	int pid, status;
	int newfd;
	char *cmd[] = { "/bin/ls", "-al", "/", 0 };
	if (argc != 2) {
		fprintf(stderr, "usage: %s output_file\n", argv[0]);
		exit(1);
	}

	pid = fork();

	if (pid == -1) {
		perror("fork");
		exit(EXIT_FAILURE);
	} else if (pid == 0) {
		if ((newfd = open(argv[1], O_CREAT|O_TRUNC|O_WRONLY, 0644)) < 0) {
			perror(argv[1]); /* open failed */
			exit(1);
		}

		printf("writing output of the command %s to \"%s\"\n", cmd[0], argv[1]);

		dup2(newfd, 1);

		if (execvp(cmd[0], cmd) == -1) {
			perror(cmd[0]); /* execvp failed */
			exit(EXIT_FAILURE);
		}
	} else {
		if (wait(&status) == -1) {
			perror("wait");
			exit(EXIT_FAILURE);
		}

		fprintf(stdout, "all done.\n");
	}

	return 0;
}
