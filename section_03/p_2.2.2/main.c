#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

int main(void) {
	char buffer[200];
	memset(buffer, 'a', 200);

	int fd = open("test.txt", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

	write(fd, buffer, 200);
	lseek(fd, 0, SEEK_SET);

	read(fd, buffer, 100);
	lseek(fd, 500, SEEK_CUR);

	write(fd, buffer, 100);

	close(fd);
}

