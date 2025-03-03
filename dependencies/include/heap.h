#ifndef __HEAP_H__
#define __HEAP_H__

#include <stddef.h> /* size_t */

typedef struct heap heap_t;

/*
Description: typedef to pointer function that compares two arguments
Params: (const void *, const void *) two elements to compare
Return Value: (int)
    positive if first argument is larger,
    zero if both arguments are equal,
    negative if the second argument is larger.
*/
typedef int (*heap_compare_func_t)(const void*, const void*);

/*
Description: typedef to pointer function that matches two arguments
Params: (const void *, const void *) two elements to compare
Return Value: (int)
    1 if matched
    0 if not
*/
typedef int (*heap_match_func_t)(const void *data1, const void *data2);


/*
HeapCreate
description: create a heap
input: comparison function for heap element comparisons
return: a pointer to a heap, if failed returns NULL
time complexity: O(1)
space complexity: O(1)
*/
heap_t *HeapCreate(heap_compare_func_t compare_func);


/*
HeapDestroy
description: frees the heap and all of its elements
input: pointer to a heap
return: void
time complexity: O(1)
space complexity: O(1)
*/
void HeapDestroy(heap_t *heap);


/*
HeapPush
description: Adds new element to the heap
input: pointer to heap, pointer to client data.
return: success status: 0 for success, non 0 - failure.
time complexity: amortized O(log(n)) 
space complexity: amortized O(1)
*/
int HeapPush(heap_t *heap, void *data);


/*
HeapPop
description: pops and returns the top element of the heap, according to the hierarchy that is set by comparison function.
input: pointer to heap.
return: void * - data that is popped. Undefined if heap is empty.
time complexity: O(log(n))
space complexity: O(1)
*/
void HeapPop(heap_t *heap);


/*
HeapPeek
description: returns the top element of the heap, according to the hierarchy
that is set by comparison function.
input: pointer to heap.
return: void pointer to the data. Undefined if heap is empty.
time complexity: O(1)
space complexity: O(1)
*/
void *HeapPeek(const heap_t *heap);


/*
HeapSize
description: returns the number of elements in the heap.
input: pointer to heap.
return: number of elements in the heap.
time complexity: O(1)
space complexity: O(1)
*/
size_t HeapSize(const heap_t *heap);


/*
HeapIsEmpty
description: checks if the heap is empty.
input: pointer to heap
return: 1 if heap is empty, 0 otherwise
time complexity: O(1)
space complexity: O(1)
*/
int HeapIsEmpty(const heap_t *heap);

/*
HeapRemove
description: Removes element from the heap where the data matches the argument.
if multiple entires match, removes the first one it finds.
input: pointer to heap, a match function, pointer to data to remove.
return: 0 for success, non-zero if failed.
time complexity: O(n)
space complexity: O(1)
*/
void *HeapRemove(heap_t *heap, heap_match_func_t is_match, void *data_to_match);

#endif /* __HEAP_H__ */
