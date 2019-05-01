#define _GNU_SOURCE (1)
#include "mm_alloc.h"
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

struct mem_block {
	struct mem_block *next;
	struct mem_block *previous;
	size_t size;
	char mem[0];
};

struct mem_block *mem_list;
struct mem_block *free_list;

#define HEADER_SIZE (sizeof(struct mem_block))

static void display_list(struct mem_block *head, char *name) {
	fprintf(stdout, "display %s list\n", name);
	struct mem_block *cursor;
	fprintf(stdout, "[");
	for (cursor = head; cursor; cursor = cursor->next) {
		printf("%lu%s", cursor->size, cursor->next ? ", " : "");
	}
	fprintf(stdout, "]\n");
}

void *mm_malloc(size_t size) {
	if (!size) {
		return NULL;
	}
	struct mem_block *block = NULL;

	/* scan free list for available big enough block */
	struct mem_block *cursor;
	for (cursor = free_list; cursor; cursor = cursor->next) {
		if (cursor->size >= size) {
			block = cursor;
			goto detach_from_free_list;
		}
	}

	/* create mem block */
	block = sbrk(HEADER_SIZE + size);
	if (block == (void *) -1) {
		perror("malloc");
		exit(EXIT_FAILURE);
	}
	goto insert_into_mem_list;

detach_from_free_list:
	// display_list(free_list, "free");

	if (block->previous)
		block->previous->next = block->next;

	if (block->next) {
		block->next->previous = block->previous;
	}

	if (free_list == block)
		free_list = block->next;

	// display_list(free_list, "free");
	// display_list(mem_list, "memory");

insert_into_mem_list:

	/* temp reuse mem block hoop */
	if (!block->size)
		block->size = size;

	memset(block->mem, 0, size);

	/* empty memory list */
	if (!mem_list) {
		mem_list = block;
		block->next = NULL;
		block->previous = NULL;
		// display_list(mem_list, "memory");
		return block->mem;
	}

	block->next = mem_list;
	mem_list->previous = block;
	block->previous = NULL;
	mem_list = block;

	// display_list(mem_list, "memory");
	// display_list(free_list, "free");

	return block->mem;
}

void *mm_realloc(void *ptr, size_t size) {
	if (!size) {
		if (ptr)
			mm_free(ptr);

		return NULL;
	}

	/* realloc */
	struct mem_block *doomed = ptr - HEADER_SIZE;

	struct mem_block *block = mm_malloc(size) - HEADER_SIZE;
	memcpy(block->mem, doomed->mem, doomed->size);

	mm_free(doomed->mem);

	// display_list(mem_list, "_memory");
	// display_list(free_list, "_free");

	return block->mem;
}

void mm_free(void *ptr) {
	if (!ptr) {
		exit(EXIT_FAILURE);
	}

	struct mem_block *doomed = ptr - HEADER_SIZE;

	/* detach doomed block from memory list
	 *
	 * doomed block head of memory list
	 */
	if (doomed == mem_list) {
		mem_list = doomed->next;
		goto insert_into_free_list;
	}

	if (doomed->previous)
		doomed->previous->next = doomed->next;

	/* if doomed block not last in memory list */
	if (doomed->next)
		doomed->next->previous = doomed->previous;

insert_into_free_list:;

	/* empty free list */
	if (!free_list) {
		free_list = doomed;
		doomed->previous = NULL;
		doomed->next = NULL;
		// display_list(free_list, "free");
		// display_list(mem_list, "memory");
		return;
	}

	/* find place for a doomed node */
	struct mem_block *cursor;
	for (cursor = free_list; cursor->next && doomed >= cursor; cursor = cursor->next);

	/* if cursor is head of the free list and address >= than doomed node */
	if (cursor == free_list && doomed < cursor) {
		doomed->previous = NULL;
		doomed->next = cursor;
		cursor->previous = doomed;
		free_list = doomed;
		// display_list(mem_list, "memory");
		// display_list(free_list, "free");
		return;
	}

	/* insert after */
	doomed->next = cursor->next;
	if (cursor->next) {
		cursor->next->previous = doomed;
	}
	cursor->next = doomed;
	doomed->previous = cursor;
	// display_list(mem_list, "memory");
	// display_list(free_list, "free");
}
