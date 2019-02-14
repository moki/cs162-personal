#include <stdlib.h>
#include <signal.h>
#include <stdio.h>

void sigint_handler(int sig) {
	char c;
	printf("\nDo you really wanna quit? [y/n]?\n");

	c = getchar();
	if (c == 'y' || c == 'Y') {
		exit(EXIT_SUCCESS);
	}
}

int main(void) {
	printf("hello\n");
	signal(SIGINT, sigint_handler);
	while(1);
}

