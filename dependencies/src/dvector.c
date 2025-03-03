#include <stdio.h> /*size_t*/
#include "dvector.h" /*vector_t*/
#include <stdlib.h> /*malloc*/
#include <assert.h> /*assert*/


struct vector
{
  size_t capacity;
  size_t element_size;
  size_t element_count;
  void *buffer;
};


/********************************************/
/*			   API Functions				*/
/********************************************/


vector_t *VectorCreate(size_t capacity, size_t element_size)
{
	vector_t *vector = (vector_t *)malloc(sizeof(vector_t));
	char *vector_data = (char *)malloc(capacity * element_size);
		
	if (NULL == vector)
	{
		return NULL;
	}

	if (NULL == vector_data)
	{
		return NULL;
	}
	
	(vector) -> capacity = capacity;
	(vector) -> element_size = element_size;
	(vector) -> element_count = 0;
	(vector) -> buffer = vector_data;
		
	return vector;
}

void VectorDestroy(vector_t *vector)
{
	assert(NULL != vector);
	free(vector -> buffer);
	free(vector);
}

void *VectorAccessVal(vector_t *vector, size_t index)
{
	assert(vector != NULL);
	
	return (char *)vector -> buffer + (index * vector -> element_size );
}

size_t VectorElementCount(const vector_t *vector)
{
	assert(NULL != vector);
	return vector -> element_count;
}

size_t VectorCapacity(const vector_t *vector)
{
	assert(NULL != vector);
	return vector -> capacity;
}

int VectorPushBack(vector_t *vector, const void *new_element_data)
{
	char *temp = (char *)new_element_data;
	size_t vector_size = vector -> element_count * vector -> element_size;
	size_t i = 0;
	
	assert(vector != NULL);

	if (vector -> element_count == vector->capacity) 
	{
		size_t new_capacity = vector -> capacity * GROWTH_FACTOR;
		char *new_buffer = realloc(vector -> buffer, new_capacity * vector -> element_size);
		
		if (NULL == new_buffer) 
		{
			return -1;
		}

		vector -> buffer = new_buffer;
		vector -> capacity = new_capacity;
	}
	
	for( i = 0; i < vector -> element_size; i++ , ++temp)
	{
		*((char *)vector -> buffer + vector_size + i) = *temp;
	}
	
	++vector -> element_count;

	return 0;

}

void VectorPopBack(vector_t *vector)
{
	assert(NULL != vector);
	
	--vector -> element_count;
	
	if (0 != vector -> element_count && vector -> element_count <= DECREASE_FACTOR * vector -> capacity)
	{
		VectorShrink(vector);
	}
}

void VectorShrink(vector_t *vector)
{
	assert(NULL != vector);
	
	vector -> buffer = realloc(vector -> buffer, vector -> element_count * vector -> element_size);
	vector -> capacity = vector -> element_count;
}

int VectorReserve(vector_t *vector, size_t n_new_elements)
{
	char *new_buffer = realloc(vector -> buffer, (vector -> capacity + n_new_elements) * vector -> element_size);
	
	assert(NULL != vector);
	
	if (NULL == new_buffer) 
	{
			return -1;
	}
	
	vector -> capacity = vector -> capacity + n_new_elements;
	vector -> buffer = new_buffer;
	
	return 0;
}


