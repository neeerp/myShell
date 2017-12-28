#ifndef ARRAY_H_
#define ARRAY_H_

#include <stdlib.h>

struct Array;
typedef struct Array Array;

/** Initialize a new array */
void init(Array **ar, size_t e_size, int initial_capacity);

/** Free the array */
void freeArray(Array *ar);

/** Returns a pointer to a copy of the item at the given array index */
void *getElement(Array *ar, int index);

/** Inserts an item at the given index in the array, returning 0 if
 * the insertion failed */
int insert(Array *ar, void *item, int index);

/** Inserts an element at the end of the array */
void append(Array *ar, void *item);

/** Deletes the element at the given index, returning 0 if the
 * deletion failed */
int deleteElement(Array *ar, int index);

/** Returns a copy of the Array's actual array */
void *array(Array *ar);

/** Returns the current number of elements in the array */
int len(Array *ar);

/** Returns the size of the elements contained in this array */
size_t e_size(Array *ar); 

#endif

