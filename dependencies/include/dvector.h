/* filename: dynamic_vector.h */

#ifndef __VECTOR_H__
#define __VECTOR_H__

#include <stddef.h> /* size_t */


#define GROWTH_FACTOR 2
#define DECREASE_FACTOR 0.5


/*
in the .c file:
struct vector
{
  size_t capacity;
  size_t element_size;
  size_t element_count;
  void *buffer; 
};
*/

typedef struct vector vector_t;

/*
Description: Create vector
Params: (size_t) capacity, (size_t) size of each element
Return Value: pointer to vector
Time Complexity: O(1)
Space: O(n)
*/
vector_t *VectorCreate(size_t capacity, size_t element_size);

/*
Description: Destroy vector
Params: (vector_t*) ptr to vector 
Return Value: void
Time Complexity: O(1)
Space: O(1)
*/
void VectorDestroy(vector_t *vector);

/*
Description: Access a value in the Dynamic Vector
Params: (vector_t*) ptr to vector, (size_t) index
Return Value: pointer to index value in vector
Time Complexity: O(1)
Space: O(1)
*/
void *VectorAccessVal(vector_t *vector, size_t index);

/*
Description: Get the current number of elements in the Dynamic Vector
Params: (vector_t*) ptr to vector
Return Value: (size_t) current number of elements
Time Complexity: O(1)
Space: O(1)
*/
size_t VectorElementCount(const vector_t *vector);

/*
Description: Get the current capacity of elements in the Dynamic Vector
Params: (vector_t*) ptr to vector
Return Value: (size_t) current capacity
Time Complexity: O(1)
Space: O(1)
*/
size_t VectorCapacity(const vector_t *vector);

/*
Description: Adds an element to the end of the Dynamic Vector
Params: (vector_t *) ptr to vector, (void*) ptr to value of elemnt to add
Return Value: (int) boolean, success - 0, failure - 1
Time Complexity: O(n)
Space: O(n)
*/
int VectorPushBack(vector_t *vector, const void *new_element_data);

/*
Description: Remove last element, and shrinks the vector when reaching a shrink factor? 
Params: (vector_t*) ptr to vector
Return Value: void
Time Complexity: O(1)
Space: O(n)
Undefined behavior: when using this function, accessing previously used data could lead to undefined behavior
*/
void VectorPopBack(vector_t *vector);

/*
Description: Shrink vector (to accomodate only the elements in it)
Params: (vector_t*) ptr to vector
Return Value: (vector_t*) ptr to vector(?)
Time Complexity: O(1)
Space: O(n)
Undefined behavior: when using this function, accessing previously used data could lead to undefined behavior
*/
void VectorShrink(vector_t *vector);

/*
Description: Expand vector to accomodate n new elements later
Params: (vector_t*) ptr to vector, (size_t) n_new_elements (buffer to add)
Return Value: (vector_t*) ptr to vector
Time Complexity: O(n)
Space: O(n)
Undefined behavior: when using this function, accessing previously used data could lead to undefined behavior
*/
int VectorReserve(vector_t *vector, size_t n_new_elements);

#endif /* __VECTOR_H__ */
