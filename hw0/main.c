#include <stdio.h>
#include <sys/resource.h>

int main() {
	struct rlimit lim;

	int err = getrlimit(RLIMIT_STACK, &lim);	

	if (err == -1) {
		printf("error finding out stack size\n");
	}
 
	printf("stack size: %ld\n", lim.rlim_cur);

	err = getrlimit(RLIMIT_NPROC, &lim);

	if (err == -1) {
		printf("error finding out max # of threads\n");
	}

	printf("process limit: %ld\n", lim.rlim_cur);

	err = getrlimit(RLIMIT_NOFILE, &lim);

	if (err == -1) {
		printf("error finding out max # of fds\n");
	}

	printf("max file descriptors: %ld\n", lim.rlim_cur);

	return 0;
}

