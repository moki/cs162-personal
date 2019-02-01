## Section 02: Processes

```
./section-02.pdf
```

## 1. Warmup

### 1.1 Hello World

What can c print in the below code? Assume the child PID is 90210.

```
int main() {
	pid_t pid = fork();
	printf("Hello World: %d\n", pid);
}
```

In the parent process pid is going to be > 0, exactly pid of the child process,

which is 90210 by the assumption.

In the child process pid is going to be 0, indicating that we are in the child process.

Also a possability to have pid < 0 indicating that we somehow failed to "fork" a process.

## 3. Problems

### 3.1 Forks

How many new processes are created in the below program?

```
int main(void) {
	for (int i = 0; i < 3; i++) {
		pid_t pid = fork();
	}
}
```

7 new child processes created, from original parent one, so 8.

*(parent)
|i=0--->|i=1--->|i=2--->*
|	|
|	|i=2--->*
|
|i=1--->|i=2--->*
|
|
|i=2--->*

### 3.2 Stack allocation

What can C print?

```
int main(void) {
	int stuff = 7;
	pid_t pid = fork();
	printf("Stuff is %d \n", stuff);
	if (pid == 0)
		stuff = 6;
}
```

7
7

### 3.3 Heap allocation

What can C print?

```
int main(void) {
	int *stuff = malloc(sizeof(int)*1);
	*stuff = 7;
	pid_t pid = fork();
	printf("Stuff is %d\n", *stuff);
	if (pid == 0)
		*stuff = 6;
}
```

7
7

### 3.4 Slightly more complex heap allocation

```
void print_ten_numbers(int *a) {
	int i;
	printf("\n");
	for (i = 0; i<10; i++)
		printf("%d", a[i]);
	exit(0);
}

int main(void) {
	int *a, i;

	a = malloc(sizeof(int));

	a[0] = 0;

	for (i=1; i<10; i++) {
		a = realloc(a, (i+1) * sizeof(int));
		a[i] = i;

		if (i == 7) {
			pid_t pid = fork();
			if (pid == 0) {
				print_ten_numbers(a);
			}
		}
	}

	print_ten_numbers(a);
}
```

Code above will print numbers from 0 to 7 inclusive
and fail with segmentation fault(possibly), for a child process.

For a parent it will print numbers 0 to 9 inclusive.

### 3.5 Simple Wait

What can C print? Assume the child PID is 90210.

```
int main(void) {
	pid_t pid = fork();
	int exit;

	if (pid != 0)
		wait(&exit);
	
	printf("Hello World\n: %d\n", pid);
}
```

Code above will print:
in case of successfully forking process:
Hello World
: 0

Hello World
: 90210

in case of failing to do so:
Hello World
: -1

What is the equivalent program using the waitpid function instead of wait?

replace

```
wait(&exit);
```

with

```
waitpid(-1, &exit, 0);
```

### 3.6 Fork and File Descriptors

What will be stored in the file "output.txt"

```
int main(void) {
	int fd;

	fd = open("output.txt", O_CREAT | O_TRUNC | O_WRONLY, 0666);

	if(!fork()) {
		write(fd, "hello ", 6);
		exit(0);
	} else {
		int status;

		wait(&status);
		write(fd, "world\n", 6);
	}
}
```

output.txt:

```
hello world

```

### 3.7 Exec

What will C print

```
int main(void) {
	char **argv = malloc(3*sizeof(char *));

	argv[0] = "/bin/ls";
	argv[1] = ".";
	argv[2] = NULL;

	for (int i = 0; i < 10; i++) {
		printf("%d\n", i);

		if (i==3)
			execv("/bin/ls", argv);
	}
}
```

```
0
1
2
3

list of files/folders in the current working directory.
```

### 3.8 Exec + Fork

How would i modify the above program using fork so it both prints the output

of ls and all the numbers from 0 to 9? You may not remove lines from the original program;

only add statements(and use fork).

```
int main(void) {
	char **argv = malloc(3*sizeof(char *));

	argv[0] = "/bin/ls";
	argv[1] = ".";
	argv[2] = NULL;

	for (int i = 0; i < 10; i++) {
		printf("%d\n", i);

		if (i==3) {
			pid_t pid = fork();

			if (!pid)
				execv("/bin/ls", argv);
		}
	}
}
```

### 3.9 Implementing fork() efficiently

Remember fork() makes the child process's address space exactly the same as its parent's.

If you were designing an OS, list some steps you would take to make this address space copy more efficient?

```
deferr copy to the first write.
apparently: https://en.wikipedia.org/wiki/Copy-on-write
```

