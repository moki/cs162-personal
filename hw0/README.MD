## Homework 0

```
./homework-0.pdf
```

## 1. Setup

### 1.3 Vagrant

vm provisioner:

```
https://github.com/Berkeley-CS162/vagrant/
```

```
mkdir cs162-vm
cd cs162-vm
git clone https://github.com/Berkeley-CS162/vagrant.git
vagrant init cs162/spring2017
vagrant up
vagrant ssh
```

to halt vm from the host machine

```
vagrant halt
```

to run vm from the host machine

```
vagrant up
```

to access vm through shell from the host machine

```
vagrant ssh
```

### 1.3.5 Repos

vm individual assignments path

```
cd ~/code/personal
```

git remote - personal

## 3 Assignments

### 3.1 make

makefile to compile all *.c sources in this homework,
a recipe [test-wc] to run shell script that tests 3.2 wc.out program,
[clean] removes all *.o object and *.out executables.

```
cat ./makefile
```

### 3.2 wc

wordcount utility implementation

```
cat wc.c

make

./wc.out [target]
```

compare original wc output and mine

```
make test-wc
```

### 3.3 Executables and addresses

#### 3.3.1 gdb

* What is the value of argv? (print argv)

	0x7fffffffe608 address pointer to the strings array of arguments.

* What is pointed to by argv? (print argv[0])

	0x7fffffffe831 first element in the argv string array of arguments,
	which is the absolute path to the program being executed(debugged) atm.

* What is the address of the function main?

	info address main.

* Try info stack. Explain what you see.

	alias for [backtrace], [bt]. Summary of how program got where it is atm,
	backtrace of stack frames.	

* Try info frame. Explain what you see.

	look at the contents of the current frame.

	[info locals] - display the list of local variables and their values for the current stack frame.

	[info args] - display the list of arguments to the current stack frame.

	stack level - how deep in the execution stack one is, stack pointer address.
	address of the next frame down, called by this frame.
	address of the next frame up, caller of this frame.
	language in which the source code corresponding to this frame is written.
	address of the argument list of this frame.
	address of the local varibles of this frame.
	while stack pointer is the pointer to the top of the stack.
	list of the saved registers.

* Try info registers. Which registers are holding aspects of the program that you recognize?

	list of the registers in the format:
	register name | hex value | decimal value

	rax - 64-bit register to store fn return value, if it exists
	and no more than 64 bits long, stack used otherwise.

	rbx, rbp, r12-r15 - callee-save registers, meaning they are saved across fn calls.

	rax, rcx, rdx, rdi, rsi, rsp, r8-r11 - caller-save registers/temp,
	not saved across fn calls.
	rdi, rsi, rdx, rcx, r8, r9 - are used to pass first six 
	integer or pointer parameters to called fns. Additional params or large ones
	like structs are passed on the stack.

	rbp - base pointer to the current stack frame, called fn saves base pointer
	of its caller prior to updating with its own.
	
	rsp - pointer to the top of the current stack frame.
 
	rip - mem address of the next instruction, program counter.
	
	eflags - condition codes.

#### 3.3.2 objdump

```
objdump -x -d [target]
```

-x - display all available header information.

-d - display asm mnemonics for the machine instructions from the object file.

```
objdump -x -d wc.out
```

* What file format is used for this binary? And which arch is it compiled for?

	Elf 64 lsb x86-64

* What are some of the names of segment/sections you find?

	segments - readelf -l [target]
	text, data segment

	sections - readelf -S [target]
	.text, .rodata, .plt; .data, .bss, .got

* What segment/section contains main and what is the address of main?(should be the same as in gdb)

	text segment/text section

	look at disasm of the .text section:

	address <_start>:
	mov   0x406fd, %rdi             # move main address to the 1st parameter register
	callq 4005c <__libc_start_main> # conventional way to start-up application.

	answer: 0x406fd

* Do you see stack segment anywhere? What about the heap? Explain.

	Nope, these are the runtime concepts.

	although there is stack segment on most gnu/linux elfs,
	it is used to mark the stack as non-exec which is form of sec. hardening,
	on certain arch., the size of stack segment is even used by the kernel to
	set the stack size of the main thread.	

#### 3.3.3 map

```
cat map.c

./map.out
```

* Use objdump with the -d flag on the map executable. Which of the addresses from the output of
running ./map.out are defined in the executable, and which segment/section is each defined in?

	_main fn address - text segment, .text section
	recur fn address - text segment, .text section

	static data stuff - data segment, .data section (symbol table)

* Make list of the important segments, and what they are used for.

	name 	| type | flags permissions
-
	text 	| LOAD | Read, Execute

	mapped sections(notable):
	.text, .rodata, .hash, .dynsym, .dynstr, .plt, .rela.dyn, .rela.plt
-
	data	| LOAD | Read, Write

	mapped sections(notable):
	.data, .bss, .got, .dynamic, .got.plt

-
	sections:

	.text - code
	.data - initialised data
	.rodata - initialised read-only data
	.bss - uninitialised data
	.plt - procedure linkage table
	.got - global offset table, dedicated to dynamically linked global vars
	.got.plt - got for dynamically linked functions
	.dynsym - symbol table for dynamic linked symbols
	.dynstr - symbol table of the .dynsym
	.rela.dyn - global var relocation table
	.rela.plt - function relocation table

* What direction is the stack growing in?

	downwards, by subtracting from stack pointer.

* How large is the stack frame for each recursive call?

	0x30 hex = 48 dec
	16*3 = 48 dec

* Where is the heap? What direction is it growing in?

	0xcc5010; upwards

* Are the two malloc()ed mem areas contiguous?(e.g. is there any extra space between their addresses?)

	nope, yes 12 bytes padding.

### 3.4 User limits

user limits for the stack, heap, file descriptors?

```
man getrlimit
```

```
cat main.c

./main.out
```

