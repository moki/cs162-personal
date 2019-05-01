#include <assert.h>
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>

/* Function pointers to hw3 functions */
void* (*mm_malloc)(size_t);
void* (*mm_realloc)(void*, size_t);
void (*mm_free)(void*);

void load_alloc_functions() {
    void *handle = dlopen("hw3lib.so", RTLD_NOW);
    if (!handle) {
        fprintf(stderr, "%s\n", dlerror());
        exit(1);
    }

    char* error;
    mm_malloc = dlsym(handle, "mm_malloc");
    if ((error = dlerror()) != NULL)  {
        fprintf(stderr, "%s\n", dlerror());
        exit(1);
    }

    mm_realloc = dlsym(handle, "mm_realloc");
    if ((error = dlerror()) != NULL)  {
        fprintf(stderr, "%s\n", dlerror());
        exit(1);
    }

    mm_free = dlsym(handle, "mm_free");
    if ((error = dlerror()) != NULL)  {
        fprintf(stderr, "%s\n", dlerror());
        exit(1);
    }
}

int main() {
    load_alloc_functions();

    int *data = (int*) mm_malloc(sizeof(int));
    assert(data != NULL);
    data[0] = 0x162;

    size_t *_data = (size_t *) mm_malloc(sizeof(size_t));
    assert(_data != NULL);
    _data[0] = 0x162;

    char *__data = (char *) mm_malloc(sizeof(char));
    assert(__data != NULL);
    *__data = 'c';

    mm_free(data);
    mm_free(_data);
    mm_free(__data);

    data = (int *) mm_malloc(sizeof(int));
    assert(data != NULL);
    data[0] = 0x162;

    mm_free(data);

    __data = (char *) mm_malloc(sizeof(char));
    assert(__data != NULL);
    *__data = 'c';

    __data = (char *) mm_realloc(__data, 3);

    __data[0] = 'c';
    __data[1] = 'h';
    __data[2] = '\0';

    __data = (char *) mm_realloc(__data, 4);

    mm_free(__data);

    printf("malloc test successful!\n");
    return 0;
}
