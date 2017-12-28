#include "array.h"
#include <string.h>
#include <stdlib.h>
struct Array {
    size_t e_size;
	int capacity;
	int elements;
	void *ar;
};

void init(Array **new_ar, size_t e_size, int initial_capacity) {
    *new_ar = malloc(sizeof(Array));
    (*new_ar)->capacity = initial_capacity;
    (*new_ar)->e_size = e_size;
    (*new_ar)->elements = 0;
    (*new_ar)->ar = malloc(e_size * initial_capacity);
}

void freeArray(Array *ar) {
    free(ar->ar);
    free(ar);
}

void expand(Array *ar) {
    ar->capacity = ar->capacity * 2;
    ar->ar = realloc(ar->ar, ar->capacity * ar->e_size );
}

void *getElement(Array *ar, int index) {
    if (index > ar->elements) {
        return NULL;
    }

    void *val = malloc(ar->e_size);
    memcpy(val, ar->ar + (ar->e_size * index), ar->e_size);
    return val;
}

int insert(Array *ar, void *item, int index) {
    if (index > ar->elements) {
        return 0;
    }

    if (ar->capacity == ar->elements) {
        expand(ar);
    }

    // Push data forward to make room for the new element and copy it in
    memmove(ar->ar + (ar->e_size * (index + 1)), ar->ar + (ar->e_size * index),
            ar->e_size * (ar->elements - index));
    memcpy(ar->ar + (ar->e_size * index), item, ar->e_size);
    ar->elements++;
    return 1;
}

void append(Array *ar, void *item) {
    insert(ar, item, ar->elements); 
}

void *array(Array *ar) {
    void *array = malloc(ar->e_size * ar->elements);
    memcpy(array, ar->ar, ar->e_size * ar->elements);
    return array;
}

int deleteElement(Array *ar, int index) {
    if (index > ar->elements) {
        return 0;
    }

    // Push data back by 1 over the removed index
    memmove(ar->ar + (ar->e_size * index), ar->ar + (ar->e_size * (index + 1)),
            ar->e_size * (ar->elements - (index + 1)));
    ar->elements--;
    return 1;
}

int len(Array *ar) {
    return ar->elements;
}

size_t e_size(Array *ar) {
    return ar->e_size;
}

