Section 03: Syscalls and I/O

```
./section-03.pdf
```

## 2. Problems

### 2.1 Signals

#### 2.1.1 Warmup

How do we stop the following program?

```
int main(void) {
	signal(SIGINT, SIG_IGN);
	while (1);
}
```

#### 2.1.2 Did you really want to quit?

Fill in the blanks for the following function using syscalls such that

when user types ctrl-c, he is prompted with a message:

"do you really want to quit [y/n]?"

y -> leads to program termination

```
cd p_2.1.2
make
./main.out
```

### 2.2 Files

#### 2.2.1 Files vs File Descriptor

What's the difference between fopen and open?

fopen - opens the file, streaming interface, returns FILE * which is used to

refer to that file, in use of other user level libs.

open - opens the file, returns fd which is handle that used by other functions

to refer to the that file. syscall 

#### 2.2.2 Quick practice with write and seek

What will the test.txt look like after i run this program?

(if you write at an offset past the end of file, the bytes inbetween the end

of the file and the offset will be set to 0.)

```
cd p_2.2.2
make
./main.out
hexdump test.txt
```

```
aaaaaaa0000000aaaaaaa
0---200-------600-700
```

### 2.3 dup and dup2

#### 2.3.1 Warmup

What does c print in the following code?

```
int main(int argc, char **argv) {
	int pid, status;
	int newfd;

	if ((newfd = open("output_file.txt", O_CREAT|O_TRUNC|O_WRONLY, 0644)) < 0) {
		exit(1);
	}

	printf("Luke, I am your...\n");
	dup2(newfd, 1);
	printf("father\n");
	exit(0);
}
```

```
program writes:
Luke, i am your...

to standart output

and:
father

to the output_file.txt
```

#### 2.3.2 Redirection: executing a process after dup2

Desribe what happens, and what the output will be.

```
int
main(int argc, char **argv) {
	int pid, status;
	int newfd;
	char *cmd[] = { "/bin/ls", "-al", "/", 0 };
	if (argc != 2) {
		fprintf(stderr, "usage: %s output_file\n", argv[0]);
		exit(1);
	}
	if ((newfd = open(argv[1], O_CREAT|O_TRUNC|O_WRONLY, 0644)) < 0) {
		perror(argv[1]); /* open failed */
		exit(1);
	}
	printf("writing output of the command %s to \"%s\"\n", cmd[0], argv[1]);
	dup2(newfd, 1);
	execvp(cmd[0], cmd);
	perror(cmd[0]); /* execvp failed */
	exit(1);
}
```

#### 2.3.3 Redirecting in a new process

Modify the above code such that the result of ls -al is written to the file

specified by the input argument and immediately after "all done" is printed

to the terminal.

```
cd p_2.3.3
make
./main output.txt
```

